#pragma once
#ifndef LinkedListH
#define LinkedListH

struct list_link {
	void* Data;
	list_link* NextLink;
};

struct list_head {
	bool32 Initialized;

	list_link* BottomLink;
	list_link* TopLink;
	uint32 LinkCount;
	uint32 DataSize;

	void* Array;
	bool32 ArrayValid;
};

#endif