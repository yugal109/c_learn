# yugallocator

A mini custom allocator written in C with explicit **TINY / SMALL / LARGE** size classes, zone management via `mmap`, block splitting, coalescing, and debug dumps.

If you ever wondered "where did my pointer come from?" or "why is this range 8 bytes?", this project makes memory layout visible.

---

## Quick Mental Model

- `my_malloc(size)` picks a size class:
  - `size <= 128` -> **TINY**
  - `size <= 1024` -> **SMALL**
  - `size > 1024` -> **LARGE**
- TINY/SMALL allocations come from reusable zones (big mapped chunks containing many blocks).
- LARGE allocations get their own dedicated zone (1 zone per allocation).
- Every allocation has a `BlockHeader` immediately before user memory.
- Every zone starts with a `ZoneHeader`.
- `my_free(ptr)` marks block free, coalesces (for non-LARGE), and unmaps the whole zone if it becomes empty.

---

## Project Structure (What Does What)

- `include/allocator.h`
  - Constants (`TINY_MAX`, `SMALL_MAX`, `ALIGNMENT`, `MIN_ALLOCS`)
  - Core structs (`BlockHeader`, `ZoneHeader`)
  - Public API (`my_malloc`, `my_free`, `my_realloc`, `my_calloc`, etc.)
- `src/allocator.c`
  - Main allocation/free/realloc logic
  - Block splitting
  - Pointer -> zone lookup
  - Coalescing adjacent free blocks
- `src/zones.c`
  - Creates zones with `mmap`
  - Finds free block in zone lists
  - Adds/removes zones from linked lists
  - Unmaps empty zones
- `src/calloc.c`
  - `my_calloc` and `my_reallocf`
- `src/debug.c`
  - Human-readable memory and zone dump helpers
- `tests/main.c`
  - End-to-end behavior tests (tiny/small/large/calloc/realloc/reuse/stress)
- `Makefile`
  - Build static lib + run tests

---

## Data Layout

Inside a zone, memory is linear:

```text
[ ZoneHeader ][ BlockHeader ][ user bytes ][ BlockHeader ][ user bytes ] ...
```

Key point:
- User pointer returned by `my_malloc` points to `BlockHeader + HEADER_SIZE`.
- On free, allocator recovers header via `ptr - HEADER_SIZE`.

---

## Flow Diagram: Allocation Path

```mermaid
flowchart TD
    A[my_malloc(size)] --> B{size == 0?}
    B -- yes --> Z[return NULL]
    B -- no --> C[ALIGN(size, 8)]
    C --> D{<=128? <=1024? >1024?}
    D -->|<=128| E[TINY list]
    D -->|<=1024| F[SMALL list]
    D -->|>1024| G[LARGE list]

    E --> H[find_free_block]
    F --> H
    H --> I{block found?}
    I -- yes --> J[split_block if possible]
    J --> K[mark block used]
    K --> L[return user ptr]

    I -- no --> M[new_zone via mmap]
    G --> M
    M --> N[add_zone to list]
    N --> O{LARGE?}
    O -- yes --> P[single block in dedicated zone]
    O -- no --> Q[find first free block in new zone]
    P --> L
    Q --> J
```

---

## Flow Diagram: Free Path

```mermaid
flowchart TD
    A[my_free(ptr)] --> B{ptr == NULL?}
    B -- yes --> Z[return]
    B -- no --> C[find_zone_for_ptr across tiny/small/large]
    C --> D{zone found?}
    D -- no --> Z
    D -- yes --> E[header = ptr - HEADER_SIZE]
    E --> F[mark block free]
    F --> G{zone type != LARGE?}
    G -- yes --> H[coalesce adjacent free blocks]
    G -- no --> I[skip coalesce]
    H --> J{zone_is_empty?}
    I --> J
    J -- yes --> K[remove_zone + munmap]
    J -- no --> Z
```

---

## Zones Explained Clearly

Think of a zone as a "container" obtained from the OS using `mmap`.

- **TINY zone**
  - Designed to serve many tiny allocations.
  - Created with a fixed target size:  
    `ZONE_HEADER_SIZE + (HEADER_SIZE + 128) * MIN_ALLOCS`
- **SMALL zone**
  - Similar idea, larger block budget:  
    `ZONE_HEADER_SIZE + (HEADER_SIZE + 1024) * MIN_ALLOCS`
- **LARGE zone**
  - Dedicated to one allocation:  
    `ZONE_HEADER_SIZE + HEADER_SIZE + aligned_user_size`

In your code:
- `MIN_ALLOCS = 100`
- TINY and SMALL zones are "capacity pools"
- LARGE is one-allocation-per-zone for simplicity and clean unmapping

---

## `mmap` on macOS (Apple) and 16KB Pages

`mmap` gives virtual memory in **page-sized chunks**.  
On many Apple systems (especially Apple Silicon), page size is often **16KB = 16384 bytes**.

Important behavior:
- Even if you ask for non-page-aligned length, the mapping is page-granular.
- Conceptually: requested size is rounded up to page multiples internally.

### Why this matters here

Your zone formulas may produce sizes like 15216 bytes, but OS mapping operates in page units.

Using common 64-bit struct sizes:
- `ZoneHeader`: 16 bytes
- `BlockHeader`: 24 bytes (aligned to 8)

Then:
- `TINY_ZONE_SIZE = 16 + (24 + 128) * 100 = 15216`
  - Fits in one 16KB page (rounded mapping footprint: 16384)
- `SMALL_ZONE_SIZE = 16 + (24 + 1024) * 100 = 104816`
  - Rounded to 7 pages on a 16KB system: `7 * 16384 = 114688`

So your allocator's logical zone size (`zone->size`) is what your metadata uses internally, while OS backing is page-based.

If you want to verify page size on your machine:

```bash
getconf PAGESIZE
```

---

## Size Classes: `<128`, `<1024`, and Large

Current boundaries in code:

- **TINY**: `size <= 128`
- **SMALL**: `129..1024`
- **LARGE**: `>= 1025`

Why this split exists:
- Small allocations are frequent -> pooling in reusable zones reduces `mmap/munmap` overhead.
- Huge allocations are less frequent / variable -> dedicated zone avoids fragmentation complexity in pooled zones.
- This is a classic tradeoff: speed for small objects, simplicity for big ones.

---

## Block Splitting and Coalescing

### Splitting (`split_block`)

When a free block is larger than requested:
- allocate only what is needed
- leave the remainder as a new free block

This helps reduce internal waste.

### Coalescing (`coalesce`)

On `my_free` (for TINY/SMALL):
- adjacent free blocks are merged into bigger free blocks
- reduces external fragmentation

If after coalescing every block in zone is free:
- zone is removed from list
- zone is released with `munmap`

---

## Debug Output: How to Read It

`show_alloc_mem()` prints lines like:

```text
0x104fdc028 - 0x104fdc030 : 8 bytes
```

This is correct because addresses are hexadecimal:
- `0x30 - 0x28 = 0x08` => 8 bytes

Range is treated as `[start, end)`, so end address is one-past-last-byte.

---

## Build and Run

From `yugallocator/`:

```bash
make
make test
```

What this does:
- builds objects into `build/`
- creates `build/libyugalloc.a`
- compiles and runs `tests/main.c`

---

## Known Limitations / Next Steps

- No thread safety (no locks around global zone lists).
- `my_realloc(NULL, size)` currently returns `NULL` (standard `realloc` usually behaves like `malloc`).
- `my_calloc` does not check multiplication overflow (`count * size`).
- Freeing invalid pointers is only softly ignored (pointer-in-zone check).
- No best-fit/first-fit policy tuning or fragmentation metrics yet.

---

## TL;DR

- You implemented a real zone allocator with clean building blocks:
  - classify size
  - reuse pooled zones for small allocations
  - dedicated mappings for large allocations
  - split on allocate, coalesce on free
  - unmap empty zones
- The `<128 / <1024 / large` strategy is exactly how allocator design starts in serious systems.
