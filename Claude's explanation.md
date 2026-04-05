# OS Simulation Project — Technical Breakdown

Here's how all the pieces fit together:

## The Big Picture

You're building a **simulated operating system** in Java that reads text files as "programs", loads them into a fake memory array, and executes their instructions while managing multiple processes concurrently using a scheduler.

---

## 1. Memory (The Foundation)

You have a single array of **40 "words"** (think `String[40]`) that stores everything:

- **PCB data** for each process (its ID, state, program counter, memory boundaries)
- **The actual lines of code** of the program
- **Variables** the program creates at runtime (up to 3 per process)

When a process is created, you carve out a slice of this array for it. For example:
- Process 1 gets slots `0–17`
- Process 2 gets slots `18–35`
- etc.

If memory is full when a new process arrives, you **swap** an existing process out to disk (a text file), freeing its memory slots, then load the new one in.

---

## 2. The Interpreter (Core Engine)

You write a parser that reads each line of a program file and executes it like a mini language. The supported instructions are:

```
assign x 5         → store variable x = 5 in memory
print x            → look up x in memory, print its value
writeFile f data   → write data to a real file on disk
readFile f         → read from a real file, return contents
printFromTo 1 5    → print 1, 2, 3, 4, 5
semWait userOutput → acquire a mutex before using a resource
semSignal userOutput → release the mutex after
```

The interpreter doesn't run the whole file at once — it executes **one instruction at a time**, advancing a **program counter** (just an integer index) stored in the PCB.

---

## 3. Process Control Block (PCB)

Each process has a PCB stored inside the memory array itself. It tracks:

| Field | What it does |
|---|---|
| `processID` | Unique identifier (1, 2, 3...) |
| `state` | Ready / Running / Blocked / Finished |
| `programCounter` | Which line of code to execute next |
| `memoryBoundaries` | The start and end index in the 40-word array |

---

## 4. The Scheduler (The Brain)

The scheduler runs a **clock loop** — each tick it decides which process runs. You implement two algorithms:

**Round Robin (RR):**
- Processes take turns, executing **2 instructions per turn**
- If not done after 2 instructions → goes back to end of the Ready Queue

**HRRN (Highest Response Ratio Next):**
- Non-preemptive — once a process starts, it runs to completion
- At each scheduling decision, calculates for each waiting process:
  ```
  Response Ratio = (Waiting Time + Burst Time) / Burst Time
  ```
- Picks the process with the highest ratio

The main loop looks roughly like this:

```
clock = 0
loop:
    check if any new process arrives at this clock tick → create it
    pick next process from Ready Queue (based on algorithm)
    execute up to N instructions
    if process tried to use a locked mutex → move to Blocked Queue
    if process finished → mark as Finished
    print memory state, queue state
    clock++
```

---

## 5. Mutexes (Mutual Exclusion)

You create **3 mutex objects**, one for each shared resource:
- `userOutput` (the screen)
- `userInput` (keyboard)
- `file` (disk read/write)

Each mutex has:
- A **key** (boolean: locked or not)
- A **blocked queue** (list of processes waiting for it)

When a process calls `semWait userOutput`:
- If the mutex is **free** → lock it, process continues
- If **locked** → move the process to the Blocked Queue, schedule something else

When a process calls `semSignal userOutput`:
- Unlock the mutex
- If anything is in the blocked queue → wake the first one up, move it to Ready Queue

---

## 6. Disk Swapping

When memory is full and a new process needs to be created:
1. Pick a process to evict (e.g., one that's not currently running)
2. Write its entire memory slice (PCB + code + variables) to a `.txt` file on disk
3. Free those memory slots
4. Load the new process into that freed space
5. When the evicted process needs to run again → read it back from disk, find free memory, load it back in

---

## How It All Connects

```
Clock Tick 0: P1 arrives → load into memory[0..17], add to Ready Queue
Clock Tick 1: P2 arrives → load into memory[18..35], add to Ready Queue
              Scheduler picks P1 (RR) → execute 2 instructions → back to queue
Clock Tick 4: P3 arrives → memory full? → swap P2 to disk → load P3
              P2 stays in Ready Queue but its data lives on disk now
...when P2's turn comes → swap it back in first, then execute
```

---

## What to Build (Work Order)

A sensible build order would be:

1. **Memory array + PCB struct** — get the data structure right first
2. **Interpreter** — parse and execute single instructions
3. **Process creation** — read a file, allocate memory, build PCB
4. **Round Robin scheduler** — simplest scheduling algorithm
5. **Mutexes + blocked queue** — add resource locking
6. **Disk swapping** — handle memory overflow
7. **HRRN** — add the second scheduling algorithm
8. **GUI** — visualize queues, memory, clock (mandatory for Java)

The trickiest parts are the **memory management** (keeping track of which slots belong to which process) and **mutex blocking** (correctly moving processes between queues). Getting those right first makes everything else much easier.