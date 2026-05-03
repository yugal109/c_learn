# yugshellE

Extended `yugshell` with support for:

- input redirection (`<`)
- output redirection (`>`, `>>`)
- single pipeline (`cmd1 | cmd2`)

This documentation is based on the current `yugshellE` source.

## Core flow

`main.c` runs an interactive loop:

1. print prompt `yugshell>: `
2. read line from stdin via `read_line()`
3. break if line is empty / EOF-style path
4. parse with `parse_pipeline(line)`
5. if pipeline -> `execute_pipeline()`
6. else -> `execute(left_command)`
7. free line + pipeline structures
8. continue while status is non-zero

## Block diagram (what you built, at a glance)

One picture of the architecture: **read → parse → run → clean up → repeat**. Not every `if`—just how the pieces connect.

```mermaid
flowchart TD
    subgraph MAIN["main.c — shell loop"]
        A[Print prompt] --> B["read_line() — input.c"]
        B --> C{Empty line?}
        C -->|yes| STOP[Leave loop]
        C -->|no| D["parse_pipeline() — parser.c"]
        D --> E{Line had a | pipe?}
        E -->|no| F["execute(left Command) — executor.c"]
        E -->|yes| G["execute_pipeline() — executor.c"]
        F --> H["free line + free_pipeline"]
        G --> H
        H --> I{status non-zero?}
        I -->|yes| A
        I -->|no| STOP
    end

    subgraph PARSE["parser.c — what parsing does"]
        D --> D1["Split first | only: left + right Command"]
        D1 --> D2["parse_line: strtok + redirect paths on Command"]
    end

    subgraph RUN1["Single command — executor.c"]
        F --> R1{Builtin? builtins.c}
        R1 -->|cd / help / exit| R2[Run in shell process]
        R1 -->|external| R3[fork → child: setup_redirection → execvp]
        R3 --> R4[Parent waitpid]
    end

    subgraph RUN2["Pipeline — executor.c"]
        G --> P1[pipe]
        P1 --> P2[Left child: stdout → pipe; execvp]
        P1 --> P3[Right child: stdin ← pipe; execvp]
        P2 --> P4[Parent closes pipe ends; wait both]
        P3 --> P4
    end

    subgraph REDIR["Redirection — setup_redirection()"]
        R3 -.-> X["input file → dup2 to stdin"]
        R3 -.-> Y["output file → dup2 to stdout"]
        P2 -.-> X
        P3 -.-> X
        P2 -.-> Y
        P3 -.-> Y
    end
```

### Read this diagram like this

- **input.c**: one line of text into a growable buffer.
- **parser.c**: turns that line into `Command` structs (args + optional redirect paths); if there is `|`, you get **two** commands.
- **executor.c**: either one process path (`execute`) or pipe + two children (`execute_pipeline`); **redirection** is always **open + dup2** right before **execvp** in the child.
- **builtins.c**: only on the **non-pipeline** path via `execute`; `exit` is what returns **0** and stops the loop.

For a separate visual asset, see `yugshellE_block_diagram.svg` in this folder.

## Components

- `input.c` / `input.h`
  - dynamic input buffer (`malloc` + `realloc`)
  - returns heap string

- `command.h`
  - `Command`:
    - `args`
    - `input_file`
    - `output_file`
    - `append`

- `parser.c` / `parser.h`
  - `parse_line()` tokenizes command
  - handles `<`, `>`, `>>` metadata
  - stores non-redirection tokens in NULL-terminated `args`
  - `parse_pipeline()` splits once at first `|`
  - `Pipeline` has `left`, `right`, `has_pipe`
  - `free_command()` and `free_pipeline()` free parser allocations

- `executor.c` / `executor.h`
  - `execute(Command *cmd)`:
    - built-in dispatch for `cd/help/exit`
    - external command with `fork + execvp`
    - applies redirection before `execvp`
  - `execute_pipeline(Pipeline *pipeline)`:
    - creates pipe
    - forks left child (stdout -> pipe write)
    - forks right child (stdin <- pipe read)
    - waits for both children

- `builtins.c` / `builtins.h`
  - `cd`, `help`, `exit`
  - `exit` returns `0` to stop loop

## Redirection behavior

Parsed in `parse_line()`:

- `>` -> `output_file`, truncate mode
- `>>` -> `output_file`, append mode
- `<` -> `input_file`

Applied in `setup_redirection()`:

- input: `open(..., O_RDONLY)` then `dup2(fd, STDIN_FILENO)`
- output: `open(..., O_WRONLY|O_CREAT|O_APPEND|O_TRUNC logic, 0644)` then `dup2(fd, STDOUT_FILENO)`

## Pipeline behavior

For `left | right`:

- `pipe(pipefd)`
- child 1 uses `pipefd[1]` as stdout, executes left
- child 2 uses `pipefd[0]` as stdin, executes right
- parent closes both ends and waits both pids

## Build and run

From `yugshellE/`:

```bash
make
./build/yugshell
```

Clean:

```bash
make clean
```

## Notes for revision

- tokenization is whitespace-based (`strtok`), no quote support
- model supports one pipe split (`left/right`) only
- parser token pointers reference the input line buffer
- built-ins are in non-pipeline execute path
- loop can end via builtin `exit` (status `0`) or empty-line break in `main.c`

## Diagram (SVG)

Same story as the Mermaid block diagram above, as a standalone image you can open or drop into slides:

- `yugshellE_block_diagram.svg` (in this directory)
