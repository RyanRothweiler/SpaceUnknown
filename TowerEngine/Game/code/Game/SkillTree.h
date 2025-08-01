
skill_bonuses SkillBonusesAdd(skill_bonuses A, skill_bonuses B)
{
	skill_bonuses Ret = {};
	Ret.FuelForce = A.FuelForce + B.FuelForce;
	Ret.ShipLimit = A.ShipLimit + B.ShipLimit;
	Ret.CargoSize = A.CargoSize + B.CargoSize;
	Ret.IndustrialActivationTimeMinutes = A.IndustrialActivationTimeMinutes + B.IndustrialActivationTimeMinutes;
	Ret.ShipRadarRangeAddition = A.ShipRadarRangeAddition + B.ShipRadarRangeAddition;
	Ret.FuelTankSize = A.FuelTankSize + B.FuelTankSize;

	for (int i = 0; i < ArrayCount(A.RecipeUnlocked); i++) {
		Ret.RecipeUnlocked[i] = A.RecipeUnlocked[i] || B.RecipeUnlocked[i];
	}

	return Ret;
}

MetaStruct struct skill_node_persistent {
	int64 ID;

	int64 KnowledgeCost;
	vector2 Position;
	skill_bonuses BonusAdditions;

	skill_node_icon Icon;
	int64 ChildrenIDs[10];
};

struct skill_node {
	skill_node_persistent Persist;

	bool32 Unlocked;

	real64 CircleRadius = 2;

	skill_node* Children[10];
	int32 ChildrenCount;

	skill_node* Parent[10];
	int32 ParentsCount;

	static_assert(ArrayCount(Children) == ArrayCount(Persist.ChildrenIDs), "Persistent and children array lengths must match.");

	void AddChild(skill_node* Child)
	{

		Children[ChildrenCount] = Child;
		Persist.ChildrenIDs[ChildrenCount] = Child->Persist.ID;
		ChildrenCount++;
		Assert(ChildrenCount < ArrayCount(Children));

		Child->Parent[Child->ParentsCount] = this;	
		Child->ParentsCount++;
		Assert(Child->ParentsCount < ArrayCount(Child->Parent));
	}

	// This assumes only one recipe is being unlocked
	recipe_id RecipeUnlocking() { 
		for (int i = 0; i < ArrayCount(Persist.BonusAdditions.RecipeUnlocked); i++) {
			if (Persist.BonusAdditions.RecipeUnlocked[i]) { 
				return (recipe_id)i;
			}
		}

		return recipe_id::none;
	}

	bool32 ParentUnlocked() {
		for (int i = 0; i < ParentsCount; i++) {
			if (Parent[i]->Unlocked) {
				return true;
			}
		}

		return false;
	}
};
