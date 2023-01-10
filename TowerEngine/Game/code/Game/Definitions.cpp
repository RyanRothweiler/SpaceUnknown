void CreateDefinitions()
{
	Assert(ArrayCount(Globals->AssetsList.ShipModuleDefinitions) > (int)ship_module_id::count);

	// Ship Module type icons
	Assert(ArrayCount(Globals->AssetsList.ShipModuleTypeIcons) > (int)ship_module_slot_type::count);
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::industrial] = assets::GetImage("Icon_ShipModuleType_Industrial");
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::structural] = assets::GetImage("Icon_ShipModuleType_Structural");

	// Ship Module icons
	Assert(ArrayCount(Globals->AssetsList.ShipModuleIcons) > (int)ship_module_id::count);
	Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::asteroid_miner] = assets::GetImage("Icon_ShipModule_AsteroidMiner");

	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ID = ship_module_id::asteroid_miner;
	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].DisplayName = "Asteroid Miner";
	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationTimeMS = SecondsToMilliseconds(60.0f * 2.0f);
	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationRange = 40.0f;
	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].SlotType = ship_module_slot_type::industrial;

	// Ship modules definitions
	{
		Globals->AssetsList.Definition_Ship_First.FuelRateGallonsPerSecond = 1.0f;
		Globals->AssetsList.Definition_Ship_First.Mass = 200;
		Globals->AssetsList.Definition_Ship_First.FuelTankGallons = 300;
		Globals->AssetsList.Definition_Ship_First.SlotsCount = 3;
		Globals->AssetsList.Definition_Ship_First.SlotTypes[0] = ship_module_slot_type::industrial;
		Globals->AssetsList.Definition_Ship_First.SlotTypes[1] = ship_module_slot_type::industrial;
		Globals->AssetsList.Definition_Ship_First.SlotTypes[2] = ship_module_slot_type::structural;
	}

	// Item definitions
	{
		Assert(ArrayCount(Globals->AssetsList.ItemDefinitions) > (int)item_id::count);

		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].ID = item_id::venigen;
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].DisplayName = "Venigen";
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].Icon = assets::GetImage("Icon_Venigen");
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].Stackable = true;

		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].ID = item_id::sm_asteroid_miner;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].DisplayName = "Asteroid Miner";
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].ShipModuleID = ship_module_id::asteroid_miner;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].Icon = Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::asteroid_miner];
	}
}