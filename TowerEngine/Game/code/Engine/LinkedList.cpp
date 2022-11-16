#pragma once

#ifndef LinkedListCPP
#define LinkedListCPP

#include "LinkedList.h"

void
ListSaveToArray(list_head* Head, memory_arena* Memory)
{
	if (Head->ArrayValid) {
		// We really should free this before we make another array!!
		// Assert(0);
	}

	Head->ArrayValid = true;
	Head->Array = ArenaAllocate(Memory, Head->DataSize * Head->LinkCount);

	list_link* CurrLink = Head->TopLink;
	for (uint32 I = 0; I < Head->LinkCount; I++) {

		uint8* NextIndex = (uint8*)Head->Array;
		NextIndex += I * Head->DataSize;
		memcpy(NextIndex, CurrLink->Data, Head->DataSize);

		CurrLink = CurrLink->NextLink;
	}
}

void*
ListGetArrayIndex(list_head* Head, uint32 Index)
{
	Assert(Head->ArrayValid);
	Assert(Index < Head->LinkCount);

	uint8* Array = (uint8*)Head->Array;
	return (Array + (Head->DataSize * Index));
}

// NOTE this is zero indexed. So to get the first link you pass 0.
list_link*
GetLink(list_head *Head, uint32 LinkNum)
{
	Assert(LinkNum >= 0);
	Assert(LinkNum < Head->LinkCount);

	list_link *CurrentLink = Head->TopLink;

	for (int LinkDepth = LinkNum; LinkDepth >= 0; LinkDepth--) {
		if (LinkDepth == 0) {
			return (CurrentLink);
		} else {
			CurrentLink = CurrentLink->NextLink;
		}

	}

	//NOTE did not find the link, or there was some issue.
	Assert(0);
	return (GameNull);
}

void *
GetLinkData(list_head *Head, uint32 LinkNum)
{
	list_link* Link = GetLink(Head, LinkNum);
	Assert(Link != NULL);
	return (Link->Data);
}

void
InitList(list_head* ListHead, uint32 DataSize)
{
	ListHead->Initialized = true;
	ListHead->LinkCount = 0;
	ListHead->DataSize = DataSize;
	ListHead->ArrayValid = false;
	ListHead->TopLink = GameNull;
	ListHead->BottomLink = GameNull;
}

list_head *
CreateList(memory_arena *Memory, uint32 DataSize)
{
	list_head *ListHead = (list_head *)ArenaAllocate(Memory, sizeof(list_head));
	InitList(ListHead, DataSize);
	return (ListHead);
}

list_link *
AllocateLink(memory_arena *Memory, void* Data, uint32 DataSize)
{
	list_link *NewLink = (list_link *)ArenaAllocate(Memory, sizeof(list_link));
	NewLink->NextLink = GameNull;
	NewLink->Data = ArenaAllocate(Memory, DataSize);
	MemoryCopy((char*)NewLink->Data, (char*)Data, DataSize);
	return (NewLink);
}

void*
AddLink(list_head *Head, void* Data, memory_arena *Memory)
{
	Assert(Head != GameNull);

	list_link *NewLink = AllocateLink(Memory, Data, Head->DataSize);

	if (Head->LinkCount != 0) {
		Head->BottomLink->NextLink = NewLink;
	} else {
		Head->TopLink = NewLink;
	}

	Head->BottomLink = NewLink;
	Head->LinkCount++;
	Head->ArrayValid = false;

	return NewLink->Data;
}

// The new link will be inserted at the index, pushing everything else down.
void
InsertLink(list_head *Head, uint32 InsertionIndex, void* Data, memory_arena *Memory)
{
	Assert(InsertionIndex >= 0); //"Probably didn't create the list"
	Assert(InsertionIndex < Head->LinkCount); //"This checks that the place inserting is valid"

	if (InsertionIndex == Head->LinkCount - 1) {
		if (Head->LinkCount == 1) {

			list_link *NewLink = AllocateLink(Memory, Data, Head->DataSize);

			Head->LinkCount++;
			Head->ArrayValid = false;

			NewLink->NextLink = Head->TopLink;
			Head->TopLink = NewLink;
			return;
		} else {
			AddLink(Head, Data, Memory);
			return;
		}
	}

	list_link *NewLink = AllocateLink(Memory, Data, Head->DataSize);

	Head->LinkCount++;
	Head->ArrayValid = false;

	if (InsertionIndex == 0) {
		if (Head->LinkCount == 0) {
			Head->TopLink = NewLink;
			Head->BottomLink = NewLink;
			return;
		} else {
			NewLink->NextLink = Head->TopLink;
			Head->TopLink = NewLink;
			return;
		}
	}

	// NOTE this is a singlely linked list, so this step is more complicated than it needs to be.
	list_link *NextLink = GetLink(Head, InsertionIndex);
	list_link *PrevLink = GetLink(Head, InsertionIndex - 1);

	NewLink->NextLink = NextLink;
	PrevLink->NextLink = NewLink;
}

// NOTE this is used within linked list code.
void
PatchNextLinkOut(list_head *Head, list_link *CurrentLink)
{
	//NOTE MEM FREE HERE
	list_link *LinkRemoving = CurrentLink->NextLink;

	if (LinkRemoving->NextLink == GameNull) {
		CurrentLink->NextLink = GameNull;
		Head->BottomLink = CurrentLink;
	} else {
		CurrentLink->NextLink = CurrentLink->NextLink->NextLink;
	}

	Head->LinkCount--;
	Head->ArrayValid = false;
}

void
RemoveLink(list_head *Head, uint32 IndexRemoving)
{
	Assert(IndexRemoving >= 0);

	if (IndexRemoving == 0) {

		// NOTE MEM FREE HERE

		if (Head->LinkCount == 1) {
			Head->TopLink = GameNull;
			Head->BottomLink = GameNull;
		} else {
			Head->TopLink = Head->TopLink->NextLink;
		}

		Head->ArrayValid = false;
		Head->LinkCount--;

		return;
	}

	// Set to 1 because we want to always be one link behind, since we don't have a prev pointer.
	uint32 CurrentLinkIndex = 1;
	list_link *CurrentLink = Head->TopLink;

	while (CurrentLink->NextLink) {
		if (IndexRemoving == CurrentLinkIndex) {
			PatchNextLinkOut(Head, CurrentLink);
			return;
		}

		CurrentLinkIndex++;
		CurrentLink = CurrentLink->NextLink;
	}

	// This is an error. The link should be removed before this happens.
	Assert(0);
}


//NOTE this doesn't check for equivalency, only if the pointers are the same
void
RemoveLinkData(list_head *Head, void *DataRemoving)
{
	if (Head->TopLink->Data == DataRemoving) {
		RemoveLink(Head, 0);
	}

	list_link *CurrentLink = Head->TopLink;
	while (CurrentLink->NextLink) {
		if (CurrentLink->NextLink->Data == DataRemoving) {
			PatchNextLinkOut(Head, CurrentLink);
			return;
		}

		CurrentLink = CurrentLink->NextLink;
	}
}

#endif

// #if UNIT_TESTING
void
LinkedListUnitTests(memory_arena *Memory)
{
	// creating a list
	list_head *TestList = CreateList(Memory, sizeof(vector2));
	Assert(TestList);
	Assert(TestList->LinkCount == 0);

	list_link *NewLink = {};
	vector2 NewVector = {};

	// adding to end of list
	// -------------------------------------------------------------------------
	NewVector = vector2{1, 2};
	AddLink(TestList, (void*)&NewVector, Memory);
	Assert(TestList->LinkCount == 1);

	NewVector = vector2{3, 4};
	AddLink(TestList, (void*)&NewVector, Memory);
	Assert(TestList->LinkCount == 2);

	NewLink = GetLink(TestList, 0);
	vector2* VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 1);
	Assert(VectorPointer->Y == 2);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	// insert to head
	// -------------------------------------------------------------------------
	NewVector = vector2{0, 0};
	InsertLink(TestList, 0, (void*)&NewVector, Memory);
	Assert(TestList->LinkCount == 3);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 0);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 1);
	Assert(VectorPointer->Y == 2);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	// insert to middle
	// -------------------------------------------------------------------------
	NewVector = vector2{10, 11};
	InsertLink(TestList, 1, (void*)&NewVector, Memory);
	Assert(TestList->LinkCount == 4);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 0);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 10);
	Assert(VectorPointer->Y == 11);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 1);
	Assert(VectorPointer->Y == 2);

	NewLink = GetLink(TestList, 3);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	// inserting to end
	// -------------------------------------------------------------------------
	NewVector = vector2{20, 25};
	InsertLink(TestList, 3, (void*)&NewVector, Memory);
	Assert(TestList->LinkCount == 5);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 0);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 10);
	Assert(VectorPointer->Y == 11);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 1);
	Assert(VectorPointer->Y == 2);

	NewLink = GetLink(TestList, 3);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	NewLink = GetLink(TestList, 4);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	// remove link with link middle
	// -------------------------------------------------------------------------
	RemoveLink(TestList, 2);
	Assert(TestList->LinkCount == 4);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 0);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 10);
	Assert(VectorPointer->Y == 11);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	NewLink = GetLink(TestList, 3);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	// // remove link with link head
	// -------------------------------------------------------------------------
	RemoveLink(TestList, 0);
	Assert(TestList->LinkCount == 3);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 10);
	Assert(VectorPointer->Y == 11);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	// remove link with link tail
	// -------------------------------------------------------------------------
	RemoveLink(TestList, 2);
	Assert(TestList->LinkCount == 2);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 10);
	Assert(VectorPointer->Y == 11);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	NewVector = vector2{20, 25};
	AddLink(TestList, (void*)&NewVector, Memory);

	NewVector = vector2{30, 45};
	AddLink(TestList, (void*)&NewVector, Memory);

	NewVector = vector2{300, 450};
	AddLink(TestList, (void*)&NewVector, Memory);

	NewVector = vector2{0, 15};
	AddLink(TestList, (void*)&NewVector, Memory);

	NewVector = vector2{31, 47};
	AddLink(TestList, (void*)&NewVector, Memory);

	//removing link with data head
	// -------------------------------------------------------------------------
	void *HeadData = GetLinkData(TestList, 0);
	RemoveLinkData(TestList, HeadData);
	Assert(TestList->LinkCount == 6);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 30);
	Assert(VectorPointer->Y == 45);

	NewLink = GetLink(TestList, 3);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 300);
	Assert(VectorPointer->Y == 450);

	NewLink = GetLink(TestList, 4);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 15);

	NewLink = GetLink(TestList, 5);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 31);
	Assert(VectorPointer->Y == 47);

	//removing link with data middle
	// -------------------------------------------------------------------------
	void *MiddleData = GetLinkData(TestList, 2);
	RemoveLinkData(TestList, MiddleData);
	Assert(TestList->LinkCount == 5);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 300);
	Assert(VectorPointer->Y == 450);

	NewLink = GetLink(TestList, 3);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 15);

	NewLink = GetLink(TestList, 4);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 31);
	Assert(VectorPointer->Y == 47);

	//removing link with data end
	// -------------------------------------------------------------------------
	void *TailData = GetLinkData(TestList, 4);
	RemoveLinkData(TestList, TailData);
	Assert(TestList->LinkCount == 4);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 3);
	Assert(VectorPointer->Y == 4);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 300);
	Assert(VectorPointer->Y == 450);

	NewLink = GetLink(TestList, 3);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 15);

	//removing link with index head
	// -------------------------------------------------------------------------
	RemoveLink(TestList, 0);
	Assert(TestList->LinkCount == 3);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 300);
	Assert(VectorPointer->Y == 450);

	NewLink = GetLink(TestList, 2);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 15);

	//removing link with index middle
	// -------------------------------------------------------------------------
	RemoveLink(TestList, 1);
	Assert(TestList->LinkCount == 2);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	NewLink = GetLink(TestList, 1);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 0);
	Assert(VectorPointer->Y == 15);

	//removing link with index tail
	RemoveLink(TestList, 1);
	Assert(TestList->LinkCount == 1);

	NewLink = GetLink(TestList, 0);
	VectorPointer = (vector2 *)NewLink->Data;
	Assert(VectorPointer->X == 20);
	Assert(VectorPointer->Y == 25);

	// Testing SaveAsArary
	// -------------------------------------------------------------------------
	/*
	{
		list_head *ListTwo = CreateList(Memory, sizeof(uint32));

		uint32 One = 1;
		uint32 Two = 2;
		uint32 Three = 3;

		AddLink(ListTwo, (void*)&One, Memory);
		AddLink(ListTwo, (void*)&Two, Memory);
		AddLink(ListTwo, (void*)&Three, Memory);

		// ListSaveToArray(ListTwo, Memory);

		uint32* IndexOne = (uint32*)ListGetArrayIndex(ListTwo, 0);
		uint32* IndexTwo = (uint32*)ListGetArrayIndex(ListTwo, 1);
		uint32* IndexThree = (uint32*)ListGetArrayIndex(ListTwo, 2);

		Assert(*IndexOne == One);
		Assert(*IndexTwo == Two);
		Assert(*IndexThree == Three);
	}
	*/

	// Long lists
	{
		list_head *ListOne = CreateList(Memory, sizeof(uint32));
		for (uint32 index = 0; index < 100; index++) {
			AddLink(ListOne, (void*)&index, Memory);
		}

		uint32* Index0 = (uint32*)GetLinkData(ListOne, 0);
		uint32* Index10 = (uint32*)GetLinkData(ListOne, 10);
		uint32* Index87 = (uint32*)GetLinkData(ListOne, 87);

		Assert(*Index0 == 0);
		Assert(*Index10 == 10);
		Assert(*Index87 == 87);

		RemoveLink(ListOne, 1);
		RemoveLink(ListOne, 89);
		RemoveLink(ListOne, ListOne->LinkCount - 1);

		uint32 Num = 10;

		RemoveLink(ListOne, 0);
		InsertLink(ListOne, ListOne->LinkCount - 1, (void*)&Num, Memory);
		ListSaveToArray(ListOne, Memory);

		RemoveLink(ListOne, 0);
		InsertLink(ListOne, ListOne->LinkCount - 2, (void*)&Num, Memory);
		ListSaveToArray(ListOne, Memory);

		RemoveLink(ListOne, 0);
		InsertLink(ListOne, ListOne->LinkCount - 3, (void*)&Num, Memory);
		ListSaveToArray(ListOne, Memory);

		RemoveLink(ListOne, 0);
		InsertLink(ListOne, ListOne->LinkCount - 4, (void*)&Num, Memory);
		ListSaveToArray(ListOne, Memory);

	}
}
// #endif
