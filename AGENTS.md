# AGENTS.md

## Project Overview

ESP8266 button interrupt handler — a minimal embedded C application for the ESP8266 (Non-OS SDK) that demonstrates GPIO pin-change interrupt handling with software debounce.

- **Language:** C
- **Target:** ESP8266 (Xtensa LX106)
- **SDK:** ESP8266 Non-OS SDK (v3.0.0+)
- **Toolchain:** `xtensa-lx106-elf-gcc`
- **Flash tool:** `esptool`
- **License:** BSD 2-Clause (Copyright 2021, Eddy Young)

## Repository Structure

```
main.c          — Application entry point: GPIO init, timer, ISR, debounce logic
main.h          — Header guard (minimal)
Makefile        — Build system: compile, link, flash, clean
user_config.h   — Empty (required by SDK)
tags            — ctags index
.gitignore      — Ignores *.swp, *.bin, *.hex, *.o
LICENSE         — BSD 2-Clause
```

## Build Commands

| Command            | Description                                      |
|--------------------|--------------------------------------------------|
| `make`             | Clean and compile `main.c` into ELF binary       |
| `make flash`       | Build firmware images and flash to ESP8266 via esptool |
| `make clean`       | Remove build artifacts                           |

### Build Configuration

- **SDK path:** Set `SDK_HOME` in Makefile to your local ESP8266 Non-OS SDK directory
- **Compiler:** `/usr/bin/xtensa-lx106-elf-gcc`
- **Flash baud rate:** 921600
- **Firmware images:** `main-0x00000.bin` and `main-0x10000.bin`

## Key Implementation Details

- **GPIO button (pin 2):** Configured for falling-edge interrupt (`GPIO_PIN_INTR_NEGEDGE`)
- **GPIO LED (pin 0):** Also configured for falling-edge interrupt
- **Debounce:** Software debounce using a 1 ms timer counter (`elapsed`); requires 30 ms gap between successive triggers
- **Timer:** `os_timer_arm` re-arms the timer every 1 ms in the callback
- **UART:** Initialized at 115200 baud for debug output
- **Partition table:** Registered in `user_pre_init()` (required by SDK v3.0.0+)

## Code Style

- C89/C99 style for embedded
- K&R brace placement
- `ICACHE_FLASH_ATTR` used for functions stored in flash
- `os_printf` for debug output (not `printf`)
- Include guards use `#ifndef _MAIN_H_` pattern
- SDK types used throughout (`uint32`, `os_timer_t`, etc.)

## Git Commits

Commit messages use imperative mood with a short subject line, no body or footer. Format:

```
<Capitalized imperative verb> <object>
```

Examples from history:

- `Add .gitignore`
- `Initial commit`

**Guidelines:**

- Keep subject lines under 50 characters
- Use imperative mood ("Add", "Fix", "Update", "Remove", "Refactor")
- Capitalize the first letter
- No trailing period
- No scope prefixes or conventional-commit tags
