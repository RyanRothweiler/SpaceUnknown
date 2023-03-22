void SalvageHovering(selectable* Sel, engine_state* EngineState, game_input* Input)
{
	salvage* Salvage = Sel->GetSalvage();

	ImGui::SetNextWindowPos(ImVec2((float)Input->MousePos.X + 20, (float)Input->MousePos.Y));
	bool Open = true;
	ImGui::Begin("Info", &Open, SelectionHoverFlags);

	ImGui::Text("Salvage");
	ImGui::Text("%i x Knowledge", Salvage->KnowledgeAmount);

	ImGui::End();
}

int32 SalvageOrigAmount = 20;

void SalvageCreate(state* State, vector2 Position)
{
	salvage* Salvage = &State->Salvages[State->SalvagesCount++];
	Assert(State->SalvagesCount < ArrayCount(State->Salvages));

	Salvage->KnowledgeAmount = SalvageOrigAmount;

	Salvage->WorldObject.Color = Color255(197.0f, 214.0f, 208.0f, 1.0f);
	Salvage->WorldObject.Position = Position;
	Salvage->WorldObject.Size = vector2{8, 8};

	Salvage->SpawnRadius = 10.0f;
	Salvage->SpawnCenter = {};

	int64 SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.SalvageImages));
	Salvage->WorldObject.Image = Globals->AssetsList.SalvageImages[SpriteIndex];

	real64 Rate = PI / 10.0f;
	Salvage->WorldObject.RotationRate = RandomRangeFloat(-Rate, Rate);
	Salvage->WorldObject.Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);

	WorldObjectRegister(State, &Salvage->WorldObject);

	selectable* Sel = RegisterSelectable(selection_type::salvage, &Salvage->WorldObject.Position, &Salvage->WorldObject.Size, (void*)Salvage, State);
	Sel->OnHover = &SalvageHovering;
}

void SalvageSpawn(salvage* Salvage)
{
	Salvage->KnowledgeAmount = SalvageOrigAmount;
	Salvage->WorldObject.Position = Salvage->SpawnCenter + RandomRadius(Salvage->SpawnRadius);
}