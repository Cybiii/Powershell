# Shell

A simple cross-platform shell implemented in C++.

## Features

- Executes external commands.
- A suite of built-in commands for filesystem, text, and system information.
- Customizable prompt that displays the current working directory.
- Command history accessible via the `history` command.
- Color-coded and formatted output for commands like `ls` and `pwd`.
- A friendly welcome message on startup.

## Built-in Commands

### Filesystem

| Command           | Description                                             | Usage                          |
| ----------------- | ------------------------------------------------------- | ------------------------------ |
| `ls [path]`       | Lists the contents of a directory in a detailed format. | `ls` or `ls src`               |
| `pwd`             | Prints the current working directory.                   | `pwd`                          |
| `cd [path]`       | Changes the current working directory.                  | `cd src`                       |
| `mkdir [name]`    | Creates a new directory.                                | `mkdir new_folder`             |
| `rm [name]`       | Removes a file or an empty directory.                   | `rm old_file.txt`              |
| `touch [name]`    | Creates an empty file or updates its timestamp.         | `touch new_file.txt`           |
| `cp [src] [dest]` | Copies a file.                                          | `cp file.txt backup.txt`       |
| `mv [src] [dest]` | Moves or renames a file.                                | `mv old_name.txt new_name.txt` |

### Text Manipulation

| Command             | Description                                 | Usage                    |
| ------------------- | ------------------------------------------- | ------------------------ |
| `cat [file]`        | Displays the content of a text file.        | `cat README.md`          |
| `grep [pat] [file]` | Searches for a pattern in a text file.      | `grep "CMake" README.md` |
| `head [file]`       | Displays the first 10 lines of a text file. | `head README.md`         |
| `head -n 15 [file]` | Displays the first 15 lines of a text file. | `head -n 15 README.md`   |
| `tail [file]`       | Displays the last 10 lines of a text file.  | `tail README.md`         |
| `tail -n 15 [file]` | Displays the last 15 lines of a text file.  | `tail -n 15 README.md`   |

### System & Session

| Command       | Description                                           | Usage                |
| ------------- | ----------------------------------------------------- | -------------------- |
| `history`     | Displays the command history for the current session. | `history`            |
| `date`        | Displays the current date and time.                   | `date`               |
| `hostname`    | Displays the computer name.                           | `hostname`           |
| `echo [text]` | Prints text to the console.                           | `echo Hello, World!` |
| `exit`        | Exits the shell.                                      | `exit`               |

## Building the Project

This project uses CMake to generate build files.

### Prerequisites

- A C++17 compliant compiler (e.g., GCC, Clang, MSVC)
- CMake (version 3.10 or later)

### Build Steps

1.  **Clone the repository (if you haven't already):**

    ```bash
    git clone <repository-url>
    cd Powershell
    ```

2.  **Create a build directory:**

    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project with CMake:**

    ```bash
    cmake ..
    ```

4.  **Build the project:**
    - On Windows (with Visual Studio):
      ```bash
      cmake --build . --config Release
      ```
    - On Linux/macOS:
      ```bash
      make
      ```

The executable `shell` (or `shell.exe` on Windows) will be created in the `build` directory.

## Running the Shell

Navigate to the build directory and run the executable:

```bash
./shell
```

Or on Windows:

```bash
.\\shell.exe
```
