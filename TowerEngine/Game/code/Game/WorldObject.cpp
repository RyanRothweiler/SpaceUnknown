void WorldObjectRegister(state* State, world_object* WorldObj)
{
	State->WorldObjects[State->WorldObjectsCount++] = WorldObj;
	Assert(ArrayCount(State->WorldObjects) > State->WorldObjectsCount);
}

void WorldObjectUnregister(state* State, world_object* WorldObj)
{
	bool32 Found = false;
	for (int i = 0; i < State->WorldObjectsCount; i++) {
		if (!Found) {
			world_object* O = State->WorldObjects[i];
			if (O == WorldObj) {
				Found = true;
			}
		}

		if (Found) {
			State->WorldObjects[i] = State->WorldObjects[i + 1];
		}
	}

	State->WorldObjectsCount--;
}