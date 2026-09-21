#include "MemoryPool.h"

#include <cstdint>
#include <limits>
#include <stdexcept>

namespace
{
    std::size_t checkedCapacity(std::size_t blockSize,
                                std::size_t blockCount)
    {
        if (blockSize == 0 || blockCount == 0)
        {
            throw std::invalid_argument(
                "Block size and block count must be greater than zero");
        }

        if (blockCount >
            std::numeric_limits<std::size_t>::max() / blockSize)
        {
            throw std::overflow_error("Memory pool size is too large");
        }

        return blockSize * blockCount;
    }
}

MemoryPool::MemoryPool(std::size_t blockSize,
                       std::size_t blockCount)
    : blockSize_(blockSize),
      blockCount_(blockCount),
      totalBytes_(checkedCapacity(blockSize, blockCount)),
      memory_(std::make_unique<unsigned char[]>(totalBytes_)),
      inUse_(std::make_unique<bool[]>(blockCount_)),
      freeBlocks_(blockCount_)
{
    for (std::size_t i = 0; i < blockCount_; ++i)
    {
        freeBlocks_.push(memory_.get() + i * blockSize_);
    }
}

MemoryPool::~MemoryPool() = default;

void *MemoryPool::allocate()
{
    if (freeBlocks_.empty())
    {
        return nullptr;
    }

    void *block = freeBlocks_.pop();

    auto *bytes = static_cast<unsigned char *>(block);
    std::size_t index =
        static_cast<std::size_t>(bytes - memory_.get()) / blockSize_;

    inUse_[index] = true;

    return block;
}

bool MemoryPool::deallocate(void *ptr)
{
    if (ptr == nullptr)
    {
        return false;
    }

    // Compare numeric addresses without subtracting unrelated pointers.
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(ptr);
    std::uintptr_t start =
        reinterpret_cast<std::uintptr_t>(memory_.get());

    if (address < start)
    {
        return false;
    }

    std::uintptr_t offset = address - start;

    // The address must be inside the pool.
    if (offset >= totalBytes_)
    {
        return false;
    }

    // The address must point to the beginning of a block.
    if (offset % blockSize_ != 0)
    {
        return false;
    }

    std::size_t index =
        static_cast<std::size_t>(offset / blockSize_);

    // Reject blocks that are already free.
    if (!inUse_[index])
    {
        return false;
    }

    freeBlocks_.push(ptr);
    inUse_[index] = false;

    return true;
}

std::size_t MemoryPool::availableBlocks() const
{
    return freeBlocks_.size();
}

std::size_t MemoryPool::allocatedBlocks() const
{
    return blockCount_ - freeBlocks_.size();
}

std::size_t MemoryPool::blockSize() const
{
    return blockSize_;
}

std::size_t MemoryPool::capacity() const
{
    return totalBytes_;
}