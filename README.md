# ⚙️ PlantOS + The Garden Wars

![Made with C](https://img.shields.io/badge/made%20with-C-blue.svg)
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi-red)
![License](https://img.shields.io/badge/license-MIT-green)
![Build](https://img.shields.io/badge/build-bare%20metal%20OS-orange)
![Status](https://img.shields.io/badge/status-completed-success)

**PlantOS** is a lightweight bare-metal operating system built from scratch for the Raspberry Pi, featuring a custom CLI, multimedia capabilities, and full hardware interfacing. On top of this OS, we developed **The Garden Wars**, an arcade-style tower defense game inspired by *Plants vs. Zombies*, running natively without any external libraries or OS dependencies.

This project was completed as part of the EEET2490 Embedded Systems course and demonstrates end-to-end system design — from kernel development to user interaction through real-time gameplay.

> 🧩 100% bare-metal C code  
> 🎮 Designed for real hardware and QEMU  
> 🌿 From bootloader to bullets — all in our control

📄 Licensed under the [MIT License](LICENSE)

![The Garden Wars Thumbnail](images/the-garden-wars.png)

## 📦 Project Structure

```md
The Garden Wars/
├── assets/                    # All visual and assets used in the game
│   ├── backgrounds/           # Background images for gameplay scenes and menus
│   ├── button/                # UI elements such as play, retry, and home buttons
│   ├── fonts/                 # Bitmap, array-based fonts used for rendering text
│   ├── selection/             # Assets for plant selection cards or interface
│   └── sprites/               # Main game sprites: zombies, plants, bullets
├── build/                     # Compilation outputs: object files, .img file, etc.
├── include/                   # Header (.h) files for declarations
├── src/                       # All source (.c) code implementing game and OS logic
│   ├── arch/                  # Architecture-specific code (e.g., boot.S, link.ld)
│   ├── cmd/                   # Implementation of command-line interface (CLI) features
│   ├── drivers/               # Peripheral drivers (UART, framebuffer, GPIO, etc.)
│   ├── games/                 # Core game logic: plants, zombies, plant grid
│   ├── video/                 # Video rendering logic (e.g., image, video displaying)
│   ├── kernel.c               # Main entry point and game state machine
│   └── utils.c                # Helper functions (e.g., string converter, maths)
├── Makefile                   # Build automation script defining how to compile/link
└── README.md                  # Project description, setup guide, and usage instruction
└── LICENSE                    # MIT License
```

## 🚀 Features

### 🖥️ PlantOS – Bare Metal Operating System

- **Welcome Screen** with custom ASCII art

- **Command-Line Interface (CLI)** with:

  - Auto-completion using `TAB`

  - Command history using `↑` (up) and `↓` (down)

  - Deletion/editing support with a preserved OS prompt

- **Core Commands:**
  - `help`, `clear`, `showinfo`, `baudrate`, `handshake`

### **📽️ Multimedia Output**

- Text rendering with custom fonts

- Image display over framebuffer

- Video playback using extracted RGB frames

- Team member names rendered over backgrounds

### 🎮 The Garden Wars Game

- Game inspired by Plants vs. Zombies

- Fixed and scalable grid for planting defense units

- Zombies attack in waves, increasing difficulty over time

- Game states: MENU → DIFFICULTY SELECT → PLAYING → GAME OVER / VICTORY

- CLI-GUI communication with ACK/NAK feedback

## 🔧 How to Build and Run

### ✅ Prerequisites

- Raspberry Pi 3/4 (bare-metal setup)

- GCC ARM cross-compiler (aarch64-linux-gnu-gcc)

- QEMU (for emulation testing)

### 🛠️ Build & Run Options

You can build specific parts of the project depending on your goal:

```bash
make           # Build everything (default)
make cli       # Build only the Command Line Interface (CLI)
make video     # Build only the video display demo
make game      # Build only The Garden Wars game
```

After building, flash the `kernel.img` in the `build/directory` to your SD card or run with QEMU:

## 🎮 How to Play

1. Launch the OS.

2. From CLI, type start game or select through GUI.

3. Choose difficulty level.

4. Use the keyboard to select and plant defenses.

5. Survive all waves of zombies to win.

## 🧠 Lessons Learned

- This project taught us:

- How to interface hardware using low-level C

- How to build framebuffer-based GUIs

- How to create a real-time game engine on bare metal

- Collaboration in version control and source management

## 🛠️ Tools Used

We utilized a variety of tools to support development, media processing, and system building:

- [GNU Make](https://www.gnu.org/software/make/manual/make.html) – for build automation  
- [image2cpp](https://javl.github.io/image2cpp/) – to convert images into C-style RGB framebuffer  
- [Frame Extractor](https://frame-extractor.com/) – to extract individual frames from video clips  

> 🧠 With PlantOS and The Garden Wars, we transformed an academic assignment into a hands-on, end-to-end system software experience.

## 🧠 Game State Flow Chart

![Game State Chart](images/game-state-chart.png)



## 📷 Screenshots

<div align="center">

<table>
  <tr>
    <td><img src="images/plantos.png" width="600px" alt="PlantOS Welcome"/></td>
    <td><img src="images/main-screen.png" width="600px" alt="Main Menu"/></td>
  </tr>
  <tr>
    <td><img src="images/game-play.png" width="600px" alt="Gameplay"/></td>
    <td><img src="images/lose-screen.png" width="600px" alt="Game Over"/></td>
  </tr>
  <tr>
    <td><img src="images/victory-screen.png" width="600px" alt="Victory"/></td>
  </tr>
  <tr>
    <td><img src="images/plant-assets.png" width="600px" alt="Plant Assets"/></td>
    <td><img src="images/zombies-assets.png" width="600px" alt="Zombie Assets"/></td>
  </tr>
</table>
</div>

## 👥 Contributors

- Ngo Van Tai ([Github](https://github.com/TaiVanNgo))
- Huynh Thai Duong ([Github](https://github.com/TDuong04))
- Nguyen Pham Anh Thu ([Github](https://github.com/thu-ngx))
- Huynh Tan Phat ([Github](https://github.com/phatgg221))
