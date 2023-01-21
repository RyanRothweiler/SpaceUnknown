void InitAsteroid(asteroid* Asteroid)
{
	Asteroid->Using = true;
	Asteroid->Position = {};
	Asteroid->Size = RandomRangeFloat(5.0f, 10.0f);

	int64 SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.AsteroidImages) - 1);
	Asteroid->Image = Globals->AssetsList.AsteroidImages[SpriteIndex];

	real64 Rate = PI / 10.0f;
	Asteroid->RotationRate = RandomRangeFloat(-Rate, Rate);
	Asteroid->Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);
}

void SpawnAsteroid(asteroid_cluster* Cluster)
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
				real64 Dist = Vector2Distance(PossiblePos, Cluster->Asteroids[i].Position);
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
					InitAsteroid(&Cluster->Asteroids[i]);
					Cluster->Asteroids[i].Position = PossiblePos;
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

	SpawnAsteroid(Self);
	SleepStepper(State, &Self->Spawner, SecondsToMilliseconds(5));
}

void AsteroidCreateCluster(vector2 Center, real64 Radius, game::state* State)
{
	Assert(ArrayCount(State->Asteroids) > State->ClustersCount);
	asteroid_cluster* Cluster = &State->Asteroids[State->ClustersCount++];

	Cluster->Center = Center;
	Cluster->Radius = Radius;
	Cluster->SpawnTimer = 0;

	RegisterStepper(&Cluster->Spawner, &AsteroidSpawnStep, (void*)Cluster, State);
}