//-----------------------------------------------------------------------------
// Copyright 2021, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

#include "Heap.h"
#include "Mem.h"

Heap::Heap()
:	pUsedHead(nullptr),
	pFreeHead(nullptr),
	pNextFit(nullptr)
{
    peakNumUsed = 0;        // number of peak used allocations
    peakUsedMemory  = 0;    // peak size of used memory

    currNumUsedBlocks = 0;  // number of current used allocations
    currUsedMem = 0;        // current size of the total used memory

    currNumFreeBlocks = 0;  // number of current free blocks
    currFreeMem = 0;        // current size of the total free memory

    pad0 = 0;                // pad

}

// ---  End of File ---
