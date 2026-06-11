# Custom Priority-Based Preemptive RTOS for AVR (ATmega328P)

This project is a custom-built, lightweight, priority-based preemptive Real-Time Operating System (RTOS) designed entirely from scratch for the ATmega328P microcontroller in C and Inline Assembly.

## Features
* **Preemptive Multitasking:** Uses Timer0 to interrupt the CPU every 1ms and switch contexts dynamically.
* **Priority-Based Scheduler:** Departs from standard Round-Robin by implementing a strict hierarchy. Higher-priority tasks will instantly preempt lower-priority ones.
* **Blocking & Yielding Mechanisms:** Replaces inefficient delay loops with an `os_delay()` function that puts tasks to sleep (`TASK_BLOCKED`), eliminating CPU starvation and time dilation.
* **Custom Context Switcher:** Implements bare-metal `SAVE_CONTEXT` and `RESTORE_CONTEXT` inline assembly macros to handle the ATmega328P's 32 general-purpose registers and SREG.

## Project Structure
* `driver.h` / `driver.c`: Hardware abstraction layer (HAL) for GPIO manipulation (LEDs, Buttons).
* `task.h` / `task.c`: RTOS core, Memory/Stack allocation, TCB (Task Control Block) definition, and task creation routines.
* `timer0.h` / `timer0.c`: System clock configuration, ISR context switching macros, and the core scheduling algorithm.
* `main.c`: Application layer implementing three independent tasks (Green, Yellow, Red) with different priorities.
* `Makefile`: Automates the compilation and flashing process via `avr-gcc` and `avrdude`.

## Hardware Setup
* **Target:** ATmega328P (Arduino Uno)
* **Outputs:** 3 LEDs connected to PORTB (PB0, PB1, PB2)
* **Inputs:** 3 Push Buttons connected to PORTD (PD2, PD3, PD4) configured with internal pull-ups.

## How to Build & Flash
Ensure you have `avr-gcc`, `avr-libc`, and `avrdude` installed. Run the following commands in your terminal:

```bash
# 1. Compile the code
make

# 2. Upload to the microcontroller (ensure your device port in Makefile is correct, e.g., /dev/ttyUSB0)
make upload

# 3. Clean build files
make clean