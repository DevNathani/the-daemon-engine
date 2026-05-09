# The Daemon — Terminal-Native Linux Learning Engine

> A terminal-based Linux exploration engine designed to teach Linux concepts through interaction, experimentation, and problem-solving.

---

# Repository Notice

This repository contains a **demonstration version** of the engine used to power **The Daemon** event.

To preserve challenge integrity and fairness for future events:

- Real event levels are **not included**
- Event-specific content remains **private**
- Demo levels and sample assets are provided for **educational and demonstration purposes only**

This repository focuses on:
- game mechanics
- system architecture
- security model
- monitoring systems
- Dockerized deployment
- SSH-based gameplay infrastructure

---

# Overview

The Daemon is a terminal-native Linux learning platform where players connect to a Linux environment through SSH and progress through multiple levels by solving filesystem-based challenges.

Unlike traditional tutorials, the project emphasizes:
- exploration
- experimentation
- debugging
- curiosity-driven learning

The system teaches Linux concepts organically through interaction with the environment itself.

---

# Design Philosophy

The Daemon treats Linux not as a subject to memorize,
but as a system to explore.

Players are expected to:
- investigate
- experiment
- fail
- retry
- learn organically

The project focuses on exploration-driven learning rather than instruction-driven teaching.

---

# Objectives

- Teach Linux using hands-on interaction
- Encourage filesystem exploration
- Improve terminal confidence
- Build debugging and analytical thinking
- Introduce Linux internals naturally

---

# Core Gameplay Loop

```text
SSH Login
    ↓
Explore Environment
    ↓
Observe Clues
    ↓
Find Hidden Passcode
    ↓
Submit Key
    ↓
Advance to Next Level
```

---

# Features

- ## Filesystem-Based Gameplay
    - Levels exist entirely inside the Linux filesystem
    - Players interact with:
        - files
        - directories
        - permissions
        - environment variables
        - hidden data



- ## SSH-Based Environment
    - Players connect using SSH
    - Fully terminal-native experience
    - Custom shell environment and prompts

- ## Real-Time Progression System
    - Dynamic level loading
    - Persistent progression tracking
    - SQLite-backed game state management



- ## Monitoring & Leaderboards
    - Real-time leaderboard system
    - Completion timestamps
    - Time-per-level tracking
    - Giveup tracking

- ## Security Model
    - SUID-based controlled execution
    - User isolation
    - Restricted filesystem access
    - Dedicated game-engine execution context


- ## Dockerized Deployment
    - Containerized game environment
    - Reproducible deployment
    - Isolated runtime configuration

---

#  Linux Concepts Covered

Examples include:
- hidden files
- file permissions
- environment variables
- Data Redirection
- encoding and decoding
- process behavior
- filesystem traversal
- shell scripting
- command-line utilities


---

#  Documentation
 Detailed documentation is available in the [`docs/`](./docs) directory.

| Document | Description |
|---|---|
| `PRD.md` | Product Requirements Document |
| `Architecture.md` | System architecture and execution flow |
| `File_Structure.md` | Repository and filesystem structure |
| `Player_Scripts.md` | Player command documentation |
| `Admin_Scripts.md` | Administrative scripts and controls |
| `Security_Model.md` | Security and permission model |
| `SSH_Configuration.md` | SSH server and shell configuration |
| `Monitoring_System.md` | Leaderboard and monitoring architecture |
| `Database_Schema.md` | Database structure and relationships |

---

# Screenshots

Screenshots and demo visuals are available in:
```text
docs/screenshots
```

---

#  Technologies Used

- ## Languages
    - C
    - Bash


- ## Infrastructure
    - Docker
    - OpenSSH
    - SQLite

- ## Linux Components
    - SUID execution
    - filesystem permissions
    - shell environments
    - process utilities

---

# Security Design

The engine uses:
- dedicated game-engine UID
- controlled privilege elevation
- isolated player environments
- restricted filesystem permissions

Sensitive operations such as:
- level validation
- progression updates
- database access

are executed through controlled binaries.

---

# 🚀 Future Enhancements

Planned improvements include:
- dynamic level plugins
- web dashboard
- achievement system
- multiplayer/co-op events
- modular challenge packs
- advanced analytics

---

# License

This project is released for educational and demonstration purposes.

Event-specific levels, challenge content, and production assets remain private.

---

# Acknowledgements

Special thanks to the contributors who helped shape the gameplay experience through level design, story concepts, testing, and event coordination.

- ## 👥 Contributors

    - [Shubham Gupta](https://github.com/kanha321) — Level design and gameplay ideas
    - [Sanyam Goel](https://github.com/hexwhiz) — Story concepts and level design
    - [Rituraj Singh](https://github.com/gintoki027) — Gameplay balancing and challenge testing
    - [Dev Nathani](https://github.com/DevNathani/DevNathani) - Core engine development, infrastructure, and system design


- ## 🏫 Club Support

    Special thanks to the **APPLICATION CLUB MNNIT** for:
    - organizing the event
    - coordinating participants
    - testing the game environment
    - helping improve the overall player experience