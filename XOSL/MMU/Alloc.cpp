/*
 * Extended Operating System Loader (XOSL)
 * Copyright (c) 1999 by Geurt Vos
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 *
 * Open Watcom Migration
 * Copyright (c) 2010 by Mario Looijkens:
 * - Change code in function static void wait() to get rid of 
 *   Error! E121: syntax error.
 * - In function void *operator new (unsigned int Size) the instruction
 *   OldDesc->Next->Prev = MemDesc is causing problems during debugging.
 *      When the New operator gets called for the first time after AllocInit
 *   OldDesc->Next will contain a NULL pointer. 
 *   Not a good idea to set NULL->next !!!
 * - Add function void *operator new [] (unsigned int Size)
 *   Copied from Watcom cpplib, file opnewarr.cpp.
 * - Add function void operator delete [] (void *ptr)  
 *   Copied from Watcom cpplib, file opdelarr.cpp
 *
 */

/*
 * Basic 'First Fit' allocation unit
 */

#include <newdefs.h>
#include <mem.h>
#include <freemem.h>

static void wait()
{
       //asm xor       ah,ah  //Error! E121: syntax error
       //asm int       0x16   //Error! E121: syntax error
  _asm{
     xor ah,ah
     int 0x16
  };
}
extern void printf(const char *fmt, ...);


typedef struct SMemDesc {
	struct SMemDesc *Next;
	struct SMemDesc *Prev;
	long PageCount;
	unsigned long MagicNum; // 0xabcd0123
} TMemDesc, *PMemDesc;

static PMemDesc FreeList;

void AllocInit(void)
{
	PMemDesc NextItem;
	unsigned long MemStart;

	MemStart = FreeMemStart();  // Returns the start of unused memory
	FreeList = (PMemDesc)MemStart;
	NextItem = (PMemDesc)(0x00010000 + MemStart);
	FreeList->Prev = NULL;
	FreeList->Next = NextItem;
	FreeList->PageCount = 0;
	NextItem->Prev = FreeList;
	NextItem->Next = NULL;
	NextItem->PageCount = (0x00098000 - PhysAddr(MemStart) >> 4) - 1;
}

void *operator new (unsigned int Size)
{
	long PageCount;
	PMemDesc MemDesc, OldDesc;

	PageCount = 1 + (Size >> 4);
	if (Size & 0x0f)
		++PageCount;

	for (MemDesc = FreeList->Next; MemDesc && MemDesc->PageCount < PageCount; MemDesc = MemDesc->Next);
	if (!MemDesc) {
		printf("malloc(): out of memory\n");
		return NULL;
	}

	OldDesc = MemDesc;
	if (MemDesc->PageCount == PageCount) {
		// remove entire free memory block
		MemDesc->Prev->Next = MemDesc->Next;
		if (MemDesc->Next)
			MemDesc->Next->Prev = MemDesc->Prev;
	}
	else {
		// shrink free memory block
		MemDesc = (PMemDesc)((PageCount << 16) + (unsigned long)MemDesc);
		MemDesc->Next = OldDesc->Next;
		MemDesc->Prev = OldDesc->Prev;
		OldDesc->Prev->Next = MemDesc;
		//OldDesc->Next->Prev = MemDesc;  //ML This is causing problems during debugging
		//ML: When the New operator gets called for the first time after AllocInit
		//    OldDesc->Next will contain a NULL pointer. 
		//    Not a good idea to set NULL->next !!!
		if (OldDesc->Next)
			OldDesc->Next->Prev = MemDesc;

		MemDesc->PageCount = OldDesc->PageCount - PageCount;
		OldDesc->PageCount = PageCount;
	}
	OldDesc->MagicNum = 0xabcd0123;
	return (void *)(0x00010000 + (unsigned long)OldDesc);
}


void *operator new [] (unsigned int Size)  //ML - Copied from Watcom cpplib, file opnewarr.cpp
{
       return ::operator new( Size );
}


void operator delete (void *ptr)
{
	PMemDesc Prev, Next, New;
	unsigned long MergeNext;

	if (!ptr) {
		printf("free(): NULL pointer\n");
		wait();
		return;
	}
	for (Next = FreeList->Next; (unsigned long)Next < (unsigned long)ptr; Next = Next->Next);

	Prev = Next->Prev;
	New = (PMemDesc)( ((unsigned long)ptr) - 0x00010000 );

	if (New->MagicNum != 0xabcd0123) {
		printf("free(): invalid pointer\n");
		wait();
		return;
	}

	MergeNext = (Prev->PageCount << 16) + (unsigned long)Prev;
	if (MergeNext == (unsigned long)New) {
		Prev->PageCount += New->PageCount;
		New = Prev;
	}
	else {
		Prev->Next = New;
		New->Prev = Prev;
	}
	MergeNext = (New->PageCount << 16) + (unsigned long)New;
	if (MergeNext == (unsigned long)Next) {
		New->PageCount += Next->PageCount;
		New->Next = Next->Next;
		if (Next->Next)
			Next->Next->Prev = New;
	}
	else {
		New->Next = Next;
		Next->Prev = New;
	}
}


void operator delete [] (void *ptr)  //ML - Copied from Watcom cpplib, file opdelarr.cpp
{
       ::delete ( (char*) ptr);
}


unsigned long CoreLeft()
{
	PMemDesc Entry;
	unsigned long Core = 0;

	for (Entry = FreeList->Next; Entry; Entry = Entry->Next)
		Core += Entry->PageCount;
	return Core << 4;
}
