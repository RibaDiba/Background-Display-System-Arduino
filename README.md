# Background Display System (BDS)

> A modular Arduino-based background display system for synchronized modules / tiles.

![Platform](https://img.shields.io/badge/platform-Arduino-orange) ![Language](https://img.shields.io/badge/language-C%2B%2B-lightgrey) 

<p>
  <a href="https://github.com/RibaDiba" target="_blank" rel="noopener noreferrer">
    <img src="https://images.weserv.nl/?url=avatars.githubusercontent.com/u/96350741?v=4&mask=circle&maxage=7d" width="60" height="60" alt="@RibaDiba" />
  </a>
  <a href="https://github.com/iamgod5" target="_blank" rel="noopener noreferrer">
    <img src="https://images.weserv.nl/?url=avatars.githubusercontent.com/u/118085362?v=4&mask=circle&maxage=7d" width="60" height="60" alt="@iamgod5" />
  </a>
</p>

---

## Project Summary

**Background Display System (BDS)** is a small modular display/control system implemented with Arduino-compatible code. Each system is capable of housing and displaying 3 backgrounds, placed internationally at Odessey of the Mind.

---

## Features

- Master / Slave architecture for synchronized displays.
- Designed to work with Bluetooth MAC-addressable modules.
- Lightweight C++ code for microcontrollers (Arduino-compatible).
- Controller code desgined to work with multiple systems 

---

## Repository Structure

| Path / File | Description |
|-------------|-------------|
| `struct.h` | Shared data structures and message formats used across modules (Master/Slave protocol). |
| `Archived/` | Old code or experiments (archived for reference). |
| `BDS.h` | Header file containing class/function declarations |
| `Master.ino` | Implementation file for the master ESP32 (microcontroller on hand). Utilizes the BDS class to effectivly control mutliple systems with a single ESP32 |
| `Slave.ino` | Implementation for slave code that reads input from the master ESP32, delivering power to the motor. |

---

## Software, Build & Upload

Two common ways to build/upload Arduino code:

### Option A — Arduino IDE
1. Install Arduino IDE (or use Arduino CLI / Web Editor).
2. Open the desired sketch (e.g., `Master/<sketch>.ino` or `Slave/<sketch>.ino`).
3. Select your board and port: `Tools → Board`, `Tools → Port`.
4. Upload: click **Upload**.

### Option B — PlatformIO (VSCode)
1. Install VSCode + PlatformIO extension.
2. Open the repo folder in VSCode.
3. Configure `platformio.ini` (add the board & upload options).
4. Build: `PlatformIO: Build`.
5. Upload: `PlatformIO: Upload`.

### Option C — Arduino CLI (headless / scripting)
```bash
arduino-cli compile --fqbn <fqbn> path/to/sketch
arduino-cli upload -p /dev/ttyUSB0 --fqbn <fqbn> path/to/sketch
