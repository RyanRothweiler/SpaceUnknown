skill_node* SkillTreeNodeFind(string ID, state* State)
{
	for (int i = 0; i < State->SkillNodesCount; i++) {
		if (State->SkillNodes[i].Persist.ID == ID) {
			return &State->SkillNodes[i];
		}
	}

	// That id doesn't exist
	Assert(0);
	return {};
}

skill_node* SkillTreeNodeCreate(state* State)
{
	skill_node* Node = &State->SkillNodes[State->SkillNodesCount++];
	Assert(ArrayCount(State->SkillNodes) > State->SkillNodesCount);

	Node->Persist.Position = vector2{2, 2};

	string ID = PlatformApi.GetGUID();
	Node->Persist.ID = StringHash(ID);

	return Node;
}

void SkillTreeNodeLoad(char* FilePath, state* State)
{
	skill_node* NewNode = SkillTreeNodeCreate(State);
	save_data::Read(
	    FilePath,
	    (void*)&NewNode->Persist,
	    &skill_node_persistent_META[0],
	    ArrayCount(skill_node_persistent_META),
	    GlobalTransMem
	);
}

// SaveData write clears the memory so we can use the same root for all nodes
void SkillTreeNodeSave(skill_node * Node, save_data::member* Root)
{
	string Path = "T:/Game/assets/SkillTreeNodes/" + string{Node->Persist.ID} + ".skill_node";
	save_data::Write(
	    Path.Array(),
	    &skill_node_persistent_META[0],
	    ArrayCount(skill_node_persistent_META),
	    (void*)&Node->Persist,
	    Root
	);
}

void SkillTreeSaveAll(state * State)
{
	save_data::member* Root = (save_data::member*)ArenaAllocate(GlobalTransMem, sizeof(save_data::member));
	for (int i = 0; i < State->SkillNodesCount; i++) {
		SkillTreeNodeSave(&State->SkillNodes[i], Root);
	}
}

void SkillTreeUnlock(skill_node * Node, state * State)
{
	State->PersistentData.Knowledge -= Node->Persist.KnowledgeCost;
	Node->Unlocked = true;

	State->PersistentData.TreeBonuses = SkillBonusesAdd(State->PersistentData.TreeBonuses, Node->Persist.BonusAdditions);
	GlobalTriggerSave = true;
}

void SkillTreeImguiDisplayBonuses(skill_bonuses Bonuses)
{
	ImGui::Separator();

	if (Bonuses.FuelForce > 0) {
		ImGui::Text("+%i%% force per fuel unit", (int)((Bonuses.FuelForce) * 100.0f));
	}

	if (Bonuses.ShipLimit > 0) {
		ImGui::Text("+%i ship limit", (int)(Bonuses.ShipLimit));
	}

	if (Bonuses.CargoSize > 0) {
		ImGui::Text("+%i(t) ship cargo hold mass limit", Bonuses.CargoSize);
	}

	for (int i = 0; i < ArrayCount(Bonuses.RecipeUnlocked); i++) {
		if (Bonuses.RecipeUnlocked[i]) { 
			string Disp = "Unlock recipe " + recipe_id_NAME[i];
			ImGui::Text(Disp.Array());
		}
	}
}

bool32 SkillNodeCanUnlock(skill_node* Node) { 

	// Can't unlock twice
	if (Node->Unlocked) { 
		return false; 
	}

	return Node->ParentUnlocked();
}
