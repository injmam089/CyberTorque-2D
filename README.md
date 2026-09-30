# 🏎️ CyberTorque 2D

**CyberTorque 2D** is a high-performance **2D top-down arcade racing game** built from scratch in **C++** using native Windows APIs.

The project focuses on custom game-engine fundamentals rather than relying on third-party game engines. It includes vehicle physics, drifting, AI traffic, police pursuit, particle effects, procedural audio, garage upgrades, persistent save data, and a custom double-buffered rendering pipeline.

![C++](https://img.shields.io/badge/C%2B%2B-14-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![Graphics](https://img.shields.io/badge/Graphics-Win32%20GDI%2FGDI%2B-orange)
![Audio](https://img.shields.io/badge/Audio-Procedural%20PCM-purple)
![License](https://img.shields.io/badge/License-MIT-green)

---

## 🎮 Features

### 🏎️ Vehicle Physics

* Acceleration, braking, reverse, and inertia-based movement
* Steering and responsive arcade handling
* Controlled drifting and power sliding
* Nitro boost system
* Tire skid marks and smoke effects
* Crash impact and particle effects
* Multiple vehicle classes with different visual designs

### 🚗 AI Traffic System

* Multi-lane highway traffic
* Multiple traffic vehicle types
* Smooth lane switching
* Collision detection
* Police pursuit vehicles
* Dynamic near-miss scoring

### 🌲 Procedural Environment

* Procedurally rendered roadside scenery
* Trees, bushes, flowers, rocks, and other vegetation
* Highway shoulders and lane markings
* Curbs and road infrastructure
* Checkpoint and finish-line structures

### 🎵 Procedural Audio

CyberTorque 2D generates audio at runtime instead of depending on external sound files.

The custom audio system includes:

* Dynamic engine RPM sounds
* Tire screeching
* Nitro boost effects
* Crash sounds
* Checkpoint and pickup sounds
* Procedurally generated synthwave-style background music

Audio is generated using the Windows **WinMM `waveOut` API**.

### 🛠️ Garage & Upgrades

Players can improve their vehicle through the garage system.

Available upgrades include:

* Top Speed
* Acceleration
* Handling / Grip
* Nitro Capacity

The game also supports vehicle paint customization and persistent player progress.

### 💾 Save System

Game progress is stored locally, including:

* Unlocked vehicles
* Credits
* Upgrade progress
* High scores
* Player progression

---

## 🎮 Controls

| Action                | Key                |
| --------------------- | ------------------ |
| Accelerate            | `W` / `↑`          |
| Steer Left            | `A` / `←`          |
| Steer Right           | `D` / `→`          |
| Brake / Reverse       | `S` / `↓`          |
| Drift                 | `Space`            |
| Nitro Boost           | `Left Shift` / `N` |
| Mute / Unmute         | `M`                |
| Pause                 | `Esc` / `P`        |
| Menu Select / Buy     | `Enter` / `Space`  |
| Switch Garage Vehicle | `A` / `D`          |

---

## 🧠 Technical Architecture

CyberTorque 2D is organized into separate systems for gameplay, rendering, audio, vehicles, traffic, and effects.

```text
                    CyberTorque 2D
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
   Game Systems       Rendering           Audio
        │                 │                 │
   ┌────┼────┐       Win32 GDI/GDI+    WinMM waveOut
   │    │    │
  Car Traffic Road
   │    │    │
Physics AI  Environment
        │
        ▼
   Particle System
```

### Technology Stack

| Technology       | Purpose                             |
| ---------------- | ----------------------------------- |
| **C++14**        | Core programming language           |
| **Win32 API**    | Windows application and game window |
| **GDI / GDI+**   | 2D rendering                        |
| **WinMM**        | Real-time audio output              |
| **MinGW / GCC**  | Build environment                   |
| **Make / Batch** | Build automation                    |

The project does not depend on Unity, Unreal Engine, SDL, SFML, or other external game engines.

---

## 📂 Project Structure

```text
CyberTorque-2D/
│
├── AudioSynth.cpp
├── AudioSynth.h
│
├── Car.cpp
├── Car.h
│
├── Game.cpp
├── Game.h
│
├── ParticleSystem.cpp
├── ParticleSystem.h
│
├── Renderer.cpp
├── Renderer.h
│
├── Road.cpp
├── Road.h
│
├── Traffic.cpp
├── Traffic.h
│
├── Types.h
├── main.cpp
│
├── Makefile
├── build.bat
└── README.md
```

### Main Components

**`Car`**
Handles vehicle movement, physics, steering, drifting, upgrades, and vehicle behavior.

**`Traffic`**
Controls AI traffic vehicles, lane movement, police pursuit, and collision interactions.

**`Road`**
Manages highway layout, progression, road elements, and procedural scenery.

**`Renderer`**
Responsible for the game's double-buffered 2D graphics and HUD rendering.

**`ParticleSystem`**
Handles tire smoke, skid marks, sparks, flames, and other visual effects.

**`AudioSynth`**
Generates procedural game audio and background music using PCM synthesis.

**`Game`**
Controls the main game loop, game states, input, scoring, progression, and save system.

**`Types`**
Contains shared data structures, vector mathematics, colors, enums, and game-state definitions.

---

## 🚀 Build & Run

### Requirements

* Windows 7 or later
* MinGW / GCC or Microsoft Visual C++
* Windows SDK

### Option 1 — Build Script

Run:

```text
build.bat
```

The project will compile automatically.

### Option 2 — MinGW / GCC

Open Command Prompt or PowerShell inside the project directory:

```bash
g++ -O3 -std=c++14 AudioSynth.cpp ParticleSystem.cpp Road.cpp Car.cpp Traffic.cpp Renderer.cpp Game.cpp main.cpp -o CyberTorque.exe -lgdi32 -lmsimg32 -lwinmm -lgdiplus -mwindows
```

Then launch:

```bash
.\CyberTorque.exe
```

---

## 🎯 Project Goals

CyberTorque 2D was developed to explore practical concepts involved in building a game without relying on a commercial game engine.

The project demonstrates:

* Object-oriented programming in C++
* Real-time game loops
* 2D physics and movement
* Collision detection
* AI behavior
* Procedural rendering
* Particle systems
* Real-time audio synthesis
* Windows API programming
* Game-state management
* File-based persistence
* Performance-oriented rendering

---

## 📸 Screenshots

Add screenshots or gameplay GIFs here:

```text
docs/
├── gameplay.png
├── garage.png
├── traffic.png
└── gameplay.gif
```

Example:

```markdown
![Gameplay](docs/gameplay.png)
```

---

## 🔮 Future Improvements

Possible future enhancements include:

* More tracks and environments
* Additional vehicles
* Advanced opponent AI
* Multiplayer support
* Leaderboards
* More environmental effects
* Improved audio system
* Controller support
* Additional game modes

---

## 📄 License

This project is licensed under the **MIT License**.

---

## 👨‍💻 Author

**Injmam**

BCA Student | Cybersecurity & Software Development

GitHub: [@injmam089](https://github.com/injmam089)

---

⭐ If you find this project interesting, consider giving the repository a star.
