struct skill_bonuses {
	real64 FuelForceAddition;
};

struct skill_node {
	string ID;
	vector2 Position;
	bool32 Unlocked;

	int64 KnowledgeCost;

	real64 CircleRadius = 2;

	skill_node* Children[10];
	int32 ChildrenCount;

	void AddChild(skill_node* Child)
	{
		Children[ChildrenCount++] = Child;
		Assert(ChildrenCount < ArrayCount(Children));
	}
};