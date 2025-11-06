# MicroSTM-OS

MicroSTM-OS is a lightweight, operating system-like program designed for STM32 microcontrollers, intended to provide a command-line interface experience for embedded systems. It supports serial communication and SPI monitoring, offering users a familiar OS-like environment to interact with the hardware at a low level.

## Project Overview

MicroSTM-OS aims to bring basic operating system functionalities to STM32 microcontrollers by implementing essential shell commands for file and directory management, command execution, and system monitoring over serial interfaces. The focus is on offering an embedded terminal experience that allows users to browse, manipulate, and monitor files and peripherals on STM32 devices.

## Key Features

- Basic shell commands inspired by Linux for file management: `ls`, `pwd`, `cd`, `mkdir`, `rmdir`, `rm`, `cp`, `mv`, `touch`, `find`.
- File content viewing commands: `cat`, `head`, `tail`.
- Command-line interface accessible via serial port.
- SPI monitoring capabilities integrated to track SPI device communication.
- Modular and lightweight design suitable for resource-constrained MCU environments.
- Foundation for developing more advanced OS-like features on STM32 MCUs.

## Intended Use

MicroSTM-OS is ideal for embedded developers who want a minimal but functional OS command shell on STM32 to ease interaction with filesystems, peripherals, and inter-device communication using serial and SPI interfaces. It can serve as a learning tool for low-level embedded OS concepts or as a base for custom embedded applications.

## Hardware and Software Support

- Targeted for STM32 microcontroller series.
- Requires a serial interface for shell access.
- SPI interface support for device monitoring.
- Simple file and directory management abstraction adapted to embedded storage solutions or virtual filesystems.

## Contribution and Development

MicroSTM-OS is an open-source project encouraging contributions to expand command sets, improve performance, and add new features. The project welcomes developers interested in embedded OS design, STM32 programming, and serial/SPI communication.

## License

This project is released under an open-source license to promote collaboration and sharing.
