//-----------------------------------------------------------------------------
// Copyright 2021, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

#include "Free.h"
#include "Used.h"

// Add code here
UsedNode::UsedNode(unsigned int blockSize)
    :mBlockSize((uint16_t)blockSize), mAboveBlockFree(false), mType(Block::Used ), pUsedNext(nullptr), pUsedPrev(nullptr)
{

}

// ---  End of File ---

