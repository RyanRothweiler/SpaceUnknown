void ModuleUpdateSalvager(void* SelfData, real64 Time, state* State)
{
	ship_module* Module = (ship_module*)SelfData;
	ship* Owner = per::GetShip(&Module->Persist->Owner, State);

	WorldTargetClear(&Module->Persist->Target);

	bool32 Skip = false;

	// Can only work when the ship is idle
	if (Owner->Persist->Status != ship_status::idle) { Skip = true; }

	if (Skip) {
		Module->Persist->ActivationTimerMS = 0.0f;
		return;
	}

	for (int i = 0; i < State->PersistentData.SalvagesCount && !WorldTargetHasTarget(&Module->Persist->Target); i++) {
		salvage* Sal = &State->Salvages[i];

		real64 Dist = Vector2Distance(Sal->Persist->WorldObject.Position, Owner->Persist->Position);
		if (Dist < Module->Definition.ActivationRange) {
			WorldTargetSet(&Module->Persist->Target, Sal);
		}
	}

	if (WorldTargetHasTarget(&Module->Persist->Target)) {
		Module->Persist->ActivationTimerMS += Time;
		if (Module->Persist->ActivationTimerMS >= Module->Definition.ActivationTimeMS) {
			Module->Persist->ActivationTimerMS = 0.0f;

			// Do module thing

			salvage* Sal = WorldTargetGetSalvage(&Module->Persist->Target, State);
			int Amount = SubtractAvailable(&Sal->Persist->KnowledgeAmount, 2);
			State->PersistentData.Knowledge += Amount;
			if (Sal->Persist->KnowledgeAmount <= 0) {
				SalvageSpawn(Sal);
			}
		}
	} else {
		Module->Persist->ActivationTimerMS = 0.0f;
	}
}
