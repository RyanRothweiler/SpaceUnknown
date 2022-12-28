enum class item_id {
	venigen,
};

struct item_definition {
	item_id ID;
	string DisplayName;
	bool32 Stackable;
	int64 Mass;
};

struct item_instance {
	item_definition Definition;
	int32 Count;
};