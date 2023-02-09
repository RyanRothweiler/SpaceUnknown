void WorldObjectRegister(game::state* State, world_object* WorldObj)
{
	State->WorldObjects[State->WorldObjectsCount++] = WorldObj;
	Assert(ArrayCount(State->WorldObjects) > State->WorldObjectsCount);
}