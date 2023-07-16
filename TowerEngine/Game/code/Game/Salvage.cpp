void SalvageHovering(selectable* Sel, engine_state* EngineState, game_input* Input)
{
	salvage* Salvage = Sel->GetSalvage();

	ImGui::SetNextWindowPos(ImVec2((float)Input->MousePos.X + 20, (float)Input->MousePos.Y));
	bool Open = true;
	ImGui::Begin("Info", &Open, SelectionHoverFlags);

	ImGui::Text("Salvage");
	ImGui::Text("%i x Knowledge", Salvage->Persist->KnowledgeAmount);

	ImGui::End();
}

int32 SalvageOrigAmount = 20;

// Setup data for existing salvage
void SalvageSetup(salvage* Salvage, salvage_persistent* Persist, state* State) {
	Salvage->Persist = Persist;

	Salvage->Persist->WorldObject.Image = Globals->AssetsList.SalvageImages[Salvage->Persist->SpriteIndex];

	WorldObjectRegister(State, &Salvage->Persist->WorldObject);

	selectable* Sel = RegisterSelectable(selection_type::salvage, &Salvage->Persist->WorldObject.Position, &Salvage->Persist->WorldObject.Size, (void*)Salvage, State);
	Sel->OnHover = &SalvageHovering;

	per::AddSource(Salvage->Persist->GUID, Salvage, State);
}

// Create new salvage
void SalvageCreate(vector2 Position, state* State)
{
	salvage* Salvage = &State->Salvages[State->PersistentData.SalvagesCount];
	Salvage->Persist = &State->PersistentData.Salvages[State->PersistentData.SalvagesCount];

	State->PersistentData.SalvagesCount++;
	Assert(ArrayCount(State->PersistentData.Salvages) > State->PersistentData.SalvagesCount);
	Assert(ArrayCount(State->Salvages) > State->PersistentData.SalvagesCount);

	Salvage->Persist->KnowledgeAmount = SalvageOrigAmount;

	Salvage->Persist->WorldObject.Color.R = 197.0f / 255.0f;
	Salvage->Persist->WorldObject.Color.G = 214.0f / 255.0f;
	Salvage->Persist->WorldObject.Color.B = 208.0f / 255.0f;
	Salvage->Persist->WorldObject.Color.A = 1.0f;

	Salvage->Persist->WorldObject.Position = Position;
	Salvage->Persist->WorldObject.Size = vector2{8, 8};

	Salvage->Persist->SpawnRadius = 10.0f;
	Salvage->Persist->SpawnCenter = {};

	Salvage->Persist->SpriteIndex = RandomRangeInt(0, ArrayCount(Globals->AssetsList.SalvageImages));

	real64 Rate = PI / 10.0f;
	Salvage->Persist->WorldObject.RotationRate = RandomRangeFloat(-Rate, Rate);
	Salvage->Persist->WorldObject.Rotation = RandomRangeFloat(-PI / 2.0f, PI / 2.0f);

	SalvageSetup(Salvage, Salvage->Persist, State);
}

void SalvageSpawn(salvage* Salvage)
{
	Salvage->Persist->KnowledgeAmount = SalvageOrigAmount;
	Salvage->Persist->WorldObject.Position = Salvage->Persist->SpawnCenter + RandomRadius(Salvage->Persist->SpawnRadius);
}
