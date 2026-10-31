# MP3 Tag Reader and Editor in C

## Project Overview

The MP3 Tag Reader and Editor is a command-line application developed using C programming. It allows users to read and modify ID3v2 metadata stored in MP3 audio files.

The project demonstrates practical implementation of file handling, structures, pointers, command-line arguments, and binary file operations in C.

## Features

- View MP3 metadata.
- Edit supported MP3 tag information.
- Read ID3 metadata from MP3 files.
- Update selected metadata fields.
- Command-line based interface.

## Technologies Used

- C Programming
- File Handling
- Structures and Pointers
- Command-Line Arguments
- Binary File Operations
- GCC Compiler

## Project Structure

```text
MP3-Tag-Reader/
│
├── main.c
├── view.c
├── edit.c
├── mp3.h
├── sample.mp3
├── .gitignore
└── README.md
```

## Compilation

Compile the source files using GCC:

```bash
gcc main.c view.c edit.c -o mp3_reader
```

## Execution

### View MP3 Tags

```bash
./mp3_reader -v sample.mp3
```

### Edit MP3 Tags

```bash
./mp3_reader -e -y 2014 sample.mp3
```

*Note: Replace the example option and value with the editing options supported by your program.*

## Concepts Learned

- File handling in C
- Reading and writing binary files
- Structures and pointers
- Command-line argument processing
- MP3 ID3 metadata handling
- Debugging and error handling

## Author

**Varun M M**

Electronics and Communication Engineering Graduate

GitHub: https://github.com/varunmm6
