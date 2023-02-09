void SalvageCreate(game::state* State, vector2 Position)
{
	salvage* Salvage = &State->Salvages[State->SalvagesCount++];
	Assert(State->SalvagesCount < ArrayCount(State->Salvages));

	Salvage->WorldObject.Color = Color255(197.0f, 214.0f, 208.0f, 1.0f);
	Salvage->WorldObject.Position = Position;
	Salvage->WorldObject.Size = 8.0f;

	int64 SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.SalvageImages));
	Salvage->WorldObject.Image = Globals->AssetsList.SalvageImages[SpriteIndex];

	real64 Rate = PI / 10.0f;
	Salvage->WorldObject.RotationRate = RandomRangeFloat(-Rate, Rate);
	Salvage->WorldObject.Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);

	WorldObjectRegister(State, &Salvage->WorldObject);
}