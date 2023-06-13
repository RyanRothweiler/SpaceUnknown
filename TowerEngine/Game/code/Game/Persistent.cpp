namespace per {

	void Resolve(persistent_pointer* Pointer, persistent_pointer_type Type, state* State)  
	{
		Assert(Pointer->Type == Type);

		// Get from hash table
		if (Pointer->Data == GameNull) {
			persistent_pointer* FromTable = hash::GetPersistentPointer(&State->PersistentPointerSources, Pointer->GUID);
			Assert(FromTable != GameNull);
			Pointer->Data = FromTable->Data;
		}
	}

	void Clear(persistent_pointer* Pointer) {
		Pointer->Type = persistent_pointer_type::none;
		Pointer->Data = GameNull;
	}

	// Station -----------------------
	void AddSource(uint32 GUID, station* Data, state* State)
	{ 
		persistent_pointer Pointer = {};
		Pointer.GUID = GUID;
		Pointer.Data = Data;
		Pointer.Type = persistent_pointer_type::station;

		hash::Add(&State->PersistentPointerSources, GUID, (void*)(&Pointer), sizeof(persistent_pointer), GlobalPermMem);
	}

	station* Get(persistent_pointer* Pointer, state* State)  
	{
		Resolve(Pointer, persistent_pointer_type::station, State);
		return (station*)Pointer->Data;
	}

	void Set(persistent_pointer* Pointer, station* Station) {
		Pointer->Type = persistent_pointer_type::station;
		Pointer->Data = (void*)Station;
		Pointer->GUID = Station->Persist->GUID;
	}

	//--------------------------------

};
