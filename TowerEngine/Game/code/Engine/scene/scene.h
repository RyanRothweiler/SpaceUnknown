struct scene {
	entity* Entities[Thousand(2)];

	void Init()
	{
		for (int x = 0; x < ArrayCount(Entities); x++) {
			Entities[x] = {};
		}
	}
};