struct asteroid {
	bool32 Using;
	vector2 Position;
	real64 Rotation;
	real64 Size;
	int64 Sprite;
	real64 RotationRate;
};

struct asteroid_cluster {
	asteroid Asteroids[25];
	vector2 Center;
	real64 Radius;
	stepper Spawner;

	real64 SpawnTimer;
};
