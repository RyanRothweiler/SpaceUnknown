skill_node* SkillTreeNodeFind(string ID, state* State)
{
	for (int i = 0; i < State->SkillNodesCount; i++) {
		if (State->SkillNodes[i].ID == ID) {
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

	Node->Position = vector2{2, 2};
	Node->ID = PlatformApi.GetGUID();

	return Node;
}

void SkillTreeNodeLoad(json::json_data* JsonIn, state* State)
{
	skill_node* NewNode = SkillTreeNodeCreate(State);
	NewNode->ID = 				json::GetString("id", JsonIn);
	NewNode->Position = 		json::GetVector2("position", JsonIn);
	NewNode->KnowledgeCost = 	json::GetInt64("knowledge_cost", JsonIn);

	NewNode->BonusAdditions.FuelForceAddition = 		(float)json::GetReal64("bonus_fuel_force_addition", JsonIn);

	// NOTE this doesn't update the children list. Only saves the IDs
	for (int i = 0; i < ArrayCount(NewNode->Children); i++) {
		string Key = "child_" + i;
		string ChildID = json::GetData(Key, JsonIn);
		if (StringLength(ChildID) > 0) {
			NewNode->SavedChildrenIDs[i] = ChildID;
		}
	}
}

void SkillTreeNodeSave(skill_node* Node)
{
	json::json_data JsonOut = json::GetJson(GlobalTransMem);

	json::AddKeyPair("id", 					Node->ID, 				&JsonOut);
	json::AddKeyPair("position", 			Node->Position, 		&JsonOut);
	json::AddKeyPair("knowledge_cost", 		Node->KnowledgeCost, 	&JsonOut);

	json::AddKeyPair("bonus_fuel_force_addition", 			Node->BonusAdditions.FuelForceAddition, 		&JsonOut);

	for (int i = 0; i < Node->ChildrenCount; i++) {
		string Key = "child_" + i;
		json::AddKeyPair(Key.Array(), Node->Children[i]->ID, &JsonOut);
	}

	json::SaveToFile(&JsonOut, "T:/Game/assets/SkillTreeNodes/" + Node->ID + ".skill_node");
}

void SkillTreeSaveAll(state* State)
{
	struct locals {
		void Save(skill_node* Node)
		{
			SkillTreeNodeSave(Node);
			for (int i = 0; i < Node->ChildrenCount; i++) {
				Save(Node->Children[i]);
			}
		}
	} Locals;

	for (int i = 0; i < State->SkillNodesCount; i++) {
		Locals.Save(&State->SkillNodes[i]);
	}
}

void SkillTreeUnlock(skill_node* Node, state* State)
{
	State->Knowledge -= Node->KnowledgeCost;
	Node->Unlocked = true;
	SkillTreeSaveAll(State);

	State->TreeBonusesTotal = State->TreeBonusesTotal + Node->BonusAdditions;
}

void SkillTreeImguiDisplayBonuses(skill_bonuses Bonuses)
{
	if (Bonuses.FuelForceAddition > 0) {
		ImGui::Text("Additional force per fuel unit %i%%", (int)((Bonuses.FuelForceAddition) * 100.0f));
	}
}