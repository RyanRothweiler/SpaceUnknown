struct skill_node {
	string ID;
	vector2 Position;
	bool32 Unlocked;

	skill_node* Children[10];
	int32 ChildrenCount;

	void AddChild(skill_node* Child)
	{
		Children[ChildrenCount++] = Child;
		Assert(ChildrenCount < ArrayCount(Children));
	}
};