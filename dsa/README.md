# DSA in C: Static Library Project

This project implements three core data structures in C:

- Dynamic array (`Array`)
- Singly linked list (`List`)
- Hash map with separate chaining (`Hashmap`)

All implementations are compiled into a **single static library**: `build/libdsa.a`.
That static library is the core artifact of the project, and tests or future applications link against it.

## Project Goal

The goal is to provide a compact, reusable data-structure toolkit in plain C with:

- Generic storage using `void *` values
- Clean header/source separation
- A build flow that produces a reusable static archive
- A simple test binary that demonstrates usage and behavior

## Project Layout

```text
dsa/
├── include/
│   ├── array.h
│   ├── list.h
│   └── hashmap.h
├── src/
│   ├── array.c
│   ├── list.c
│   └── hashmap.c
├── tests/
│   ├── main.c
│   └── mainc.c
├── Makefile
├── README.md
└── architecture.svg
```

## Build Architecture (Core Idea)

The Makefile compiles each data-structure implementation into object files, then archives them into:

- `build/libdsa.a` (the static library)

Any client (currently `tests/main.c`) is linked with this static library to produce an executable:

- `build/test`

This means the implementation details live inside the archive, while clients consume only the exported APIs from `include/*.h`.

## Visual Diagram

Open `architecture.svg` to see the full flow/block diagram.

It explicitly highlights `libdsa.a` as the center that connects:

- Source modules (`src/*.c`)
- Public headers (`include/*.h`)
- Test/client code (`tests/main.c`)
- Final executable (`build/test`)

## Data Structures and Behavior

### 1) Dynamic Array (`Array`)

Files:

- `include/array.h`
- `src/array.c`

Key behavior:

- Stores pointers (`void **data`)
- Tracks `size` and `capacity`
- Grows automatically by doubling capacity when full
- Supports indexed operations: `get`, `set`, `remove`

Core operations:

- `array_create(initial_capacity)`
- `array_push(arr, item)`
- `array_get(arr, index)`
- `array_set(arr, index, item)`
- `array_remove(arr, index)`
- `array_size(arr)`
- `array_free(arr, free_item)`

Complexity notes:

- Push (amortized): O(1)
- Get/Set by index: O(1)
- Remove at index: O(n) due to shifting

### 2) Singly Linked List (`List`)

Files:

- `include/list.h`
- `src/list.c`

Key behavior:

- Node-based linear structure
- Tracks `head` and `size`
- Supports front and back insertion
- Supports indexed read by traversal

Core operations:

- `list_create()`
- `list_push_front(list, data)`
- `list_push_back(list, data)`
- `list_pop_front(list)`
- `list_get(list, index)`
- `list_size(list)`
- `list_free(list, free_item)`

Complexity notes:

- Push front: O(1)
- Push back: O(n) (traverses to tail)
- Pop front: O(1)
- Get by index: O(n)

### 3) Hash Map (`Hashmap`)

Files:

- `include/hashmap.h`
- `src/hashmap.c`

Key behavior:

- Key-value map using `char *` keys
- Uses hashing (djb2-style) and bucket indexing
- Handles collisions with linked-list chaining
- Automatically resizes when load factor threshold is reached

Core operations:

- `hashmap_create()`
- `hashmap_put(map, key, value)`
- `hashmap_get(map, key)`
- `hashmap_remove(map, key, free_value)`
- `hashmap_size(map)`
- `hashmap_free(map, free_value)`

Complexity notes (average case):

- Put/Get/Remove: O(1)
- Worst case with heavy collisions: O(n)

## Memory Ownership Model

All containers store `void *` references, so value ownership is caller-controlled unless explicitly passed free callbacks.

- `array_free`, `list_free`, and `hashmap_free` accept optional destructor callbacks.
- If callback is `NULL`, only container internals are freed.
- If callback is provided, each stored value is freed first.

This design keeps structures generic and reusable for many value types.

## Build and Run

From the `dsa/` directory:

```bash
make
```

Builds object files and creates `build/libdsa.a`.

Run tests:

```bash
make test
```

This compiles `tests/main.c`, links it with `build/libdsa.a`, runs the binary, and prints test output.

Clean build artifacts:

```bash
make clean
```

## Why Static Library is the Center

The static library model is the key architectural choice in this project:

- It creates a reusable binary package of data structures.
- It cleanly separates implementation (`src`) from usage (`tests` or external apps).
- It keeps build outputs modular and scalable as more structures are added.

In short: **the project is not just three source files; it is a small C library system centered on `libdsa.a`.**

## Extending the Project

To add a new data structure module:

1. Add a public header in `include/`
2. Add implementation in `src/`
3. Add the source file into `SRCS` in `Makefile`
4. Add usage tests in `tests/main.c`
5. Rebuild to include it in `libdsa.a`

The same central flow remains: compile module -> archive into `libdsa.a` -> link clients.
