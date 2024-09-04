### Author: Niket Mittal
### Roll Number: 2024201076
### Title: POSIX Shell Implementation

# Overview
This project implements a custom shell that supports various functionalities such as command execution, input/output redirection, pipelines, signal handling, and autocompletion using RAW mode. The shell aims to mimic basic features found in typical Unix-like shells while providing a personalized prompt and responsive command-line experience.

# Features
1. Dynamic Prompt: Displays a custom prompt showing the username, hostname, and current directory.
2. Command Execution: Execute all type of system commands.
3. Input/Output Redirection: Redirect input and output using <, >, and >>.
4. Pipelines: Chain commands using the pipe (|) operator.
5. Signal Handling:
    CTRL-Z: Stops the current foreground job and pushes it to the background.
    CTRL-C: Interrupts the current foreground job.
    CTRL-D: Exits the shell.
6. Autocompletion: Autocomplete file and directory names when the TAB key is pressed.

# Prerequisites
1. C++ Compiler: Ensure you have g++ or any other C++ compiler installed on your system.

### Compilation
You can compile the project using the provided Makefile. To do so:
- Open a terminal and navigate to the directory containing the source files and the Makefile.
- Run the following command: make (This will compile the source files and generate the 'myShell' executable.)
- After compilation, you can start the shell by executing: ./myShell
