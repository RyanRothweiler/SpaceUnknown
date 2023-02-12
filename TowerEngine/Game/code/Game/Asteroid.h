struct asteroid {
	bool32 Using;
	int32 OreCount;
	world_object WorldObject;
};

struct asteroid_cluster {
	asteroid Asteroids[25];
	vector2 Center;
	real64 Radius;
	stepper Spawner;
};