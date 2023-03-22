void AsteroidDestroy(asteroid* Asteroid, state* State)
{
	Asteroid->Using = false;
	WorldObjectUnregister(State, &Asteroid->WorldObject);
}

void AsteroidHovering(selectable* Sel, engine_state* EngineState, game_input* Input)
{
	asteroid* Asteroid = Sel->GetAsteroid();

	ImGui::SetNextWindowPos(ImVec2((float)Input->MousePos.X + 20, (float)Input->MousePos.Y));
	bool Open = true;
	ImGui::Begin("Info", &Open, SelectionHoverFlags);

	ImGui::Text("Asteroid");
	ImGui::Text("%i x %s", Asteroid->OreCount, item_id_NAME[(int)Asteroid->OreItem]);

	ImGui::End();
}

void InitAsteroid(asteroid* Asteroid, item_id OreItem, state* State)
{
	Asteroid->Using = true;
	Asteroid->OreCount = 10;
	Asteroid->OreItem = OreItem;

	color Col = {};
	switch (OreItem) {
		case item_id::venigen: { Col = Color255(79.0f, 60.0f, 48.0f, 1.0f); break; }
		case item_id::pyrexium: { Col = Color255(135.0f, 58.0f, 45.0f, 1.0f); break; }

			INVALID_DEFAULT;
	}

	Asteroid->WorldObject.Color = Col;
	Asteroid->WorldObject.Position = {};

	real64 S = RandomRangeFloat(5.0f, 10.0f);
	Asteroid->WorldObject.Size = vector2{S, S};

	int64 SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.AsteroidImages));
	Asteroid->WorldObject.Image = Globals->AssetsList.AsteroidImages[SpriteIndex];

	real64 Rate = PI / 10.0f;
	Asteroid->WorldObject.RotationRate = RandomRangeFloat(-Rate, Rate);
	Asteroid->WorldObject.Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);

	WorldObjectRegister(State, &Asteroid->WorldObject);
	selectable* Sel = RegisterSelectable(selection_type::asteroid, &Asteroid->WorldObject.Position, &Asteroid->WorldObject.Size, (void*)Asteroid, State);
	Sel->OnHover = &AsteroidHovering;
}

void SpawnAsteroid(asteroid_cluster* Cluster, state* State)
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
					InitAsteroid(&Cluster->Asteroids[i], Cluster->OreItem, State);
					Cluster->Asteroids[i].WorldObject.Position = PossiblePos;
					return;
				}
			}
		}
	}

	// No empty space found
}

void AsteroidSpawnStep(void* SelfData, real64 Time, state* State)
{
	asteroid_cluster* Self = (asteroid_cluster*)SelfData;

	SpawnAsteroid(Self, State);
	SleepStepper(State, &Self->Spawner, SecondsToMilliseconds(5));
}

void AsteroidCreateCluster(vector2 Center, real64 Radius, item_id OreItem, state* State)
{
	Assert(ArrayCount(State->Asteroids) > State->ClustersCount);
	asteroid_cluster* Cluster = &State->Asteroids[State->ClustersCount++];

	Cluster->OreItem = OreItem;
	Cluster->Center = Center;
	Cluster->Radius = Radius;

	RegisterStepper(&Cluster->Spawner, &AsteroidSpawnStep, (void*)Cluster, State);
}