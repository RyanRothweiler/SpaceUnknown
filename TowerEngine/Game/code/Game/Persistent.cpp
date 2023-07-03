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

	// Add source -------------
	void AddSourceTyped(uint32 GUID, void* Data, persistent_pointer_type Type, state* State) {
		persistent_pointer Pointer = {};
		Pointer.GUID = GUID;
		Pointer.Data = Data;
		Pointer.Type = Type;

		hash::Add(&State->PersistentPointerSources, GUID, (void*)(&Pointer), sizeof(persistent_pointer), GlobalPermMem);
	}
	
	void AddSource(uint32 GUID, station* Data, state* State)
	{
		AddSourceTyped(GUID, (void*)Data, persistent_pointer_type::station, State);
	}

	void AddSource(uint32 GUID, ship* Data, state* State)
	{
		AddSourceTyped(GUID, (void*)Data, persistent_pointer_type::ship, State);
	}

	void AddSource(uint32 GUID, asteroid* Data, state* State)
	{
		AddSourceTyped(GUID, (void*)Data, persistent_pointer_type::asteroid, State);
	}

	void AddSource(uint32 GUID, salvage* Data, state* State)
	{
		AddSourceTyped(GUID, (void*)Data, persistent_pointer_type::salvage, State);
	}

	//--------------------------------

	// Get ----------------
	
	station* GetStation(persistent_pointer* Pointer, state* State)  
	{
		Resolve(Pointer, persistent_pointer_type::station, State);
		return (station*)Pointer->Data;
	}

	ship* GetShip(persistent_pointer* Pointer, state* State)  
	{
		Resolve(Pointer, persistent_pointer_type::ship, State);
		return (ship*)Pointer->Data;
	}

	asteroid* GetAsteroid(persistent_pointer* Pointer, state* State)  
	{
		Resolve(Pointer, persistent_pointer_type::asteroid, State);
		return (asteroid*)Pointer->Data;
	}

	salvage* GetSalvage(persistent_pointer* Pointer, state* State)  
	{
		Resolve(Pointer, persistent_pointer_type::salvage, State);
		return (salvage*)Pointer->Data;
	}

	//--------------------------------

	// Set -----------------
	 
	void SetStation(persistent_pointer* Pointer, station* Station) {
		Pointer->Type = persistent_pointer_type::station;
		Pointer->Data = (void*)Station;
		Pointer->GUID = Station->Persist->GUID;
	}

	void SetShip(persistent_pointer* Pointer, ship* Ship) {
		Pointer->Type = persistent_pointer_type::ship;
		Pointer->Data = (void*)Ship;
		Pointer->GUID = Ship->Persist->GUID;
	}

	void SetAsteroid(persistent_pointer* Pointer, asteroid* Asteroid) {
		Pointer->Type = persistent_pointer_type::asteroid;
		Pointer->Data = (void*)Asteroid;
		Pointer->GUID = Asteroid->Persist->GUID;
	}

	void SetSalvage(persistent_pointer* Pointer, salvage* Salvage) {
		Pointer->Type = persistent_pointer_type::salvage;
		Pointer->Data = (void*)Salvage;
		Pointer->GUID = Salvage->Persist->GUID;
	}
	//--------------------------------
};
