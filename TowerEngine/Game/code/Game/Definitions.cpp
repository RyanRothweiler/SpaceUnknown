void CreateDefinitions()
{
	Assert(ArrayCount(Globals->AssetsList.ShipModuleDefinitions) > (int)ship_module_id::count);

	// Ship Module type icons
	Assert(ArrayCount(Globals->AssetsList.ShipModuleTypeIcons) > (int)ship_module_slot_type::count);
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::industrial] = assets::GetImage("Icon_ShipModuleType_Industrial");
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::structural] = assets::GetImage("Icon_ShipModuleType_Structural");
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::science] = assets::GetImage("Icon_ShipModuleType_Science");

	// Ship Module icons
	Assert(ArrayCount(Globals->AssetsList.ShipModuleIcons) > (int)ship_module_id::count);
	Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::asteroid_miner] = assets::GetImage("Icon_ShipModule_AsteroidMiner");
	Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::salvager_i] = assets::GetImage("Icon_ShipModule_Salvager");

	// Ship modules
	{
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ID = ship_module_id::asteroid_miner;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ItemID = item_id::sm_asteroid_miner;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].DisplayName = "Asteroid Miner";
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationTimeMS = SecondsToMilliseconds(60.0f * 2.0f);
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationRange = 40.0f;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].SlotType = ship_module_slot_type::industrial;

		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ID = ship_module_id::salvager_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ItemID = item_id::sm_salvager_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].DisplayName = "Salvager MK1";
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ActivationTimeMS = SecondsToMilliseconds(60.0f * 2.0f);
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ActivationRange = 10.0f;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].SlotType = ship_module_slot_type::science;
	}

	// Ships
	{
		Globals->AssetsList.Definition_Ship_First.FuelRateMassPerSecond = 0.5f;
		Globals->AssetsList.Definition_Ship_First.Mass = 200;
		Globals->AssetsList.Definition_Ship_First.FuelTankMassLimit = 300;
		Globals->AssetsList.Definition_Ship_First.SlotsCount = 4;
		Globals->AssetsList.Definition_Ship_First.SlotTypes[0] = ship_module_slot_type::industrial;
		Globals->AssetsList.Definition_Ship_First.SlotTypes[1] = ship_module_slot_type::industrial;
		Globals->AssetsList.Definition_Ship_First.SlotTypes[2] = ship_module_slot_type::structural;
		Globals->AssetsList.Definition_Ship_First.SlotTypes[3] = ship_module_slot_type::science;
	}

	// Items
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

		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].ID = item_id::sm_salvager_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].DisplayName = "Salvager MK1";
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].ShipModuleID = ship_module_id::salvager_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].Icon = Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::salvager_i];

		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].ID = item_id::stl;
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].DisplayName = "STL Fuel";
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].Mass = 0.25f;
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].Icon = assets::GetImage("Icon_STL");
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].Stackable = true;
	}

	// Recipes
	{
		Assert(ArrayCount(Globals->AssetsList.RecipeDefinitions) > (int)recipe_id::count);

		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].RegisterInput(item_id::venigen, 5.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].RegisterOutput(item_id::stl, 1.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].DurationMS = MinutesToMilliseconds(60);
	}
}