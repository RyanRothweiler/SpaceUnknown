struct global_id {
	ui32 ID;
	void* Data;
};

global_id GlobalIDGenerate(void* Data) {
	global_id ID = {};
	ID.ID = PlatformApi.GetGUID();
	ID.Data = Data;
	return ID;
}
