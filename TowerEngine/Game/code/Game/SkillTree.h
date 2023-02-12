
/*
	When adding new fields
	- Add Field
	- Add to saving / loading serialization
	- Add to operators in skill_bonuses struct
	- Add to displays when hovering
*/
struct skill_bonuses {
	float FuelForceAddition;

	skill_bonuses operator+(skill_bonuses Input)
	{
		skill_bonuses Ret = {};
		Ret.FuelForceAddition = FuelForceAddition + Input.FuelForceAddition;
		return Ret;
	}
};

struct skill_node {
	string ID;
	vector2 Position;

	bool32 Unlocked;

	int64 KnowledgeCost;

	real64 CircleRadius = 2;

	skill_bonuses BonusAdditions;

	// used only for loading the children ids
	string SavedChildrenIDs[10];

	skill_node* Children[10];
	int32 ChildrenCount;
	skill_node* Parent;

	void AddChild(skill_node* Child)
	{
		Children[ChildrenCount++] = Child;
		Assert(ChildrenCount < ArrayCount(Children));

		Child->Parent = this;
	}
};