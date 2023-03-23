
/*
	When adding new fields
	- Add Field
	- Add to operators in SkillBonusesAdd
	- Add to displays when hovering
*/

MetaStruct struct skill_bonuses {
	real32 FuelForceAddition;
};

skill_bonuses SkillBonusesAdd(skill_bonuses A, skill_bonuses B)
{
	skill_bonuses Ret = {};
	Ret.FuelForceAddition = A.FuelForceAddition + B.FuelForceAddition;
	return Ret;
}

MetaStruct struct skill_node_persistent {
	int64 ID;
	skill_bonuses Bonuses;
	int64 KnowledgeCost;
	vector2 Position;
	skill_bonuses BonusAdditions;

	int64 ChildrenIDs[10];
};

struct skill_node {
	skill_node_persistent Persist;

	bool32 Unlocked;

	real64 CircleRadius = 2;

	skill_node* Children[10];
	int32 ChildrenCount;
	skill_node* Parent;

	void AddChild(skill_node* Child)
	{
		Assert(ArrayCount(Children) == ArrayCount(Persist.ChildrenIDs));

		Children[ChildrenCount] = Child;
		Persist.ChildrenIDs[ChildrenCount] = Child->Persist.ID;
		ChildrenCount++;
		Assert(ChildrenCount < ArrayCount(Children));

		Child->Parent = this;
	}
};

MetaStruct struct skill_node_player {
	int64 ID;
};