//-----------------------------------------------------------------------------
// Copyright 2021, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

#include "Used.h"
#include "Free.h"
#include "Block.h"

// Add magic here
FreeNode::FreeNode(unsigned int blockSize) 
    :mBlockSize((uint16_t)blockSize), mAboveBlockFree(false),mType(Block::Free),pFreeNext(nullptr),pFreePrev(nullptr)
{
  
}

// ---  End of File ---
