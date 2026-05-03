# yugshell

A minimal Unix-like shell written in C for learning process control, tokenization, and built-ins.

This README is based directly on the current `yugshell` source files in this repository.

## What yugshell does

- Shows the prompt `yugshell>: `
- Reads one command line from standard input
- Splits input by whitespace into `argv`-style tokens
- Runs built-ins (`cd`, `help`, `exit`) inside the shell process
- Runs all other commands as external programs using `fork()` + `execvp()`
- Waits for child process completion for external commands
- Repeats until `exit` returns status `0`

## High-level execution flow

The control loop in `main.c` is:

1. print prompt
2. `read_line()` from `input.c`
3. `parse_line()` from `parser.c`
4. `execute()` from `executor.c`
5. `free(line)` and `free(args)`
6. continue while status is non-zero

`main()` exits after loop termination and prints `exiting yugshell.`

## Detailed block diagram (in README)

This is the most detailed runtime diagram in text form so you can revise from README alone.

```mermaid
flowchart TD
    A["Start: main() in main.c"] --> B["loop() begins"]
    B --> C["Print prompt: yugshell>: "]
    C --> D["read_line() in input.c"]

    subgraph INPUT["Input stage (input.c)"]
      D --> D1["Allocate buffer (initial size = 64)"]
      D1 --> D2["Read char using getchar()"]
      D2 --> D3{"char is EOF or newline?"}
      D3 -- "No" --> D4["Store char in buffer[position], position++"]
      D4 --> D5{"position >= buffer size?"}
      D5 -- "Yes" --> D6["realloc(buffer, size*2)"]
      D6 --> D2
      D5 -- "No" --> D2
      D3 -- "Yes" --> D7["Append \\0 and return char* line"]
    end

    D7 --> E["parse_line(line) in parser.c"]

    subgraph PARSER["Parse stage (parser.c)"]
      E --> E1["Allocate token array (initial count = 8)"]
      E1 --> E2["token = strtok(line, ' \\t\\r\\n\\a')"]
      E2 --> E3{"token != NULL?"}
      E3 -- "Yes" --> E4["tokens[position] = token; position++"]
      E4 --> E5{"position >= token capacity?"}
      E5 -- "Yes" --> E6["realloc(tokens, capacity*2)"]
      E6 --> E7["token = strtok(NULL, delimiters)"]
      E5 -- "No" --> E7
      E7 --> E3
      E3 -- "No" --> E8["tokens[position] = NULL"]
      E8 --> E9["Return char** args"]
    end

    E9 --> F["execute(args) in executor.c"]
    F --> F1{"args[0] == NULL?"}
    F1 -- "Yes" --> Z1["Return status=1 (continue loop)"]

    F1 -- "No" --> F2["is_builtin(args) in builtins.c"]
    F2 --> F3{"Builtin command?"}

    subgraph BUILTIN["Builtin path (parent process)"]
      F3 -- "Yes" --> G["run_builtin(args)"]
      G --> G1{"args[0] == cd?"}
      G1 -- "Yes" --> G2{"args[1] exists?"}
      G2 -- "No" --> G3["Print: missing argument; return 1"]
      G2 -- "Yes" --> G4["chdir(args[1]); return 1"]

      G --> G5{"args[0] == help?"}
      G5 -- "Yes" --> G6["Print built-in help; return 1"]

      G --> G7{"args[0] == exit?"}
      G7 -- "Yes" --> G8["Return 0 (stop shell loop)"]
    end

    subgraph EXTERNAL["External command path (executor.c)"]
      F3 -- "No" --> H["fork()"]
      H --> H1{"pid == 0 (child)?"}
      H1 -- "Yes" --> H2["execvp(args[0], args)"]
      H2 --> H3{"execvp failed?"}
      H3 -- "Yes" --> H4["perror('yugshell'); exit(1)"]

      H1 -- "No" --> H5{"pid < 0?"}
      H5 -- "Yes" --> H6["perror('yugshell: fork failed')"]
      H5 -- "No" --> H7["Parent waitpid(pid, &status, 0)"]
      H7 --> H8["Return status=1"]
    end

    Z1 --> R["main.c frees line and args"]
    G3 --> R
    G4 --> R
    G6 --> R
    H8 --> R
    H6 --> R

    G8 --> X["loop ends (status = 0)"]
    R --> Y{"status != 0?"}
    Y -- "Yes" --> C
    Y -- "No" --> X
    X --> X1["Print: exiting yugshell."]
    X1 --> X2["Program terminates"]
```

### Diagram notes (important)

- Built-ins execute in the shell process; this is why `cd` can change shell state.
- External commands execute in a child process created by `fork()`.
- `args` is NULL-terminated and used as `execvp` argv input.
- `parse_line()` tokens reference memory inside `line`; they are not deep-copied strings.
- Loop exit happens only when builtin `exit` returns `0`.

## Module responsibilities

- `main.c`
  - Owns the interactive loop (`loop()`).
  - Connects input, parser, and executor.
  - Controls shell lifecycle via return status.

- `input.c` / `input.h`
  - Implements `read_line()`.
  - Reads characters with `getchar()` until newline or EOF.
  - Uses dynamic buffer growth (`malloc` + `realloc`) starting from size `64`.
  - Returns a heap string that caller must free.

- `parser.c` / `parser.h`
  - Implements `parse_line(char *line)`.
  - Tokenizes with `strtok(line, " \t\r\n\a")`.
  - Grows token array dynamically from initial token capacity `8`.
  - Returns a NULL-terminated `char **` token list.
  - Tokens point into the original `line` buffer (no deep copy).

- `executor.c` / `executor.h`
  - Implements `execute(char **args)`.
  - Handles empty command (`args[0] == NULL`) by continuing loop.
  - Delegates built-ins to builtin dispatcher.
  - External command path:
    - `fork()`
    - child: `execvp(args[0], args)`
    - parent: `waitpid(pid, &status, 0)`
  - Returns `1` to continue loop, `0` to stop only via builtin `exit`.

- `builtins.c` / `builtins.h`
  - Builtin list: `cd`, `help`, `exit`.
  - `cd`: calls `chdir(args[1])`; prints error when missing arg.
  - `help`: prints usage summary.
  - `exit`: returns `0` to terminate shell loop.
  - `is_builtin()` checks command name membership.
  - `run_builtin()` dispatches to concrete builtin function.

- `Makefile`
  - Compiler: `clang`
  - Flags: `-Wall -Wextra -pedantic -g -fsanitize=address`
  - Output binary: `build/yugshell`
  - Object files go under `build/`

## Build and run

From `yugshell/`:

```bash
make
./build/yugshell
```

Clean build artifacts:

```bash
make clean
```

## Built-in command reference

- `cd [dir]`
  - Changes current working directory of shell process.
  - If no directory is provided, prints: `yugshell: cd: missing argument`

- `help`
  - Prints built-in command help.

- `exit`
  - Ends the shell session.

## External command behavior

For non-built-ins, yugshell relies on `execvp`, so:

- command lookup uses `PATH`
- arguments are passed as tokenized by whitespace
- process is blocking (shell waits until command completes)
- execution errors are shown via `perror("yugshell")`

## Current limitations (important for revision)

These are expected from the current code and not bugs in this README:

- No support for quoted strings (`"a b"` is split into two tokens)
- No pipelines (`|`), redirection (`>`, `<`), or background jobs (`&`)
- No command history or line editing
- `cd` does not default to `$HOME` when argument is omitted
- Parser uses `strtok`, so tokenization is simple whitespace splitting only

## Memory ownership model

- `read_line()` allocates `line` (heap) -> freed in `main.c`
- `parse_line()` allocates `args` array (heap) -> freed in `main.c`
- each `args[i]` points inside `line` buffer -> do not free individually

This ownership is why freeing order in `main.c` works safely.

## Quick revision checklist

When revising yugshell, verify these invariants:

1. Loop continues while `execute()` returns non-zero.
2. Built-ins run in parent shell process (especially `cd`).
3. External commands use `fork` + `execvp` + `waitpid`.
4. `args` remains NULL-terminated.
5. `line` and `args` are freed once per loop iteration.
6. `exit` path returns `0` from builtin to break loop.

## Suggested next evolution steps

- Add quoted-string aware tokenizer
- Add redirection (`>`, `<`, `>>`)
- Add single-pipe support
- Add `cd` with HOME fallback
- Add status code reporting (`$?`-like behavior)
