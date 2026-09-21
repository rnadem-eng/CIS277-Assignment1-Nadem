#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <cstddef>
#include <memory>
#include "Stack.h"

class MemoryPool
{
public:
    MemoryPool(std::size_t blockSize, std::size_t blockCount);
    ~MemoryPool();

    void *allocate();
    bool deallocate(void *ptr);

    std::size_t availableBlocks() const;
    std::size_t allocatedBlocks() const;
    std::size_t blockSize() const;
    std::size_t capacity() const;

private:
    std::size_t blockSize_;
    std::size_t blockCount_;
    std::size_t totalBytes_;

    std::unique_ptr<unsigned char[]> memory_;
    std::unique_ptr<bool[]> inUse_;

    Stack<void *> freeBlocks_;
};

#endif