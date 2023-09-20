void AsteroidDestroy(asteroid* Asteroid, state* State)
{
	Asteroid->Persist->Using = false;
	WorldObjectUnregister(State, &Asteroid->Persist->WorldObject);
}

void AsteroidHovering(selectable* Sel, engine_state* EngineState, game_input* Input)
{
	asteroid* Asteroid = Sel->GetAsteroid();

	ImGui::SetNextWindowPos(ImVec2((float)Input->MousePos.X + 20, (float)Input->MousePos.Y));
	bool Open = true;
	ImGui::Begin("Info", &Open, SelectionHoverFlags);

	ImGui::Text("Asteroid");
	ImGui::Text("%i x %s", Asteroid->Persist->OreCount, item_id_NAME[(int)Asteroid->Persist->OreItem]);

	ImGui::End();
}

void InitAsteroid(asteroid* Asteroid, item_id OreItem, state* State)
{
	Asteroid->Persist->Using = true;
	Asteroid->Persist->OreCount = 10;
	Asteroid->Persist->OreItem = OreItem;

	color Col = {};
	switch (OreItem) {
		case item_id::venigen: { Col = Color255(79.0f, 60.0f, 48.0f, 1.0f); break; }
		case item_id::pyrexium: { Col = Color255(135.0f, 58.0f, 45.0f, 1.0f); break; }

		INVALID_DEFAULT;
	}

	Asteroid->Persist->WorldObject.Color.R = Col.R;
	Asteroid->Persist->WorldObject.Color.G = Col.G;
	Asteroid->Persist->WorldObject.Color.B = Col.B;
	Asteroid->Persist->WorldObject.Color.A = Col.A;

	Asteroid->Persist->WorldObject.Position = {};

	real64 S = RandomRangeFloat(5.0f, 10.0f);
	Asteroid->Persist->WorldObject.Size = vector2{S, S};

	Asteroid->Persist->SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.AsteroidImages));
	Asteroid->Persist->WorldObject.Image = Globals->AssetsList.AsteroidImages[Asteroid->Persist->SpriteIndex];

	real64 Rate = PI / 10.0f;
	Asteroid->Persist->WorldObject.RotationRate = RandomRangeFloat(-Rate, Rate);
	Asteroid->Persist->WorldObject.Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);

	WorldObjectRegister(State, &Asteroid->Persist->WorldObject);
	selectable* Sel = RegisterSelectable(selection_type::asteroid, &Asteroid->Persist->WorldObject.Position, &Asteroid->Persist->WorldObject.Size, (void*)Asteroid, State);
	Sel->OnHover = &AsteroidHovering;

	per::AddSource(Asteroid->Persist->GUID, Asteroid, State);

	// NOTE if more is added here consider if it needs to be added to the loading from persist method also
}

void SpawnAsteroid(asteroid_cluster* Cluster, state* State)
{
	real64 CollisionRadius = 15.0f;

	int Attempts = 10;
	while (Attempts > 0) {
		Attempts--;

		real64 r = Cluster->Persist->Radius * sqrt(RandomRangeFloat(0.0f, 1.0f));
		real64 theta = RandomRangeFloat(0.0f, 1.0f) * 2.0f * PI;

		vector2 PossiblePos = {};
		PossiblePos.X = Cluster->Persist->Center.X + r * cos(theta);
		PossiblePos.Y = Cluster->Persist->Center.Y + r * sin(theta);

		bool32 Valid = true;
		for (int i = 0; i < ArrayCount(Cluster->Asteroids); i++) {
			if (Cluster->Asteroids[i].Persist->Using) {
				real64 Dist = Vector2Distance(PossiblePos, Cluster->Asteroids[i].Persist->WorldObject.Position);
				if (Dist < CollisionRadius) {
					Valid = false;
					break;
				}
			}
		}

		// Spawn asteroid
		if (Valid) {
			for (int i = 0; i < ArrayCount(Cluster->Asteroids); i++) {
				if (!Cluster->Asteroids[i].Persist->Using) {
					InitAsteroid(&Cluster->Asteroids[i], Cluster->Persist->OreItem, State);
					Cluster->Asteroids[i].Persist->WorldObject.Position = PossiblePos;
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

// Setup data for existing asteroid cluster
void AsteroidClusterSetup(asteroid_cluster* Cluster, asteroid_cluster_persistent* Persist, state* State) {
	Cluster->Persist = Persist;
	RegisterStepper(&Cluster->Spawner, &AsteroidSpawnStep, (void*)Cluster, State);

	Assert(ArrayCount(Cluster->Asteroids) == ArrayCount(Persist->Asteroids));
	for (int i = 0; i < ArrayCount(Cluster->Asteroids); i++) {
		asteroid* Asteroid = &Cluster->Asteroids[i];

		// Set persist
		Asteroid->Persist = &Persist->Asteroids[i];
		
		// Setup images
		Asteroid->Persist->WorldObject.Image = Globals->AssetsList.AsteroidImages[Asteroid->Persist->SpriteIndex];

		// Register data
		WorldObjectRegister(State, &Asteroid->Persist->WorldObject);
		selectable* Sel = RegisterSelectable(selection_type::asteroid, &Asteroid->Persist->WorldObject.Position, &Asteroid->Persist->WorldObject.Size, (void*)Asteroid, State);
		Sel->OnHover = &AsteroidHovering;
		
		per::AddSource(Asteroid->Persist->GUID, Asteroid, State);
	}
}

// Create a whole new asteroid cluster
void AsteroidClusterCreate(vector2 Center, real64 Radius, item_id OreItem, state* State)
{
	asteroid_cluster* Cluster = &State->AsteroidClusters[State->PersistentData.AsteroidClustersCount];
	AsteroidClusterSetup(
			Cluster, 
			&State->PersistentData.AsteroidClusters[State->PersistentData.AsteroidClustersCount],
			State
	);

	State->PersistentData.AsteroidClustersCount++;
	Assert(ArrayCount(State->PersistentData.AsteroidClusters) > State->PersistentData.AsteroidClustersCount);
	Assert(ArrayCount(State->AsteroidClusters) > State->PersistentData.AsteroidClustersCount);

	Cluster->Persist->OreItem = OreItem;
	Cluster->Persist->Center = Center;
	Cluster->Persist->Radius = Radius;
}
