# STM32

## Overview
As a software developer, I usually work on applications with a high level of abstraction. However, during my apprenticeship, I had the privilege of developing embedded applications. Since I had no prior experience in this domain and no background in electronics, I had to learn everything from the ground up, and start from the basics. With the help of my colleagues, I managed to implement several functionalities from scratch, including:

### Implemented Features
- **Software Timer**
- **USART Communication**
  - Initially implemented iteratively, then optimized using interruptions
- **Circular FIFO**
  - Used for buffering data efficiently
- **Formatted Output Functions**
  - Implemented `tiny_printf` and `tiny_sprintf`
- **Stream Descriptor**
  - Created a USART descriptor
  - Designed a set of `sd_Read`, `sd_Write`, and `sd_Printf` functions
  - Enabled output redirection to USART or any other descriptor when implemented
- **Basic Terminal Interface**
  - Manage Echap Sequences
  - Manage Interaction such typing a text, editing it, moving the cursor using VT100 commands.
  - Create command, and execute it depending on user input.

## What I Learned
Through this experience, I gained a solid understanding of low-level programming and significantly improved my C programming skills. More specifically, I learned about:
- **Embedded systems programming** and real-time constraints
- **Low-level communication protocols**, such as USART
- **Efficient data buffering techniques** (e.g., circular FIFOs)
- **Memory management** and working without dynamic allocation
- **Hardware-software interaction** at a fundamental level

This project was a challenging yet rewarding opportunity to step into the world of embedded development, reinforcing my ability to work closer to the hardware.

