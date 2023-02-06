void SpawnSalvage(game::state* State, vector2 Position)
{
	salvage* Salvage = &State->Salvages[State->SalvagesCount++];
	Assert(State->SalvagesCount < ArrayCount(State->Salvages));

	Salvage->Position = Position;

	int64 SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.SalvageImages) - 1);
	Salvage->Image = Globals->AssetsList.SalvageImages[SpriteIndex];
}