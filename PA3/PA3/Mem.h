//-----------------------------------------------------------------------------
// Copyright 2021, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

#ifndef MEM_H
#define MEM_H

#include "Heap.h"

class Mem
{
public:
	enum class MemHeaderGuard
	{
		Type_A,
		Type_5
	};

public:

	Mem( MemHeaderGuard type );	

	Mem() = delete;//constructor
	Mem(const Mem &) = delete;//copy constructor
	Mem & operator = (const Mem &) = delete;//assignment operator
	~Mem();

	Heap *getHeap();
	void Print(int count);

	// implement these functions
	void Free( void * const data );
	void *Malloc( const uint32_t size );
	void Initialize( );

	//helper func
	void InsertUsed(UsedNode* &WholeList, UsedNode* toAdd);
	void InsertFree(FreeNode* &WholeList, FreeNode* toAdd);
	bool Coalescing(FreeNode* &pFreeIn);



private:
	void *poRawMem;//老師的不用管
	Heap *poHeap;
	void *pTopAddr;//given
	void *pBottomAddr;//given
	MemHeaderGuard type;
};

#endif 

// ---  End of File ---

