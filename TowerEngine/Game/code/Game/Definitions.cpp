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


	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].DisplayName = "Asteroid Miner";
	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationTimeMS = SecondsToMilliseconds(60.0f * 2.0f);
	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationRange = 40.0f;
	Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].SlotType = ship_module_slot_type::industrial;

	Globals->AssetsList.Definition_Ship_First.FuelRateGallonsPerSecond = 1.0f;
	Globals->AssetsList.Definition_Ship_First.Mass = 200;
	Globals->AssetsList.Definition_Ship_First.FuelTankGallons = 300;
	Globals->AssetsList.Definition_Ship_First.SlotsCount = 3;
	Globals->AssetsList.Definition_Ship_First.SlotTypes[0] = ship_module_slot_type::industrial;
	Globals->AssetsList.Definition_Ship_First.SlotTypes[1] = ship_module_slot_type::industrial;
	Globals->AssetsList.Definition_Ship_First.SlotTypes[2] = ship_module_slot_type::structural;

	Globals->AssetsList.ItemDefinitions[0].ID = item_id::venigen;
	Globals->AssetsList.ItemDefinitions[0].DisplayName = "Venigen";
	Globals->AssetsList.ItemDefinitions[0].Mass = 1;
	Globals->AssetsList.ItemDefinitions[0].Icon = assets::GetImage("Icon_Venigen");
}

item_definition GetItemDefinition(item_id ID)
{
	for (int i = 0; i < ArrayCount(Globals->AssetsList.ItemDefinitions); i++) {
		if (Globals->AssetsList.ItemDefinitions[i].ID == ID) {
			return Globals->AssetsList.ItemDefinitions[i];
		}
	}

	Assert(false); // Couldn't find that item ID
	return {};
}