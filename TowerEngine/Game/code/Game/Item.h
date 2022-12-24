enum class item_id {
	venigen,
};

struct item_definition {
	item_id ID;
	string DisplayName;
	bool32 Stackable;
	real64 Weight;
};

struct item_instance {
	item_definition Definition;
	int32 Count;
};