const float SpawnSeconds = 5.0f;

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
					Cluster->Asteroids[i].Using = true;
					Cluster->Asteroids[i].Position = PossiblePos;
					Cluster->Asteroids[i].Size = RandomRangeFloat(5.0f, 10.0f);

					real64 Rate = PI / 10.0f;
					Cluster->Asteroids[i].RotationRate = RandomRangeFloat(-Rate, Rate);
					Cluster->Asteroids[i].Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);

					return;
				}
			}

			// No empty space found
			return;
		}
	}
}

void AsteroidSpawnStep(void* SelfData, real64 Time)
{
	game::asteroid_cluster* Self = (game::asteroid_cluster*)SelfData;
	Self->SpawnTimer -= Time;
	if (Self->SpawnTimer < 0) {
		Self->SpawnTimer = SecondsToMilliseconds(SpawnSeconds);
		SpawnAsteroid(Self);
	}
}

void AsteroidCreateCluster(vector2 Center, real64 Radius, game::state* State)
{
	Assert(ArrayCount(State->Asteroids) > State->ClustersCount);
	asteroid_cluster* Cluster = &State->Asteroids[State->ClustersCount++];

	Cluster->Center = Center;
	Cluster->Radius = Radius;
	Cluster->SpawnTimer = SecondsToMilliseconds(SpawnSeconds);

	RegisterStepper(&Cluster->Spawner, &AsteroidSpawnStep, (void*)Cluster, State);
}