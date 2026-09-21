# CIS-277 Assignment 1: Network Packet Buffer Pool

## Student

Robeul Alem Nadem

## Description

This program implements a fixed-size memory pool for network packet data. It reserves memory when constructed and uses a custom generic Stack to track available blocks.

The demonstration shows allocation, binary data storage and retrieval, memory reuse, pool exhaustion, and rejection of invalid and duplicate deallocations.

## Stack Implementation

**Dynamic Array**

My Stack uses a dynamically allocated array with a fixed capacity set during construction. The pool knows its block count in advance, so the stack can reserve enough space for every free-block address.

The Stack supports push, pop, top, empty, and size. Each operation takes O(1) time. It does not use `std::stack`.

The pool also maintains an array recording whether each block is allocated, allowing it to reject double deallocation without searching.

## How to Compile

From the repository’s root directory, use a C++17 compiler.

On my Mac, I used:

```bash
clang++ -std=c++17 -Wall -Wextra -pedantic main.cpp MemoryPool.cpp -o buffer_pool
```

With GCC:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp MemoryPool.cpp -o buffer_pool
```

## How to Run

Run the compiled program from the same directory:

```bash
./buffer_pool
```

Memory addresses may differ between runs.

## Analysis Questions

### 1. Why is a Stack appropriate for managing the free blocks in this memory pool?

A Stack provides O(1) push and pop operations for returning and allocating blocks. Its LIFO behavior makes the most recently released block the next block reused. Any available block can satisfy an allocation because all blocks have the same size.

### 2. What happens when the free-block Stack becomes empty?

Every block is currently allocated. The pool’s allocate() checks whether the Stack is empty and returns nullptr if no block is available.

### 3. Why must a released block be returned to the Stack?

The Stack tracks which blocks are available. Returning a released block makes it available for future allocations. Otherwise, the pool would lose track of that reusable space.

### 4. What problem could occur if the same block were deallocated twice?

Its address could appear in the free Stack twice. Two later allocations could then return the same block to different users, allowing their data to overwrite each other. The in-use array prevents duplicate deallocation.

### 5. What is the Big-O time complexity of allocate()? Explain why.

allocate() is O(1). It checks whether the Stack is empty, pops one address, calculates the block index, and updates one in-use entry. None of these operations requires a loop or search.

### 6. What is the Big-O time complexity of deallocate()? Explain why.

deallocate() is O(1). It performs a fixed number of address and alignment checks, calculates the block index, checks one in-use entry, and pushes the address onto the Stack. It does not search through the blocks.