#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "ReverbEffect.h"
#include "efx-presets.h"

using namespace std;

struct AppConfig {
    string inputName;
    string outputName;
    int presetId;
    float mixLevel;
    bool isEnabled;

    bool isCustomized;
    float t_decayTime;
    float t_density;
    float t_reflectionsDelay;
    float t_gainHF;

    float t_diffusion;
    float t_gain;
    float t_decayHFRatio;
    float t_reflectionsGain;
    float t_lateReverbGain;
    float t_lateReverbDelay;
    float t_airAbsorptionGainHF;
    float t_roomRolloffFactor;
};

ReverbEffect g_reverbEffect;
AppConfig g_config = { "", "", 8, 1.0f, true, false, 1.49f, 1.0f, 0.007f, 0.89f, 1.0f, 0.32f, 0.83f, 0.05f, 1.26f, 0.011f, 0.994f, 0.0f };
int g_sampleRate = 48000;

enum AppState { STATE_DASHBOARD, STATE_EDIT };
AppState g_state = STATE_DASHBOARD;
int g_editCursor = 0; 

float Clamp(float v, float minV, float maxV) {
    if (v < minV) return minV;
    if (v > maxV) return maxV;
    return v;
}

EFXEAXREVERBPROPERTIES GetPresetProperties(int preset)
{
    switch (preset)
    {
    case 0: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_GENERIC;
    case 1: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PADDEDCELL;
    case 2: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ROOM;
    case 3: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_BATHROOM;
    case 4: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_LIVINGROOM;
    case 5: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_STONEROOM;
    case 6: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_AUDITORIUM;
    case 7: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CONCERTHALL;
    case 8: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CAVE;
    case 9: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ARENA;
    case 10: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_HANGAR;
    case 11: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CARPETEDHALLWAY;
    case 12: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_HALLWAY;
    case 13: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_STONECORRIDOR;
    case 14: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ALLEY;
    case 15: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FOREST;
    case 16: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CITY;
    case 17: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_MOUNTAINS;
    case 18: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_QUARRY;
    case 19: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PLAIN;
    case 20: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PARKINGLOT;
    case 21: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SEWERPIPE;
    case 22: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_UNDERWATER;
    case 23: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRUGGED;
    case 24: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DIZZY;
    case 25: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PSYCHOTIC;
    case 26: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_SMALLROOM;
    case 27: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE;
    case 28: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_MEDIUMROOM;
    case 29: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_LARGEROOM;
    case 30: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_LONGPASSAGE;
    case 31: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_HALL;
    case 32: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_CUPBOARD;
    case 33: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_COURTYARD;
    case 34: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CASTLE_ALCOVE;
    case 35: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_SMALLROOM;
    case 36: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE;
    case 37: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_MEDIUMROOM;
    case 38: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_LARGEROOM;
    case 39: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_LONGPASSAGE;
    case 40: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_HALL;
    case 41: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_CUPBOARD;
    case 42: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_COURTYARD;
    case 43: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_FACTORY_ALCOVE;
    case 44: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_SMALLROOM;
    case 45: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE;
    case 46: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM;
    case 47: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_LARGEROOM;
    case 48: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE;
    case 49: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_HALL;
    case 50: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_CUPBOARD;
    case 51: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_COURTYARD;
    case 52: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_ICEPALACE_ALCOVE;
    case 53: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_SMALLROOM;
    case 54: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE;
    case 55: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM;
    case 56: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_LARGEROOM;
    case 57: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE;
    case 58: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_HALL;
    case 59: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_CUPBOARD;
    case 60: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPACESTATION_ALCOVE;
    case 61: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_SMALLROOM;
    case 62: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE;
    case 63: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_MEDIUMROOM;
    case 64: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_LARGEROOM;
    case 65: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_LONGPASSAGE;
    case 66: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_HALL;
    case 67: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_CUPBOARD;
    case 68: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_COURTYARD;
    case 69: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_WOODEN_ALCOVE;
    case 70: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM;
    case 71: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPORT_SQUASHCOURT;
    case 72: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL;
    case 73: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL;
    case 74: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPORT_GYMNASIUM;
    case 75: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPORT_FULLSTADIUM;
    case 76: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SPORT_STADIUMTANNOY;
    case 77: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PREFAB_WORKSHOP;
    case 78: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PREFAB_SCHOOLROOM;
    case 79: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PREFAB_PRACTISEROOM;
    case 80: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PREFAB_OUTHOUSE;
    case 81: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PREFAB_CARAVAN;
    case 82: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DOME_TOMB;
    case 83: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PIPE_SMALL;
    case 84: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DOME_SAINTPAULS;
    case 85: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PIPE_LONGTHIN;
    case 86: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PIPE_LARGE;
    case 87: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_PIPE_RESONANT;
    case 88: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_OUTDOORS_BACKYARD;
    case 89: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS;
    case 90: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON;
    case 91: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_OUTDOORS_CREEK;
    case 92: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_OUTDOORS_VALLEY;
    case 93: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_MOOD_HEAVEN;
    case 94: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_MOOD_HELL;
    case 95: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_MOOD_MEMORY;
    case 96: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_COMMENTATOR;
    case 97: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_PITGARAGE;
    case 98: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_INCAR_RACER;
    case 99: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS;
    case 100: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY;
    case 101: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND;
    case 102: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND;
    case 103: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DRIVING_TUNNEL;
    case 104: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CITY_STREETS;
    case 105: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CITY_SUBWAY;
    case 106: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CITY_MUSEUM;
    case 107: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CITY_LIBRARY;
    case 108: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CITY_UNDERPASS;
    case 109: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CITY_ABANDONED;
    case 110: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_DUSTYROOM;
    case 111: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_CHAPEL;
    case 112: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_SMALLWATERROOM;
    case 113: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_HAUNTED_CAVERN; // Haunted Cavern
    default: return EFXEAXREVERBPROPERTIES EFX_REVERB_PRESET_GENERIC;
    }
}

const char* GetPresetName(int preset)
{
    switch (preset)
    {
    case 0: return "GENERIC";
    case 1: return "PADDEDCELL";
    case 2: return "ROOM";
    case 3: return "BATHROOM";
    case 4: return "LIVINGROOM";
    case 5: return "STONEROOM";
    case 6: return "AUDITORIUM";
    case 7: return "CONCERTHALL";
    case 8: return "CAVE";
    case 9: return "ARENA";
    case 10: return "HANGAR";
    case 11: return "CARPETEDHALLWAY";
    case 12: return "HALLWAY";
    case 13: return "STONECORRIDOR";
    case 14: return "ALLEY";
    case 15: return "FOREST";
    case 16: return "CITY";
    case 17: return "MOUNTAINS";
    case 18: return "QUARRY";
    case 19: return "PLAIN";
    case 20: return "PARKINGLOT";
    case 21: return "SEWERPIPE";
    case 22: return "UNDERWATER";
    case 23: return "DRUGGED";
    case 24: return "DIZZY";
    case 25: return "PSYCHOTIC";
    case 26: return "CASTLE_SMALLROOM";
    case 27: return "CASTLE_SHORTPASSAGE";
    case 28: return "CASTLE_MEDIUMROOM";
    case 29: return "CASTLE_LARGEROOM";
    case 30: return "CASTLE_LONGPASSAGE";
    case 31: return "CASTLE_HALL";
    case 32: return "CASTLE_CUPBOARD";
    case 33: return "CASTLE_COURTYARD";
    case 34: return "CASTLE_ALCOVE";
    case 35: return "FACTORY_SMALLROOM";
    case 36: return "FACTORY_SHORTPASSAGE";
    case 37: return "FACTORY_MEDIUMROOM";
    case 38: return "FACTORY_LARGEROOM";
    case 39: return "FACTORY_LONGPASSAGE";
    case 40: return "FACTORY_HALL";
    case 41: return "FACTORY_CUPBOARD";
    case 42: return "FACTORY_COURTYARD";
    case 43: return "FACTORY_ALCOVE";
    case 44: return "ICEPALACE_SMALLROOM";
    case 45: return "ICEPALACE_SHORTPASSAGE";
    case 46: return "ICEPALACE_MEDIUMROOM";
    case 47: return "ICEPALACE_LARGEROOM";
    case 48: return "ICEPALACE_LONGPASSAGE";
    case 49: return "ICEPALACE_HALL";
    case 50: return "ICEPALACE_CUPBOARD";
    case 51: return "ICEPALACE_COURTYARD";
    case 52: return "ICEPALACE_ALCOVE";
    case 53: return "SPACESTATION_SMALLROOM";
    case 54: return "SPACESTATION_SHORTPASSAGE";
    case 55: return "SPACESTATION_MEDIUMROOM";
    case 56: return "SPACESTATION_LARGEROOM";
    case 57: return "SPACESTATION_LONGPASSAGE";
    case 58: return "SPACESTATION_HALL";
    case 59: return "SPACESTATION_CUPBOARD";
    case 60: return "SPACESTATION_ALCOVE";
    case 61: return "WOODEN_SMALLROOM";
    case 62: return "WOODEN_SHORTPASSAGE";
    case 63: return "WOODEN_MEDIUMROOM";
    case 64: return "WOODEN_LARGEROOM";
    case 65: return "WOODEN_LONGPASSAGE";
    case 66: return "WOODEN_HALL";
    case 67: return "WOODEN_CUPBOARD";
    case 68: return "WOODEN_COURTYARD";
    case 69: return "WOODEN_ALCOVE";
    case 70: return "SPORT_EMPTYSTADIUM";
    case 71: return "SPORT_SQUASHCOURT";
    case 72: return "SPORT_SMALLSWIMMINGPOOL";
    case 73: return "SPORT_LARGESWIMMINGPOOL";
    case 74: return "SPORT_GYMNASIUM";
    case 75: return "SPORT_FULLSTADIUM";
    case 76: return "SPORT_STADIUMTANNOY";
    case 77: return "PREFAB_WORKSHOP";
    case 78: return "PREFAB_SCHOOLROOM";
    case 79: return "PREFAB_PRACTISEROOM";
    case 80: return "PREFAB_OUTHOUSE";
    case 81: return "PREFAB_CARAVAN";
    case 82: return "DOME_TOMB";
    case 83: return "PIPE_SMALL";
    case 84: return "DOME_SAINTPAULS";
    case 85: return "PIPE_LONGTHIN";
    case 86: return "PIPE_LARGE";
    case 87: return "PIPE_RESONANT";
    case 88: return "OUTDOORS_BACKYARD";
    case 89: return "OUTDOORS_ROLLINGPLAINS";
    case 90: return "OUTDOORS_DEEPCANYON";
    case 91: return "OUTDOORS_CREEK";
    case 92: return "OUTDOORS_VALLEY";
    case 93: return "MOOD_HEAVEN";
    case 94: return "MOOD_HELL";
    case 95: return "MOOD_MEMORY";
    case 96: return "DRIVING_COMMENTATOR";
    case 97: return "DRIVING_PITGARAGE";
    case 98: return "DRIVING_INCAR_RACER";
    case 99: return "DRIVING_INCAR_SPORTS";
    case 100: return "DRIVING_INCAR_LUXURY";
    case 101: return "DRIVING_FULLGRANDSTAND";
    case 102: return "DRIVING_EMPTYGRANDSTAND";
    case 103: return "DRIVING_TUNNEL";
    case 104: return "CITY_STREETS";
    case 105: return "CITY_SUBWAY";
    case 106: return "CITY_MUSEUM";
    case 107: return "CITY_LIBRARY";
    case 108: return "CITY_UNDERPASS";
    case 109: return "CITY_ABANDONED";
    case 110: return "DUSTYROOM";
    case 111: return "CHAPEL";
    case 112: return "SMALLWATERROOM";
    case 113: return "HAUNTED_CAVERN"; 
    default: return "GENERIC";
    }
}

void ApplyReverb() {
    EFXEAXREVERBPROPERTIES props = GetPresetProperties(g_config.presetId);

    if (g_config.isCustomized) {
        props.flDecayTime = g_config.t_decayTime;
        props.flDensity = g_config.t_density;
        props.flReflectionsDelay = g_config.t_reflectionsDelay;
        props.flGainHF = g_config.t_gainHF;

        props.flDiffusion = g_config.t_diffusion;
        props.flGain = g_config.t_gain;
        props.flDecayHFRatio = g_config.t_decayHFRatio;
        props.flReflectionsGain = g_config.t_reflectionsGain;
        props.flLateReverbGain = g_config.t_lateReverbGain;
        props.flLateReverbDelay = g_config.t_lateReverbDelay;
        props.flAirAbsorptionGainHF = g_config.t_airAbsorptionGainHF;
        props.flRoomRolloffFactor = g_config.t_roomRolloffFactor;
    }

    ReverbEffectLoadPreset(&g_reverbEffect, &props);
    ReverbEffectUpdate(&g_reverbEffect, g_sampleRate);
}

void StartCustomizing() {
    if (!g_config.isCustomized) {
        EFXEAXREVERBPROPERTIES p = GetPresetProperties(g_config.presetId);
        g_config.t_decayTime = p.flDecayTime;
        g_config.t_density = p.flDensity;
        g_config.t_reflectionsDelay = p.flReflectionsDelay;
        g_config.t_gainHF = p.flGainHF;

        g_config.t_diffusion = p.flDiffusion;
        g_config.t_gain = p.flGain;
        g_config.t_decayHFRatio = p.flDecayHFRatio;
        g_config.t_reflectionsGain = p.flReflectionsGain;
        g_config.t_lateReverbGain = p.flLateReverbGain;
        g_config.t_lateReverbDelay = p.flLateReverbDelay;
        g_config.t_airAbsorptionGainHF = p.flAirAbsorptionGainHF;
        g_config.t_roomRolloffFactor = p.flRoomRolloffFactor;

        g_config.isCustomized = true;
    }
}

void SaveConfig() {
    ofstream out("reverb_config.txt");
    if (out.is_open()) {
        out << g_config.inputName << "\n";
        out << g_config.outputName << "\n";
        out << g_config.presetId << "\n";
        out << g_config.mixLevel << "\n";
        out << g_config.isEnabled << "\n";
        out << g_config.isCustomized << "\n";
        out << g_config.t_decayTime << "\n";
        out << g_config.t_density << "\n";
        out << g_config.t_reflectionsDelay << "\n";
        out << g_config.t_gainHF << "\n";

        // حفظ القيم الجديدة
        out << g_config.t_diffusion << "\n";
        out << g_config.t_gain << "\n";
        out << g_config.t_decayHFRatio << "\n";
        out << g_config.t_reflectionsGain << "\n";
        out << g_config.t_lateReverbGain << "\n";
        out << g_config.t_lateReverbDelay << "\n";
        out << g_config.t_airAbsorptionGainHF << "\n";
        out << g_config.t_roomRolloffFactor << "\n";
    }
}

bool LoadConfig() {
    ifstream in("reverb_config.txt");
    if (in.is_open()) {
        getline(in, g_config.inputName);
        if (!g_config.inputName.empty() && g_config.inputName.back() == '\r') g_config.inputName.pop_back();

        getline(in, g_config.outputName);
        if (!g_config.outputName.empty() && g_config.outputName.back() == '\r') g_config.outputName.pop_back();

        in >> g_config.presetId;
        in >> g_config.mixLevel;
        in >> g_config.isEnabled;
        if (in >> g_config.isCustomized) {
            in >> g_config.t_decayTime;
            in >> g_config.t_density;
            in >> g_config.t_reflectionsDelay;
            in >> g_config.t_gainHF;
            if (in >> g_config.t_diffusion) {
                in >> g_config.t_gain;
                in >> g_config.t_decayHFRatio;
                in >> g_config.t_reflectionsGain;
                in >> g_config.t_lateReverbGain;
                in >> g_config.t_lateReverbDelay;
                in >> g_config.t_airAbsorptionGainHF;
                in >> g_config.t_roomRolloffFactor;
            }
        }
        else {
            g_config.isCustomized = false;
        }
        return true;
    }
    return false;
}

void PrintUI() {
    system("cls"); 
    cout << "==========================================================" << endl;
    cout << "      EAX Reverb Audio Router (VoiceMeeter Edition)       " << endl;
    cout << "==========================================================\n" << endl;

    cout << " [ ROUTING ]" << endl;
    cout << "  Input  : " << (g_config.inputName.empty() ? "Default" : g_config.inputName) << endl;
    cout << "  Output : " << (g_config.outputName.empty() ? "Default" : g_config.outputName) << endl;

    cout << "\n [ DSP EFFECT ]" << endl;
    cout << "  Status : " << (g_config.isEnabled ? "[ 🟢 ON ]" : "[ 🔴 OFF ]") << endl;
    cout << "  Preset : " << GetPresetName(g_config.presetId) << (g_config.isCustomized ? " (CUSTOM)" : "") << endl;

    if (g_state == STATE_DASHBOARD) {
        cout << "  Mix    : " << (int)(g_config.mixLevel * 100) << "% \n" << endl;
        cout << "==========================================================" << endl;
        cout << " [ HOTKEYS ]" << endl;
        cout << "  [ T ]          -> Toggle Reverb ON/OFF" << endl;
        cout << "  [ Left/Right ] -> Change Reverb Preset" << endl;
        cout << "  [ E ]          -> Edit Parameters (Room Size, Decay..)" << endl;
        cout << "  [ ESC ]        -> Exit Program" << endl;
        cout << "==========================================================" << endl;
    }
    else {
        cout << "\n----------------------------------------------------------" << endl;
        cout << " [ EDIT MODE ] Use Up/Down to select, Left/Right to change" << endl;

        EFXEAXREVERBPROPERTIES p = GetPresetProperties(g_config.presetId);
        float currentDecay = g_config.isCustomized ? g_config.t_decayTime : p.flDecayTime;
        float currentDensity = g_config.isCustomized ? g_config.t_density : p.flDensity;
        float currentDelay = g_config.isCustomized ? g_config.t_reflectionsDelay : p.flReflectionsDelay;
        float currentDamping = g_config.isCustomized ? g_config.t_gainHF : p.flGainHF;
        float currentDiff = g_config.isCustomized ? g_config.t_diffusion : p.flDiffusion;
        float currentGain = g_config.isCustomized ? g_config.t_gain : p.flGain;
        float currentHFRatio = g_config.isCustomized ? g_config.t_decayHFRatio : p.flDecayHFRatio;
        float currentReflGain = g_config.isCustomized ? g_config.t_reflectionsGain : p.flReflectionsGain;
        float currentLateGain = g_config.isCustomized ? g_config.t_lateReverbGain : p.flLateReverbGain;
        float currentLateDelay = g_config.isCustomized ? g_config.t_lateReverbDelay : p.flLateReverbDelay;

        cout << (g_editCursor == 0 ? " >> " : "    ") << "1. Wet Mix Level     : " << (int)(g_config.mixLevel * 100) << " %" << endl;
        cout << (g_editCursor == 1 ? " >> " : "    ") << "2. Decay Time        : " << currentDecay << " s" << endl;
        cout << (g_editCursor == 2 ? " >> " : "    ") << "3. Room Size/Density : " << (int)(currentDensity * 100) << " %" << endl;
        cout << (g_editCursor == 3 ? " >> " : "    ") << "4. Pre-Delay         : " << (int)(currentDelay * 1000) << " ms" << endl;
        cout << (g_editCursor == 4 ? " >> " : "    ") << "5. Damping (HF)      : " << (int)((1.0f - currentDamping) * 100) << " %" << endl;
        cout << (g_editCursor == 5 ? " >> " : "    ") << "6. Diffusion         : " << (int)(currentDiff * 100) << " %" << endl;
        cout << (g_editCursor == 6 ? " >> " : "    ") << "7. Master Gain       : " << currentGain << endl;
        cout << (g_editCursor == 7 ? " >> " : "    ") << "8. Decay HF Ratio    : " << currentHFRatio << endl;
        cout << (g_editCursor == 8 ? " >> " : "    ") << "9. Reflections Gain  : " << currentReflGain << endl;
        cout << (g_editCursor == 9 ? " >> " : "    ") << "10. Late Reverb Gain : " << currentLateGain << endl;
        cout << (g_editCursor == 10 ? " >> " : "   ") << "11. Late Reverb Delay: " << (int)(currentLateDelay * 1000) << " ms" << endl;

        cout << "\n==========================================================" << endl;
        cout << "  [ R ] -> Restore to Default Preset Values" << endl;
        cout << "  [ 0 ] -> Back to Dashboard" << endl;
        cout << "==========================================================" << endl;
    }
}

bool CheckKey(int vKey, bool& state) {
    bool isPressed = (GetAsyncKeyState(vKey) & 0x8000) != 0;
    if (isPressed && !state) { state = true; return true; }
    if (!isPressed) { state = false; }
    return false;
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    float* pOut = (float*)pOutput;
    const float* pIn = (const float*)pInput;

    if (pIn == NULL || pOut == NULL) return;

    if (!g_config.isEnabled) {
        memcpy(pOut, pIn, frameCount * pDevice->capture.channels * sizeof(float));
        return;
    }

    float* monoInput = new float[frameCount];
    for (ma_uint32 i = 0; i < frameCount; i++) {
        monoInput[i] = (pIn[i * 2] + pIn[i * 2 + 1]) / 2.0f;
    }

    memset(pOut, 0, frameCount * 2 * sizeof(float));

    ma_uint32 offset = 0;
    ma_uint32 remaining = frameCount;

    while (remaining > 0)
    {
        ma_uint32 currentSamples = (remaining > 256) ? 256 : remaining;

        ReverbEffectProcess(&g_reverbEffect, currentSamples, &monoInput[offset], &pOut[offset * 2]);

        for (ma_uint32 i = 0; i < currentSamples; i++)
        {
            pOut[(offset + i) * 2 + 0] *= g_config.mixLevel;
            pOut[(offset + i) * 2 + 1] *= g_config.mixLevel;

            pOut[(offset + i) * 2 + 0] += pIn[(offset + i) * 2 + 0];
            pOut[(offset + i) * 2 + 1] += pIn[(offset + i) * 2 + 1];
        }

        remaining -= currentSamples;
        offset += currentSamples;
    }

    delete[] monoInput;
}

int main()
{
    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        cout << "Failed to initialize miniaudio context." << endl;
        return -1;
    }

    ma_device_info* pPlaybackInfos;
    ma_uint32 playbackCount;
    ma_device_info* pCaptureInfos;
    ma_uint32 captureCount;

    if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS) {
        cout << "Failed to enumerate devices." << endl;
        ma_context_uninit(&context);
        return -1;
    }

    if (!LoadConfig()) {
        cout << "========================================" << endl;
        cout << "       FIRST TIME SETUP WIZARD          " << endl;
        cout << "========================================\n" << endl;

        cout << "[ Source Devices (Input) ]" << endl;
        cout << "0. Default System Input" << endl;
        for (ma_uint32 i = 0; i < captureCount; i++) {
            cout << i + 1 << ". " << pCaptureInfos[i].name << endl;
        }

        int inputChoice = 0;
        cout << "\nSelect Source Device (0 - " << captureCount << "): ";
        cin >> inputChoice;
        if (inputChoice > 0 && inputChoice <= captureCount) {
            g_config.inputName = pCaptureInfos[inputChoice - 1].name;
        }

        cout << "\n[ Destination Devices (Output) ]" << endl;
        cout << "0. Default System Output" << endl;
        for (ma_uint32 i = 0; i < playbackCount; i++) {
            cout << i + 1 << ". " << pPlaybackInfos[i].name << endl;
        }

        int outputChoice = 0;
        cout << "\nSelect Destination Device (0 - " << playbackCount << "): ";
        cin >> outputChoice;
        if (outputChoice > 0 && outputChoice <= playbackCount) {
            g_config.outputName = pPlaybackInfos[outputChoice - 1].name;
        }

        SaveConfig();
    }

    ma_device_id* pInputId = NULL;
    if (!g_config.inputName.empty()) {
        for (ma_uint32 i = 0; i < captureCount; i++) {
            if (g_config.inputName == pCaptureInfos[i].name) {
                pInputId = &pCaptureInfos[i].id;
                break;
            }
        }
    }

    ma_device_id* pOutputId = NULL;
    if (!g_config.outputName.empty()) {
        for (ma_uint32 i = 0; i < playbackCount; i++) {
            if (g_config.outputName == pPlaybackInfos[i].name) {
                pOutputId = &pPlaybackInfos[i].id;
                break;
            }
        }
    }

    ReverbEffectCreate(&g_reverbEffect, g_sampleRate);
    ApplyReverb();

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_duplex);
    deviceConfig.capture.pDeviceID = pInputId;
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 2;
    deviceConfig.playback.pDeviceID = pOutputId;
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = g_sampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.periodSizeInFrames = 256;
    deviceConfig.periods = 2;

    ma_device device;
    if (ma_device_init(&context, &deviceConfig, &device) != MA_SUCCESS) {
        cout << "\n[ERROR] Failed to init Audio Engine. Check routing." << endl;
        ma_context_uninit(&context);
        return -1;
    }

    ma_device_start(&device);

    PrintUI();

    bool k_T = false, k_E = false, k_R = false, k_0 = false, k_ESC = false;
    bool k_UP = false, k_DOWN = false, k_LEFT = false, k_RIGHT = false;

    while (true)
    {
        if (CheckKey(VK_ESCAPE, k_ESC)) {
            if (g_state == STATE_EDIT) {
                g_state = STATE_DASHBOARD;
                PrintUI();
            }
            else {
                break; 
            }
        }

        if (g_state == STATE_DASHBOARD) {
            if (CheckKey('T', k_T)) {
                g_config.isEnabled = !g_config.isEnabled;
                SaveConfig();
                PrintUI();
            }
            if (CheckKey('E', k_E)) {
                g_state = STATE_EDIT;
                g_editCursor = 0;
                PrintUI();
            }
            if (CheckKey(VK_RIGHT, k_RIGHT)) {
                g_config.presetId = (g_config.presetId + 1) % 114; 
                g_config.isCustomized = false;
                ApplyReverb();
                SaveConfig();
                PrintUI();
            }
            if (CheckKey(VK_LEFT, k_LEFT)) {
                g_config.presetId = (g_config.presetId - 1 + 114) % 114; 
                g_config.isCustomized = false;
                ApplyReverb();
                SaveConfig();
                PrintUI();
            }
        }
        else if (g_state == STATE_EDIT) {

            if (CheckKey('0', k_0)) {
                g_state = STATE_DASHBOARD;
                PrintUI();
            }
            if (CheckKey('R', k_R)) {
                g_config.isCustomized = false;
                ApplyReverb();
                SaveConfig();
                PrintUI();
            }
            if (CheckKey(VK_DOWN, k_DOWN)) {
                g_editCursor = (g_editCursor + 1) % 11; 
                PrintUI();
            }
            if (CheckKey(VK_UP, k_UP)) {
                g_editCursor = (g_editCursor - 1 + 11) % 11; 
                PrintUI();
            }

            bool changed = false;
            if (CheckKey(VK_RIGHT, k_RIGHT)) {
                if (g_editCursor == 0) g_config.mixLevel = Clamp(g_config.mixLevel + 0.05f, 0.0f, 1.0f);
                else {
                    StartCustomizing();
                    if (g_editCursor == 1) g_config.t_decayTime = Clamp(g_config.t_decayTime + 0.1f, 0.1f, 20.0f);
                    if (g_editCursor == 2) g_config.t_density = Clamp(g_config.t_density + 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 3) g_config.t_reflectionsDelay = Clamp(g_config.t_reflectionsDelay + 0.005f, 0.0f, 0.3f);
                    if (g_editCursor == 4) g_config.t_gainHF = Clamp(g_config.t_gainHF - 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 5) g_config.t_diffusion = Clamp(g_config.t_diffusion + 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 6) g_config.t_gain = Clamp(g_config.t_gain + 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 7) g_config.t_decayHFRatio = Clamp(g_config.t_decayHFRatio + 0.05f, 0.1f, 2.0f);
                    if (g_editCursor == 8) g_config.t_reflectionsGain = Clamp(g_config.t_reflectionsGain + 0.05f, 0.0f, 3.16f);
                    if (g_editCursor == 9) g_config.t_lateReverbGain = Clamp(g_config.t_lateReverbGain + 0.1f, 0.0f, 10.0f);
                    if (g_editCursor == 10) g_config.t_lateReverbDelay = Clamp(g_config.t_lateReverbDelay + 0.005f, 0.0f, 0.1f);
                }
                changed = true;
            }
            if (CheckKey(VK_LEFT, k_LEFT)) {
                if (g_editCursor == 0) g_config.mixLevel = Clamp(g_config.mixLevel - 0.05f, 0.0f, 1.0f);
                else {
                    StartCustomizing();
                    if (g_editCursor == 1) g_config.t_decayTime = Clamp(g_config.t_decayTime - 0.1f, 0.1f, 20.0f);
                    if (g_editCursor == 2) g_config.t_density = Clamp(g_config.t_density - 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 3) g_config.t_reflectionsDelay = Clamp(g_config.t_reflectionsDelay - 0.005f, 0.0f, 0.3f);
                    if (g_editCursor == 4) g_config.t_gainHF = Clamp(g_config.t_gainHF + 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 5) g_config.t_diffusion = Clamp(g_config.t_diffusion - 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 6) g_config.t_gain = Clamp(g_config.t_gain - 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 7) g_config.t_decayHFRatio = Clamp(g_config.t_decayHFRatio - 0.05f, 0.1f, 2.0f);
                    if (g_editCursor == 8) g_config.t_reflectionsGain = Clamp(g_config.t_reflectionsGain - 0.05f, 0.0f, 3.16f);
                    if (g_editCursor == 9) g_config.t_lateReverbGain = Clamp(g_config.t_lateReverbGain - 0.1f, 0.0f, 10.0f);
                    if (g_editCursor == 10) g_config.t_lateReverbDelay = Clamp(g_config.t_lateReverbDelay - 0.005f, 0.0f, 0.1f);
                }
                changed = true;
            }
            if (CheckKey(VK_LEFT, k_LEFT)) {
                if (g_editCursor == 0) g_config.mixLevel = Clamp(g_config.mixLevel - 0.05f, 0.0f, 1.0f);
                else {
                    StartCustomizing();
                    if (g_editCursor == 1) g_config.t_decayTime = Clamp(g_config.t_decayTime - 0.1f, 0.1f, 20.0f);
                    if (g_editCursor == 2) g_config.t_density = Clamp(g_config.t_density - 0.05f, 0.0f, 1.0f);
                    if (g_editCursor == 3) g_config.t_reflectionsDelay = Clamp(g_config.t_reflectionsDelay - 0.01f, 0.0f, 0.3f);
                    if (g_editCursor == 4) g_config.t_gainHF = Clamp(g_config.t_gainHF + 0.05f, 0.0f, 1.0f);
                }
                changed = true;
            }

            if (changed) {
                ApplyReverb();
                SaveConfig();
                PrintUI();
            }
        }

        Sleep(10);
    }

    ma_device_uninit(&device);
    ma_context_uninit(&context);
    ReverbEffectDestroy(&g_reverbEffect);

    return 0;
}