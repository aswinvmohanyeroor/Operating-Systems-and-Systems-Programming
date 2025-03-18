
# Operating Systems and Systems Programming

## Overview

This repository contains assignments and projects related to **Operating Systems and Systems Programming**. The work includes implementation of shell commands, parsers, system calls, and utility functions for process management.

## Directory Structure

```
Operating-Systems-and-Systems-Programming/
│── Assignment 2/
│   ├── Report/
│   │   ├── report.pdf
│   ├── include/
│   │   ├── command.h
│   │   ├── log.h
│   │   ├── parser.h
│   │   ├── shell_builtins.h
│   │   ├── utils.h
│   ├── src/
│   │   ├── command.c
│   │   ├── main.c
│   │   ├── parser.c
│   │   ├── shell_builtins.c
│   │   ├── utils.c
```

## Features

- **Shell Implementation**: A basic shell supporting command execution and built-in functions.
- **Command Parser**: A parser that tokenizes user input into commands and arguments.
- **Logging Mechanism**: A logging utility for debugging.
- **Process Management**: Handling process creation using `fork()`, `exec()`, and `wait()`.
- **File Handling**: Implementation of file redirection and piping.

## Installation

1. Clone this repository:
   ```sh
   git clone https://github.com/aswinvmohanyeroor/Operating-Systems-and-Systems-Programming.git
   cd Operating-Systems-and-Systems-Programming/Assignment 2
   ```

2. Compile the source code:
   ```sh
   gcc -o shell src/*.c -Iinclude
   ```

3. Run the shell:
   ```sh
   ./shell
   ```

## Usage

- Run built-in shell commands:
  ```
  ls -l
  cd /home/user
  echo "Hello, World!"
  ```
- Execute programs:
  ```
  ./a.out
  python3 script.py
  ```
- Use pipes and redirection:
  ```
  ls | grep ".c"
  cat file.txt > output.txt
  ```

## Contributing

Contributions are welcome! If you would like to contribute:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Commit your changes (`git commit -m "Add new feature"`).
4. Push to the branch (`git push origin feature-branch`).
5. Create a Pull Request.

----------------------------