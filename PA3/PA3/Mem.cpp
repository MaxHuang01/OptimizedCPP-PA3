//-----------------------------------------------------------------------------
// Copyright 2021, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

#include "Mem.h"
#include "Heap.h"
#include "Block.h"

// To help with coalescing... not required
struct SecretPtr
{
	FreeNode *pFree;
};

// ----------------------------------------------------
//  Initialized the Memory block:
//  Format the Heap
//  Create a Free block
//  Heap structure links to Free hdr
//-----------------------------------------------------
void Mem::Initialize()
{
	Heap* pHeap = this->poHeap;
	//add a free hdr
	FreeNode* pFreeHdrStart = (FreeNode*)this->pTopAddr;
	FreeNode* pFreeHdrEnd = pFreeHdrStart + 1;

	unsigned int blockSize = (unsigned int)this->pBottomAddr - (unsigned int)pFreeHdrEnd;
	//Trace::out("block size:%d",blockSize);
	FreeNode* pFreeNode = new(pFreeHdrStart) FreeNode(blockSize);

	//update heap
	pHeap->pUsedHead = nullptr;
	pHeap->pFreeHead = pFreeNode;
	pHeap->pNextFit = pFreeNode;

	pHeap->currFreeMem = pFreeNode->mBlockSize;
	pHeap->currNumFreeBlocks++;


}
// ----------------------------------------------------
//  Do your Magic here:
//  Find a Free block that fits
//  Change it to used (may require subdivision)
//  Correct the heap Links (used,free) headers 
//  Update stats
//  Return pointer to block
//-----------------------------------------------------
void *Mem::Malloc( const uint32_t _size )
{
	
	void* toReturn = nullptr;
	Heap* pHeap = this->poHeap;
	FreeNode* pFree = this->poHeap->pFreeHead;///free 游標
	if (this->poHeap->pUsedHead == nullptr) {//若是第一個used node, 還沒有used node
		
		while (pFree != nullptr)
		{
			if ((unsigned int)_size == (unsigned int)pFree->mBlockSize)//find the fit size
			{
				//FreeNode* pFreeHdrStart = pFree;
				FreeNode* pFreeHdrEnd = pFree + 1;
				toReturn = pFreeHdrEnd;
				//UsedNode* pUsed = new(pFreeHdrStart) UsedNode(_size);
				//用cast的方式把Free轉成Used
				UsedNode* pUsed = (UsedNode*)pFree;
				pUsed->mType = Block::Used;
				
				this->poHeap->pUsedHead = pUsed;
				if (pFree->pFreePrev == nullptr && pFree->pFreeNext == nullptr)//pFree is the head and the only node
				{
					this->poHeap->pFreeHead = nullptr;
					this->poHeap->pNextFit = nullptr;
				}
				//update heap
				pHeap->currFreeMem = (uint16_t)((uint16_t)pHeap->currFreeMem - (uint16_t)_size);
				pHeap->currNumFreeBlocks--;
				pHeap->currNumUsedBlocks++;
				pHeap->currUsedMem = (uint16_t)(pHeap->currUsedMem + (uint16_t)_size);
				if (pHeap->currNumUsedBlocks > pHeap->peakNumUsed) 
				{
					pHeap->peakNumUsed = pHeap->currNumUsedBlocks;
				}
				if (pHeap->currUsedMem > pHeap->peakUsedMemory) 
				{
					pHeap->peakUsedMemory = pHeap->currUsedMem;
				}
				break;
			}
			else if ((unsigned int)_size < (unsigned int)pFree->mBlockSize)//needs to subdivision
			{
				//keep the size of original block
				//uint16_t OrgSize = pFree->mBlockSize;
				FreeNode* pFreeHdrEnd = pFree + 1;
				FreeNode* pFreeEnd = (FreeNode*)((uint32_t)pFreeHdrEnd + pFree->mBlockSize);

				//turn free to used and allocate memory
				UsedNode* pNewUsed = (UsedNode*)pFree;
				toReturn = pNewUsed + 1;
				pNewUsed->mType = Block::Used;
				pNewUsed->mBlockSize = (uint16_t)_size;
				UsedNode* pUsedEnd = (UsedNode*)((uint32_t)(pNewUsed + 1) + (uint32_t)_size);


				//deal with remain memory
				FreeNode* pNewFree = (FreeNode*)((uint32_t)(pNewUsed + 1) + (uint32_t)_size);
				pNewFree = new(pNewFree) FreeNode((uint32_t)pFreeEnd - (uint32_t)pUsedEnd-sizeof(FreeNode));


				
				
				//insert used and free to list
				InsertFree(pHeap->pFreeHead,pNewFree);
				InsertUsed(pHeap->pUsedHead, pNewUsed);

				
				pHeap->currFreeMem = (uint16_t)((pHeap->currFreeMem - _size) - ((uint32_t)pFreeHdrEnd - (uint32_t)pFree));
				pHeap->currNumUsedBlocks++;
				pHeap->currUsedMem = (uint16_t)(pHeap->currUsedMem + (uint16_t)_size);
				if (pHeap->currNumUsedBlocks > pHeap->peakNumUsed)
				{
					pHeap->peakNumUsed = pHeap->currNumUsedBlocks;
				}
				if (pHeap->currUsedMem > pHeap->peakUsedMemory)
				{
					pHeap->peakUsedMemory = pHeap->currUsedMem;
				}
				pHeap->pNextFit = pNewFree;
				break;
			}


			pFree = pFree->pFreeNext;
		}
	}
	return toReturn;
	/*AZUL_REPLACE_ME(_size);
	return AZUL_REPLACE_ME_STUB(0);*/
}

// ----------------------------------------------------
//  Do your Magic here:
//  Return the Free block to the system
//  May require Coalescing
//  Correct the heap Links (used,free) headers 
//  Update stats
//-----------------------------------------------------
void Mem::Free( void * const data )
{
	UsedNode* pDataHdr = (UsedNode*)data - 1;
	uint32_t DataSize = pDataHdr->mBlockSize;
	Heap* pHeap = this->poHeap;
	FreeNode* pToFree = (FreeNode*)pDataHdr;
	pToFree->mType = Block::Free;
	
	if (pDataHdr->pUsedNext == nullptr && pDataHdr->pUsedPrev != nullptr)
	{
		//Trace::out("if 1\n");
	}
	else if (pDataHdr->pUsedNext != nullptr && pDataHdr->pUsedPrev != nullptr)
	{
		//Trace::out("if 2\n");
	}
	else if (pDataHdr->pUsedNext == nullptr && pDataHdr->pUsedPrev == nullptr)
	{
		
		UsedNode* Start = pDataHdr;
		
		FreeNode* pFree = new(Start) FreeNode(pDataHdr->mBlockSize);
		InsertFree(pHeap->pFreeHead,pFree);
		
		FreeNode* toPass = pFree->pFreeNext;
		
		pHeap->pUsedHead = nullptr;
		pHeap->pNextFit = pFree;
		bool ifCoal = Coalescing(toPass);

		if (ifCoal == true) 
		{
			pHeap->currNumUsedBlocks--;
			pHeap->currUsedMem = (uint16_t)((uint32_t)pHeap->currUsedMem - DataSize);
			pHeap->currNumFreeBlocks;
			pHeap->currFreeMem = (uint16_t)(pHeap->currFreeMem + DataSize + sizeof(FreeNode));
		}
		else {
			pHeap->currNumUsedBlocks--;
			pHeap->currUsedMem = (uint16_t)(pHeap->currUsedMem - DataSize);
			pHeap->currNumFreeBlocks++;
			pHeap->currFreeMem = (uint16_t)(pHeap->currFreeMem + DataSize);
		}
	}
	else if (pDataHdr->pUsedNext != nullptr && pDataHdr->pUsedPrev == nullptr)//此為第一個且後面有東西
	{
		//Trace::out("if 4\n");
	}
	
	
	AZUL_REPLACE_ME(data);	
} 
 
void Mem::InsertUsed(UsedNode * &WholeList, UsedNode* toAdd) 
{
	if (WholeList == nullptr)
	{
		WholeList = toAdd;
	}
	else if (WholeList != nullptr)
	{
		
		WholeList->pUsedPrev = toAdd;
		toAdd->pUsedNext = WholeList;
		WholeList = toAdd;
	}
}

void Mem::InsertFree(FreeNode* &WholeList, FreeNode* toAdd)
{
	FreeNode* tmp = WholeList;

	if (WholeList != nullptr && WholeList->mType == Block::Used)
	{
		toAdd->pFreePrev = nullptr;
		if (tmp->pFreeNext != nullptr)
		{
			tmp = tmp->pFreeNext;
			toAdd->pFreeNext = tmp;
		}
		WholeList = toAdd;

	}else if (WholeList == nullptr)
	{
		WholeList = toAdd;
	}
	else if (WholeList != nullptr)
	{
		

		while (true) 
		{
			
			if (toAdd < tmp)
			{
				if (tmp->pFreePrev == nullptr) 
				{
					tmp->pFreePrev = toAdd;
					toAdd->pFreeNext = tmp;
					WholeList = toAdd;
				}
				else if (tmp->pFreePrev != nullptr && tmp->pFreeNext != nullptr)
				{
					toAdd->pFreeNext = tmp;
					toAdd->pFreePrev = tmp->pFreePrev;
					tmp->pFreePrev->pFreeNext = toAdd;
					tmp->pFreePrev = toAdd;

				}
				break;
			}
			if (tmp->pFreeNext == nullptr) 
			{
				tmp->pFreeNext = toAdd;
				toAdd->pFreePrev = tmp;
				break;
			}
			tmp = tmp->pFreeNext;
		}
	}
	if (toAdd->pFreeNext != nullptr) {
		FreeNode* toAddEnd = (FreeNode*)((uint32_t)(toAdd + 1) + toAdd->mBlockSize);
		toAddEnd->mAboveBlockFree = true;
	}
}

bool Mem::Coalescing(FreeNode* &pFreeIn) 
{
	Heap* pHeap = this->poHeap;
	//FreeNode* FreeTop = (FreeNode*)(pHeap + 1);
	//FreeNode* HeapEnd = (FreeNode*)this->pBottomAddr;
	if (pFreeIn!= nullptr && pFreeIn->mAboveBlockFree == true && pFreeIn->mType == Block::Free) 
	{

		pFreeIn = nullptr;
		FreeNode* pNewFree = new(this->pTopAddr) FreeNode((uint32_t)this->pBottomAddr - ((uint32_t)this->pTopAddr +sizeof(FreeNode)));
		pHeap->pFreeHead = pNewFree;
		return true;
	}
	return false;
	
}

// ---  End of File ---
