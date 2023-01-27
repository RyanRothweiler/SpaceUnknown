skill_node* SkillTreeNodeCreate(fixed_allocator::memory* Memory)
{
	skill_node* Node = (skill_node*)fixed_allocator::Alloc(Memory);

	Node->Position = vector2{2, 2};
	Node->ID = PlatformApi.GetGUID();

	return Node;
}

void SkillTreeNodeSave(skill_node* Node)
{
	json::json_data JsonOut = json::GetJson(GlobalTransMem);

	json::AddKeyPair("position_", Node->Position, &JsonOut);

	json::SaveToFile(&JsonOut, "T:/Game/assets/SkillTreeNodes/" + Node->ID + ".txt");
}