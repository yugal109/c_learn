# CPro

A collection of C projects built for practice, systems understanding, and tool-building.

This repository is organized as a multi-project workspace. Each project lives in its own folder with source code, build setup, and usage notes.

## Project Index

| #   | Project | Status  | Folder   | Summary                                                   |
| --- | ------- | ------- | -------- | --------------------------------------------------------- |
| 1   | cgrep   | Active  | `cgrep/` | A tiny grep-like CLI that searches text patterns in files |
| 2   | TBA     | Planned | -        | Upcoming project                                          |
| 3   | TBA     | Planned | -        | Upcoming project                                          |

## Project 1: cgrep

`cgrep` is a lightweight grep-style command-line utility written in C.

### Highlights

- Substring-based pattern search using `strstr`
- Optional line number output with `-n`
- Search across one or many files
- Clear error output for missing files

### Layout

- `cgrep/main.c` - CLI argument handling and execution flow
- `cgrep/search.c` - line-by-line file scanning and match printing
- `cgrep/search.h` - search function declaration
- `cgrep/Makefile` - build and clean targets
- `cgrep/build/` - compiled binary output

### Build

```bash
cd cgrep
make
```

Binary output:

```bash
./build/cgrep
```

### Usage

```bash
./build/cgrep [-n] pattern file...
```

Examples:

```bash
./build/cgrep ERROR logs.txt
./build/cgrep -n ERROR logs.txt
./build/cgrep -n TODO main.c search.c
```

### Notes

- Matching is case-sensitive.
- Pattern matching is substring-based (not regex).
- Build uses AddressSanitizer (`-fsanitize=address`) for debugging safety.

## Roadmap

- Add more standalone C tools as separate project folders.
- Keep each project documented with goals, usage, and known limitations.
