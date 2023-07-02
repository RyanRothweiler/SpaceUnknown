struct asteroid {
	asteroid_persistent* Persist;
};

struct asteroid_cluster {
	asteroid_cluster_persistent* Persist;

	asteroid Asteroids[32];
	stepper Spawner;
};
