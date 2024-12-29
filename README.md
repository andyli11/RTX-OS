
# RTX Operating System

This project implements a **Real-Time Executive (RTX)** Operating System on the **Intel DE1-SoC** board using **C** and the **ARM DS IDE**. The system supports multitasking, memory management, inter-task communication, and real-time scheduling.

## Table of Contents
1. [Project Overview](#project-overview)
2. [Setup](#setup)
3. [Project Structure](#project-structure)
4. [Features](#features)
5. [Building and Running](#building-and-running)
6. [Testing](#testing)
7. [Contributors](#contributors)

---

## Project Overview

The RTX project is developed as part of the **ECE350 Lab** course at the University of Waterloo. It provides a basic multitasking environment that supports:
- Dynamic memory management.
- Task management with strict-priority scheduling.
- Inter-task communication using mailboxes.
- Interrupt-driven UART for terminal I/O.

**Target Hardware**: Intel DE1-SoC (Cyclone V SoC chip with dual-core ARM Cortex-A9 and Altera FPGA).

---

## Setup

### Prerequisites
- **ARM DS IDE** (Eclipse-based IDE for programming the DE1-SoC board).
- **Intel DE1-SoC Board**.
- **Git** for version control.
- UART Terminal Emulator (e.g., PuTTY).

### Setting up the Development Environment
1. Clone the repository:
   ```bash
   git clone git@github.com:andyli11/RTX-OS.git
   ```
2. Open ARM DS IDE and import the project:
   - Go to `File > Open Projects from File System...`.
   - Select the `RTX` folder from the cloned repository.
3. Set up the DE1-SoC JTAG UART:
   - Configure the terminal as per the manual's instructions (see **Section 2.5**).

---

## Project Structure

The project consists of the following key directories:

| Directory            | Description                                                              |
|----------------------|--------------------------------------------------------------------------|
| `src/kernel`         | Kernel source code for memory management, scheduling, and task management. |
| `src/board/DE1_SoC_A9` | Board-specific files for the DE1-SoC platform.                         |
| `src/app`            | Application test cases and user tasks.                                   |
| `src/INC`            | Header files for the RTX API and common definitions.                     |

Key files:
- `k_mem.c`: Memory management implementation.
- `k_task.c`: Task management and scheduling logic.
- `kcd_task.c`: Implementation of the Keyboard Command Decoder (KCD).

---

## Features

1. **Memory Management**:
   - Implements **first-fit dynamic memory allocation** and deallocation.
   - Tracks ownership of allocated regions.

2. **Task Management**:
   - Supports task creation, termination, priority setting, and yielding.
   - Scheduler uses **strict-priority FIFO** for ready tasks.

3. **Inter-task Communication**:
   - Mailbox-based message passing.
   - Message types include `KCD_REG`, `KCD_CMD`, and `KEY_IN`.

4. **Keyboard Command Decoder (KCD)**:
   - Registers and processes terminal commands starting with `%`.
   - Handles input via UART and forwards valid commands to respective tasks.

5. **Interrupt Handling**:
   - UART-based interrupt handling for terminal I/O.

---

## Building and Running

### Building
1. Import the project into ARM DS IDE.
2. Build the project:
   - Right-click on the `RTX` project in the `Project Explorer`.
   - Select `Build Project`.

### Running
1. Program the DE1-SoC board:
   - Navigate to the DE1-SoC directory and run `program.bat` to reset the board.
2. Open a UART terminal to monitor system output.
3. Debug and run the project:
   - Use the `.launch` file in ARM DS IDE for debugging.

---

## Testing

### Test Cases
- **Memory Management**:
  - Located in `src/app/ae_mem.c`.
  - Includes allocation, deallocation, and fragmentation analysis.

- **Task Management**:
  - Located in `src/app/ae_usr_tasks.c` and `src/app/ae_priv_tasks.c`.
  - Tests preemption, priority changes, and task creation.

- **Inter-task Communication**:
  - Validates mailbox operations and KCD functionality.

### Running Tests
1. Modify `ae_set_task_info` in `src/app/ae.c` to configure initial tasks.
2. Build and load the project onto the board.
3. Verify outputs using the UART terminal.

