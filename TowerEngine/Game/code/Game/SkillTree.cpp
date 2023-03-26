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


	/*
	// NOTE this doesn't update the children list. Only saves the IDs
	for (int i = 0; i < ArrayCount(NewNode->Children); i++) {
		string Key = "child_" + i;
		string ChildID = json::GetData(Key, JsonIn);
		if (StringLength(ChildID) > 0) {
			NewNode->SavedChildrenIDs[i] = ChildID;
		}
	}
	*/
}

void SkillTreeNodeSave(skill_node * Node)
{
	string Path = "T:/Game/assets/SkillTreeNodes/" + string{Node->Persist.ID} + ".skill_node";
	save_data::Write(
	    Path.Array(),
	    &skill_node_persistent_META[0],
	    ArrayCount(skill_node_persistent_META),
	    (void*)&Node->Persist
	);

	/*
	for (int i = 0; i < Node->ChildrenCount; i++) {
		string Key = "child_" + i;
		json::AddKeyPair(Key.Array(), Node->Children[i]->ID, &JsonOut);
	}

	json::SaveToFile(&JsonOut, );
	*/
}

void SkillTreeSaveAll(state * State)
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

void SkillTreeUnlock(skill_node * Node, state * State)
{
	State->Knowledge -= Node->Persist.KnowledgeCost;
	Node->Unlocked = true;
	SkillTreeSaveAll(State);

	State->TreeBonusesTotal = SkillBonusesAdd(State->TreeBonusesTotal, Node->Persist.BonusAdditions);
}

void SkillTreeImguiDisplayBonuses(skill_bonuses Bonuses)
{
	if (Bonuses.FuelForceAddition > 0) {
		ImGui::Text("Additional force per fuel unit %i%%", (int)((Bonuses.FuelForceAddition) * 100.0f));
	}
}