void InitAsteroid(asteroid* Asteroid, game::state* State)
{
	Asteroid->Using = true;

	Asteroid->WorldObject.Color = Color255(79.0f, 60.0f, 48.0f, 1.0f);
	Asteroid->WorldObject.Position = {};
	Asteroid->WorldObject.Size = RandomRangeFloat(5.0f, 10.0f);

	int64 SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.AsteroidImages));
	Asteroid->WorldObject.Image = Globals->AssetsList.AsteroidImages[SpriteIndex];

	real64 Rate = PI / 10.0f;
	Asteroid->WorldObject.RotationRate = RandomRangeFloat(-Rate, Rate);
	Asteroid->WorldObject.Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);

	WorldObjectRegister(State, &Asteroid->WorldObject);
}

void SpawnAsteroid(asteroid_cluster* Cluster, game::state* State)
{
	real64 CollisionRadius = 15.0f;

	int Attempts = 10;
	while (Attempts > 0) {
		Attempts--;

		real64 r = Cluster->Radius * sqrt(RandomRangeFloat(0.0f, 1.0f));
		real64 theta = RandomRangeFloat(0.0f, 1.0f) * 2.0f * PI;

		vector2 PossiblePos = {};
		PossiblePos.X = Cluster->Center.X + r * cos(theta);
		PossiblePos.Y = Cluster->Center.Y + r * sin(theta);

		bool32 Valid = true;
		for (int i = 0; i < ArrayCount(Cluster->Asteroids); i++) {
			if (Cluster->Asteroids[i].Using) {
				real64 Dist = Vector2Distance(PossiblePos, Cluster->Asteroids[i].WorldObject.Position);
				if (Dist < CollisionRadius) {
					Valid = false;
					break;
				}
			}
		}

		// Spawn asteroid
		if (Valid) {
			for (int i = 0; i < ArrayCount(Cluster->Asteroids); i++) {
				if (!Cluster->Asteroids[i].Using) {
					InitAsteroid(&Cluster->Asteroids[i], State);
					Cluster->Asteroids[i].WorldObject.Position = PossiblePos;
					return;
				}
			}
		}
	}

	// No empty space found
}

void AsteroidSpawnStep(void* SelfData, real64 Time, game::state* State)
{
	game::asteroid_cluster* Self = (game::asteroid_cluster*)SelfData;

	SpawnAsteroid(Self, State);
	SleepStepper(State, &Self->Spawner, SecondsToMilliseconds(5));
}

void AsteroidCreateCluster(vector2 Center, real64 Radius, game::state* State)
{
	Assert(ArrayCount(State->Asteroids) > State->ClustersCount);
	asteroid_cluster* Cluster = &State->Asteroids[State->ClustersCount++];

	Cluster->Center = Center;
	Cluster->Radius = Radius;

	RegisterStepper(&Cluster->Spawner, &AsteroidSpawnStep, (void*)Cluster, State);
}