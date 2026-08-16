# 🏎️ Apex Cyber Drive 2D — High-Performance C++ Arcade Racing Game

[![Language](https://img.shields.io/badge/Language-C%2B%2B14-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B14)
[![Platform](https://img.shields.io/badge/Platform-Windows%20(Win32)-lightgrey.svg)](https://learn.microsoft.com/en-us/windows/win32/)
[![Dependencies](https://img.shields.io/badge/Dependencies-Zero%20External%20Libs-brightgreen.svg)]()
[![Graphics](https://img.shields.io/badge/Graphics-Double--Buffered%20GDI%20%2F%20GDI%2B-orange.svg)]()
[![Audio](https://img.shields.io/badge/Audio-Real--Time%20Procedural%20PCM%20Synthesizer-purple.svg)]()

> A feature-complete **2D Top-Down Arcade Highway Racing Game** engineered from scratch in pure **C++** using native **Win32 APIs**, **Double-Buffered Vector GDI Graphics**, and a custom **Real-Time Procedural PCM Audio Synthesizer** (WinMM) — requiring **zero third-party game engines or external DLLs**.

---

## 📸 Overview & Key Highlights

**Apex Cyber Drive 2D** combines responsive arcade vehicle dynamics, multi-lane highway traffic AI, procedural roadside nature rendering, and a retro synthwave procedural audio engine into a lightweight, standalone Windows executable running at a rock-solid 60 FPS.

```
       ____________________________________________________________________
      |  [SCORE: 014250]        PACIFIC COASTLINE PARKWAY       [TIME: 01:24.50]  |
      |--------------------------------------------------------------------|
      |   (Tree)   |   |   |   |   |   |   |   |   |   |   |   |   (Tree)  |
      |  (Foliage) | [Curb] |   |   | [Traffic: Sedan]  | [Curb] |(Blossom)|
      |   (Rock)   |   |    |   |   |       |       |   |   |    | (Pine)  |
      |            |   |    | [Player: Supercar]    |   |   |    |         |
      |   (Bush)   |   |    |   |   |       |       |   |   |    | (Bush)  |
      |  (Forest)  | [Curb] |   |   | [Police Cruiser]  | [Curb] |(Forest) |
      |--------------------------------------------------------------------|
      | [WASD] Drive   [Space] Drift   [Shift] Nitro Boost   [Speed: 165 MPH]  |
      |____________________________________________________________________|
```

---

## 🌟 Core Features

### 1. 🏎️ Advanced 2D Vehicle Physics & Handling
- **Rigid-Body 2D Dynamics**: Forward throttle, progressive braking, reverse, and inertia curves.
- **Power Sliding & Controlled Drifting**: Pressing `Spacebar` while turning initiates controlled lateral slides with counter-steering control and continuous Nitro recharge.
- **Continuous Skid Marks & Particle FX**: Trailing tire skid marks buffer, tire smoke puffs during slides, exhaust nitro flames, and crash impact sparks.
- **4 Distinct Vehicle Classes**:
  - **Apex Falcon**: Aerodynamic Supercar with coke-bottle curves, carbon splitter, glass cockpit dome with glare reflection, and dual-tier GT wing.
  - **Viper GT**: American Muscle Car with widebody flared fenders, power hood scoop, dual racing stripes, and drag radials.
  - **Cyber Phantom**: Le Mans Hypercar with front aero tunnels, teardrop cockpit, Le Mans center shark fin, and continuous LED lightblade.
  - **Titan Enforcer**: Heavy armored GT with steel bull-bar push bumper, armored wheel arches, and roof rails.

### 2. 🌲 Procedural Roadside Nature & Scenery
- **Multi-Layered Tree Rendering**: Organic oak trees with dark shadow underlayers, vibrant green leaf canopies, sunlight highlights, and soft ground shadows.
- **Diverse Flora**: Evergreen pines, cherry blossom trees, flowering bushes with blossom dots, and granite boulders.
- **Detailed Highway Infrastructure**: Textured gravel shoulders, red/white curbs, solid yellow center median, dashed white lane dividers, overhead checkpoint and finish line truss arches.

### 3. 🚙 Multi-Lane AI Traffic & Police Pursuits
- **Intelligent Traffic AI**: Sedans, Sports Coupes, 18-Wheeler Semi-Trucks (with chrome cabs and corrugated trailers), and Police Cruisers.
- **Smooth Spring-Damper Lane Switching**: AI cars glide smoothly across lanes along natural S-curves.
- **Police Pursuit AI**: Police cruisers chase the player with aggressive interception and dual-color flashing LED rooftop strobes.
- **"Close Call" Near-Miss Scoring**: Rewards +500 combo points and credits for threading tight gaps at high speed.

### 4. 🎵 Real-Time Procedural PCM Audio Synthesizer
- Built using native Windows Multimedia (`waveOut` / WinMM) running on a dedicated worker thread.
- **Zero Audio Files Required**: Procedurally generates soundwaves in real-time:
  - Dynamic RPM engine revving sound modulated by player throttle.
  - White-noise filtered tire screeching during power slides.
  - Resonant whoosh sound on Nitro Overdrive.
  - Crash explosion booms, checkpoint chimes, and coin pickups.
  - Multi-channel retro Synthwave background soundtrack with bassline and melody.

### 5. 🛠️ Garage Tuning & Persistence
- **Performance Upgrades**: Upgrade **Top Speed**, **Acceleration**, **Handling / Grip**, and **Nitro Capacity** (5 levels per stat).
- **Custom Paint Schemes**: 5 selectable high-gloss paint finishes per vehicle.
- **Persistent Save System**: Automatically saves progress, unlocked cars, credits, and high scores to binary save storage (`savegame.dat`).

---

## 🕹️ Controls

| Action | Primary Key | Secondary Key |
| :--- | :--- | :--- |
| **Accelerate / Drive** | `W` | `Up Arrow` |
| **Steer Left / Right** | `A` / `D` | `Left` / `Right Arrow` |
| **Brake / Reverse** | `S` | `Down Arrow` |
| **2D Drift / Handbrake** | `Spacebar` | - |
| **Nitro Boost** | `Left Shift` | `N` |
| **Mute / Unmute Audio** | `M` | - |
| **Pause Game** | `Esc` | `P` |
| **Menu Select / Buy Upgrade** | `Enter` | `Spacebar` |
| **Garage Car Switch** | `A` / `D` | `Left` / `Right Arrow` |

---

## 🏗️ Architecture & Technology Stack

```
   ┌────────────────────────────────────────────────────────┐
   │                     Apex Cyber Drive 2D                │
   ├───────────────────┬──────────────────┬─────────────────┤
   │   Game Logic &    │   Double-Buffer  │   Procedural    │
   │     Physics       │  Vector Renderer │ PCM Synthesizer │
   │  (Car, Traffic)   │  (GDI / GDI+)    │ (WinMM waveOut) │
   └─────────┬─────────┴────────┬─────────┴────────┬────────┘
             │                  │                  │
             └──────────────────┼──────────────────┘
                                │
                    ┌───────────▼───────────┐
                    │ Native Windows 32 API │
                    └───────────────────────┘
```

- **Language**: C++ (C++14 standard)
- **Graphics Pipeline**: Win32 GDI & GDI+ Double-Buffered Rendering (zero screen tearing, 60 FPS)
- **Audio Pipeline**: Custom PCM 16-bit 44.1kHz real-time software audio synthesizer
- **Target OS**: Windows 7 / 8 / 10 / 11 (32-bit & 64-bit compatible)
- **Dependencies**: Native Windows SDK (`lgdi32`, `lmsimg32`, `lwinmm`, `lgdiplus`)

---

## 📂 Project Structure

```
ApexCyberDrive/
├── AudioSynth.h / .cpp    # Procedural PCM audio synthesizer & synthwave sequencer
├── Car.h / .cpp           # 2D Vehicle rigid body physics, drifting & upgrades
├── Traffic.h / .cpp       # Multi-lane AI traffic, police pursuit AI & collision detection
├── Road.h / .cpp          # Straight 4-lane speedway, track progression & nature generation
├── ParticleSystem.h / .cpp# 2D Tire skid mark buffer, tire smoke, sparks, and flames
├── Renderer.h / .cpp      # 2D Double-buffered vector renderer, car sprites & UI HUD
├── Game.h / .cpp          # Core game loop, state machine, save profiles & input
├── Types.h                # 2D Vector math, Color structs, enums & state definitions
├── main.cpp               # WinMain entry point, window management & 60 FPS timing
├── Makefile               # MinGW build automation file
├── build.bat              # One-click Windows compilation batch script
└── README.md              # Comprehensive documentation
```

---

## 🚀 How to Build and Run

### Prerequisites
- Windows OS (Windows 7 or later)
- MinGW / GCC (`g++`) or Microsoft Visual C++ (MSVC)

### Option 1: Quick One-Click Build
Double-click `build.bat` in the project root directory.

### Option 2: Command Line (MinGW / GCC)
Open PowerShell or Command Prompt in the project folder and run:
```powershell
g++ -O3 -std=c++14 AudioSynth.cpp ParticleSystem.cpp Road.cpp Car.cpp Traffic.cpp Renderer.cpp Game.cpp main.cpp -o CyberTorque.exe -lgdi32 -lmsimg32 -lwinmm -lgdiplus -mwindows
```

### Option 3: Launch Executable
```powershell
.\CyberTorque.exe
```

---

## 📜 License
This project is open-source and available under the **MIT License**.
