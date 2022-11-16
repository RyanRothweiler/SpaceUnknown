namespace fixed_allocator {

    struct memory {
        int32 BlockSize;
        int32 BlocksCount;
        int32* FreeBlockIndecies;
        void* Blocks;
    };

    memory Create(int32 BlockSize, int32 BlocksCount)
    {
        memory Alloc = {};
        Alloc.BlockSize = BlockSize;
        Alloc.BlocksCount = BlocksCount;

        Alloc.Blocks = ArenaAllocate(GlobalPermMem, BlockSize * BlocksCount);
        Alloc.FreeBlockIndecies = (int32*)ArenaAllocate(GlobalPermMem, sizeof(*Alloc.FreeBlockIndecies) * BlocksCount);

        for (int x = 0; x < BlocksCount; x++) {
            Alloc.FreeBlockIndecies[x] = x;
        };

        return Alloc;
    }

    void* Alloc(memory* Alloc)
    {
        for (int x = 0; x < Alloc->BlocksCount; x++) {
            if (Alloc->FreeBlockIndecies[x] >= 0)  {
                int index = Alloc->FreeBlockIndecies[x];
                Alloc->FreeBlockIndecies[x] = -1;
                return (void*)((char*)Alloc->Blocks + (index * Alloc->BlockSize));
            }
        }

        //ConsoleLog("Fixed allcoator is full.");
        Assert(0);
        return Alloc->Blocks;
    }

    void Free(memory* Alloc, void* Block)
    {
        for (int x = 0; x < Alloc->BlocksCount; x++) {

            // Find block in array
            if ( (char*)Alloc->Blocks + (x * Alloc->BlockSize) == (char*)Block) {
                ClearMemory((uint8*)Block, Alloc->BlockSize);

                for (int y = 0; y < Alloc->BlocksCount; y++) {
                    if (Alloc->FreeBlockIndecies[y] < 0) {
                        Alloc->FreeBlockIndecies[y] = x;

                        return;
                    }
                }

                // Could not find an empty index slot, this is an error and shouldn't ever happen. Even if we run out of memory.
                Assert(0);
            }
        }

        // That memory is not in this fixed allocator
        Assert(0);
    }
}