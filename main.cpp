#include "MemoryPool.h"

#include <cstring>
#include <iomanip>
#include <iostream>

int main()
{
    MemoryPool pool(512, 8);

    std::cout << "Network Packet Buffer Pool\n\n";
    std::cout << "Block size: " << pool.blockSize() << " bytes\n";
    std::cout << "Blocks: "
              << pool.availableBlocks() + pool.allocatedBlocks() << '\n';
    std::cout << "Total capacity: " << pool.capacity() << " bytes\n\n";

    // Allocate three blocks.
    void *packet1 = pool.allocate();
    void *packet2 = pool.allocate();
    void *packet3 = pool.allocate();

    std::cout << "Packet 1 allocated: " << packet1 << '\n';
    std::cout << "Packet 2 allocated: " << packet2 << '\n';
    std::cout << "Packet 3 allocated: " << packet3 << '\n';

    if (!packet1 || !packet2 || !packet3)
    {
        std::cerr << "Initial allocation failed.\n";
        return 1;
    }

    std::cout << "\nAvailable blocks: " << pool.availableBlocks() << '\n';
    std::cout << "Allocated blocks: " << pool.allocatedBlocks() << '\n';

    // Store binary data, including zero bytes.
    unsigned char data[] = {
        0x45, 0x00, 0x00, 0x3C,
        0xAB, 0xCD, 0x12, 0x34};

    if (sizeof(data) > pool.blockSize())
    {
        std::cerr << "Packet data exceeds the block size.\n";
        return 1;
    }

    std::memcpy(packet1, data, sizeof(data));

    auto *storedData = static_cast<unsigned char *>(packet1);

    std::cout << "\nBinary data read back: ";
    for (std::size_t i = 0; i < sizeof(data); ++i)
    {
        std::cout << std::hex << std::uppercase
                  << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(storedData[i]) << ' ';
    }
    std::cout << std::dec << std::setfill(' ') << '\n';

    if (std::memcmp(packet1, data, sizeof(data)) != 0)
    {
        std::cerr << "Binary data verification failed.\n";
        return 1;
    }
    std::cout << "Binary data verified successfully.\n";

    // Release a block and immediately allocate again.
    if (!pool.deallocate(packet2))
    {
        std::cerr << "Packet 2 release failed.\n";
        return 1;
    }

    std::cout << "\nPacket 2 released.\n";
    std::cout << "Available blocks: " << pool.availableBlocks() << '\n';
    std::cout << "Allocated blocks: " << pool.allocatedBlocks() << '\n';

    void *packet4 = pool.allocate();
    std::cout << "\nPacket 4 allocated: " << packet4 << '\n';

    if (packet4 != packet2)
    {
        std::cerr << "Memory reuse verification failed.\n";
        return 1;
    }
    std::cout << "Packet 4 reused the released block.\n";

    // Allocate all remaining blocks.
    std::cout << "\nAttempting to exhaust pool...\n";
    while (pool.availableBlocks() > 0)
    {
        void *block = pool.allocate();
        std::cout << "Allocated: " << block << '\n';
    }

    void *extra = pool.allocate();
    if (extra != nullptr)
    {
        std::cerr << "Exhaustion verification failed.\n";
        return 1;
    }

    std::cout << "allocate() returned nullptr: pool exhausted.\n";
    std::cout << "Available blocks: " << pool.availableBlocks() << '\n';
    std::cout << "Allocated blocks: " << pool.allocatedBlocks() << '\n';

    // Reject a second release of the same block.
    std::cout << "\nAttempting double deallocation...\n";
    bool firstRelease = pool.deallocate(packet1);
    bool secondRelease = pool.deallocate(packet1);

    std::cout << std::boolalpha;
    std::cout << "First deallocation: " << firstRelease << '\n';
    std::cout << "Second deallocation: " << secondRelease << '\n';

    if (!firstRelease || secondRelease)
    {
        std::cerr << "Double deallocation verification failed.\n";
        return 1;
    }
    std::cout << "Double deallocation rejected.\n";

    // Check other invalid pointers.
    int outsideValue = 0;
    bool nullRejected = !pool.deallocate(nullptr);
    bool outsideRejected = !pool.deallocate(&outsideValue);
    bool interiorRejected = !pool.deallocate(
        static_cast<unsigned char *>(packet3) + 1);

    std::cout << "\nNull pointer rejected: " << nullRejected << '\n';
    std::cout << "Outside pointer rejected: " << outsideRejected << '\n';
    std::cout << "Interior pointer rejected: " << interiorRejected << '\n';

    if (!nullRejected || !outsideRejected || !interiorRejected)
    {
        std::cerr << "Invalid pointer verification failed.\n";
        return 1;
    }

    std::cout << "\nFinal available blocks: "
              << pool.availableBlocks() << '\n';
    std::cout << "Final allocated blocks: "
              << pool.allocatedBlocks() << '\n';

    return 0;
}