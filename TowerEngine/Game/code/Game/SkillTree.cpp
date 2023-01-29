skill_node* SkillTreeNodeCreate(fixed_allocator::memory* Memory)
{
	skill_node* Node = (skill_node*)fixed_allocator::Alloc(Memory);

	Node->Position = vector2{2, 2};
	Node->ID = PlatformApi.GetGUID();

	return Node;
}

void SkillTreeNodeLoad(json::json_data* JsonIn, skill_node* Parent, game::state* State)
{
	skill_node* NewNode = SkillTreeNodeCreate(&State->SkillNodesMemory);
	NewNode->ID = 				json::GetString("id", JsonIn);
	NewNode->Position = 		json::GetVector2("position", JsonIn);
	Parent->AddChild(NewNode);
}

void SkillTreeNodeSave(skill_node* Node)
{
	json::json_data JsonOut = json::GetJson(GlobalTransMem);

	json::AddKeyPair("position", Node->Position, &JsonOut);
	json::AddKeyPair("id", Node->ID, &JsonOut);

	json::SaveToFile(&JsonOut, "T:/Game/assets/SkillTreeNodes/" + Node->ID + ".skill_node");
}