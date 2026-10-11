# 🎛️ EAX Reverb Audio Router (VoiceMeeter Edition)

A lightweight, real-time C++ audio routing utility that intercepts audio from an input device (e.g., a microphone or VoiceMeeter Virtual Output), applies advanced **EAX (Environmental Audio Extensions)** Reverb DSP effects, and routes it to a designated output device. 

This tool is designed for users who want hardware-style, zero-latency environmental microphone effects without relying on heavy DAWs or complex VST plugins.

## ✨ Key Features

*   **Real-Time Audio Routing:** Built on top of `miniaudio` for ultra-low latency audio capture and playback.
*   **114 Built-in EAX Presets:** Includes classic environments such as *Concert Hall, Cave, Hangar, Sewers, Psychotic, Ice Palace*, and many more.
*   **Live Parameter Tweaking:** Dynamically edit DSP parameters on the fly via the console UI without restarting the audio engine:
    *   Wet Mix Level
    *   Decay Time & Decay HF Ratio
    *   Room Size (Density) & Diffusion
    *   Pre-Delay (Reflections Delay) & Damping (HF)
    *   Master Gain, Reflections Gain, and Late Reverb Gain/Delay
*   **Persistent Configuration:** Automatically saves your I/O routing choices, active preset, and custom tweaks to `reverb_config.txt`.
*   **Global Hotkeys:** Fully controllable via keyboard (Toggle bypass, switch presets, enter edit mode).

## 🚀 How It Works

The application creates a full-duplex audio stream. It captures mono/stereo frames from your selected Input (e.g., VoiceMeeter Aux Output), applies a complex Reverb node graph (utilizing biquad, low-pass, high-pass, and all-pass filters to simulate physical environments), and pushes the processed frames to your Output (e.g., VoiceMeeter Aux Input or directly to speakers).

### 🎮 Console Hotkeys
*   `[ T ]` - Toggle Reverb ON/OFF (Bypass)
*   `[ Left / Right ]` - Cycle through the 114 EAX Presets
*   `[ E ]` - Enter **Edit Mode** to customize individual reverb parameters
*   `[ R ]` - Restore current preset to its default values
*   `[ ESC ]` - Exit Edit Mode / Quit Application

## 🛠️ Tech Stack & Integration

*   **Language:** `C++`
*   **Audio Backend:** [miniaudio](https://miniaud.io/) - A single-file C audio library for decoding, routing, and device I/O.
*   **DSP/Effects:** Standard EFX EAX Reverb properties translation.

## 🤝 Credits & Acknowledgements

*   **DSP Implementation:** The core Reverb mathematics, delay lines, and EAX preset definitions (`ReverbEffect.cpp` & `efx-presets.h`) are heavily based on the excellent work by [Relfos/EAXReverb](https://github.com/Relfos/EAXReverb). 
*   **Audio Engine:** Audio capturing, playback, and device management powered by the amazing [miniaudio](https://github.com/mackron/miniaudio) library.
*   **Integration & UI:** Custom routing, config management, real-time tweaking interface, and VoiceMeeter pipeline integration developed by [AliShe3a](https://github.com/AliShe3a).

## ⚠️ Disclaimer
This project was built primarily for educational purposes and as a favor for a friend to use alongside VoiceMeeter. Feel free to fork, modify, and improve upon the architecture!
