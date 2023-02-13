struct asteroid {
	item_id OreItem;
	int32 OreCount;

	bool32 Using;
	world_object WorldObject;
};

struct asteroid_cluster {
	asteroid Asteroids[25];
	item_id OreItem;
	vector2 Center;
	real64 Radius;
	stepper Spawner;
};