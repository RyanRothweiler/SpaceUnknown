skill_node* SkillTreeNodeFind(string ID, game::state* State)
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

skill_node* SkillTreeNodeCreate(game::state* State)
{
	skill_node* Node = &State->SkillNodes[State->SkillNodesCount++];
	Assert(ArrayCount(State->SkillNodes) > State->SkillNodesCount);

	Node->Position = vector2{2, 2};
	Node->ID = PlatformApi.GetGUID();

	return Node;
}

void SkillTreeNodeLoad(json::json_data* JsonIn, game::state* State)
{
	skill_node* NewNode = SkillTreeNodeCreate(State);
	NewNode->ID = 				json::GetString("id", JsonIn);
	NewNode->Position = 		json::GetVector2("position", JsonIn);

	// Add Children
	for (int i = 0; i < ArrayCount(NewNode->Children); i++) {
		string Key = "child_" + i;
		string ChildID = json::GetData(Key, JsonIn);
		if (StringLength(ChildID) > 0) {
			NewNode->AddChild(SkillTreeNodeFind(ChildID, State));
		}
	}
}

void SkillTreeNodeSave(skill_node* Node)
{
	json::json_data JsonOut = json::GetJson(GlobalTransMem);

	json::AddKeyPair("position", Node->Position, &JsonOut);
	json::AddKeyPair("id", Node->ID, &JsonOut);

	for (int i = 0; i < Node->ChildrenCount; i++) {
		string Key = "child_" + i;
		json::AddKeyPair(Key.Array(), Node->Children[i]->ID, &JsonOut);
	}

	json::SaveToFile(&JsonOut, "T:/Game/assets/SkillTreeNodes/" + Node->ID + ".skill_node");
}