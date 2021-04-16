//-----------------------------------------------------------------------------
// Copyright 2021, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

#ifndef HEAPHDR_H
#define HEAPHDR_H

#include "Types.h"
#include "Used.h"
#include "Free.h"

class Heap
{
public:
	// Big four
	Heap();
	Heap(const Heap&) = delete;
	Heap& operator =(const Heap&) = delete;
	~Heap() = default;

public:
	// Verify that the Heap is 16 byte aligned.

	// allocation links
	UsedNode	*pUsedHead;//used的頭
	FreeNode	*pFreeHead;//free的頭

	// Next fit allocation strategy
	FreeNode    *pNextFit;//下周的內容

	uint16_t    currNumUsedBlocks;   // number of current used allocations
	uint16_t    currUsedMem;         // current size of the total used memory

	uint16_t    peakNumUsed;         // number of peak used allocations
	uint16_t    peakUsedMemory;      // peak size of used memory

	uint16_t    currNumFreeBlocks;   // number of current free blocks
	uint16_t    currFreeMem;         // current size of the total free memory

	uint32_t    pad0;

	
};

#endif 

// ---  End of File ---
