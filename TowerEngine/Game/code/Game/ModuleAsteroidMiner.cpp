
void ModuleUpdateAsteroidMiner(void* SelfData, real64 Time, state* State)
{
	ship_module* Module = (ship_module*)SelfData;
	ship* Owner = per::GetShip(&Module->Persist->Owner, State);

	WorldTargetClear(&Module->Persist->Target);

	bool32 Skip = false;

	// If no cargo space then do nothing
	if (Owner->Hold.MassCurrent == Owner->Hold.GetMassLimit()) { Skip = true; }

	// Can only work when the ship is idle
	if (Owner->Persist->Status != ship_status::idle) { Skip = true; }

	if (Skip) {
		Module->Persist->ActivationTimerMS = 0.0f;
		return;
	}

	for (int i = 0; i < State->PersistentData.AsteroidClustersCount && !WorldTargetHasTarget(&Module->Persist->Target); i++) {
		asteroid_cluster* Cluster = &State->AsteroidClusters[i];
		for (int a = 0; a < ArrayCount(Cluster->Asteroids) && !WorldTargetHasTarget(&Module->Persist->Target); a++) {
			asteroid* Roid = &Cluster->Asteroids[a];
			if (Roid->Persist->Using) {
				real64 Dist = Vector2Distance(Roid->Persist->WorldObject.Position, Owner->Persist->Position);
				if (Dist < Module->Definition.ActivationRange) {
					WorldTargetSet(&Module->Persist->Target, Roid);
				}
			}
		}
	}

	if (WorldTargetHasTarget(&Module->Persist->Target)) {
		Module->Persist->ActivationTimerMS += Time;

		r64 ActivationTime = ModuleGetActivationTime(&Module->Definition);
		if (Module->Persist->ActivationTimerMS >= ActivationTime) {
			Module->Persist->ActivationTimerMS = 0.0f;

			// Do module thing

			asteroid* Roid = WorldTargetGetAsteroid(&Module->Persist->Target, State);

			int Amount = SubtractAvailable(&Roid->Persist->OreCount, 2);
			ItemGive(&Owner->Hold, Roid->Persist->OreItem, Amount);

			if (Roid->Persist->OreCount <= 0) {
				AsteroidDestroy(Roid, State);
			}

		}
	} else {
		Module->Persist->ActivationTimerMS = 0.0f;
	}
}

