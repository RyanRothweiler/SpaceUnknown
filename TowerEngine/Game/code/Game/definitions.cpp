void CreateDefinitions()
{
	static_assert(ArrayCount(Globals->AssetsList.ShipModuleDefinitions) > gen_ship_module_id_count, "Not enough ship module definitions.");

	// Ship Module type icons
	Assert(ArrayCount(Globals->AssetsList.ShipModuleTypeIcons) > (int)ship_module_slot_type::count);
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::industrial] = assets::GetImage("Icon_ShipModuleType_Industrial");
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::structural] = assets::GetImage("Icon_ShipModuleType_Structural");
	Globals->AssetsList.ShipModuleTypeIcons[(int)ship_module_slot_type::science] = assets::GetImage("Icon_ShipModuleType_Science");

	// Ship Module icons
	{
		static_assert(ArrayCount(Globals->AssetsList.ShipModuleIcons) > gen_ship_module_id_count, "Not enough ship module icons");

		Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::asteroid_miner] = assets::GetImage("Icon_ShipModule_AsteroidMiner");
		Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::salvager_i] = assets::GetImage("Icon_ShipModule_Salvager");
		Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::cargo_expansion_i] = assets::GetImage("Icon_ShipModule_Cargo_Expansion_I");
		Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::foreman_i] = assets::GetImage("Icon_ShipModule_Formean_I");

		// Verify we have setup all the icons
		for (int i = 0; i < gen_ship_module_id_count; i++) {
			if (i == (int)ship_module_id::none) { continue; }
			Assert(Globals->AssetsList.ShipModuleIcons[i] != GameNull);
		}
	}

	// Skill node icons
	{
		static_assert(ArrayCount(Globals->AssetsList.SkillNodeIcons) > gen_skill_node_icon_count, "Skill node image array not big enough");

		Globals->AssetsList.SkillNodeIcons[(int)skill_node_icon::ship_limit] = assets::GetImage("NodeIcon_ShipLimit");
		Globals->AssetsList.SkillNodeIcons[(int)skill_node_icon::fuel_force] = assets::GetImage("NodeIcon_FuelForce");
		Globals->AssetsList.SkillNodeIcons[(int)skill_node_icon::cargo_size] = assets::GetImage("NodeIcon_CargoSize");
		Globals->AssetsList.SkillNodeIcons[(int)skill_node_icon::industrial_activation_time] = assets::GetImage("NodeIcon_IndustrialActivationTime");
	}

	// Ship modules
	{
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ID = ship_module_id::asteroid_miner;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ItemID = item_id::sm_asteroid_miner;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].DisplayName = "Asteroid Miner";
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationTimeMS = (i64)MinutesToMilliseconds(30);
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationRange = 30.0f;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationRangeDisplayColor = COLOR_TEAL;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].SlotType = ship_module_slot_type::industrial;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::asteroid_miner].ActivationStepMethod = &ModuleUpdateAsteroidMiner;

		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ID = ship_module_id::salvager_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ItemID = item_id::sm_salvager_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].DisplayName = "Salvager MK1";
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ActivationTimeMS = (i64)MinutesToMilliseconds(60);
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ActivationRange = 10.0f;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].SlotType = ship_module_slot_type::science;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::salvager_i].ActivationStepMethod = &ModuleUpdateSalvager;

		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::foreman_i].ID = ship_module_id::foreman_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::foreman_i].ItemID = item_id::foreman_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::foreman_i].DisplayName = "Foreman I";
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::foreman_i].ActivationRange = 100.0f;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::foreman_i].ActivationRangeDisplayColor = COLOR_PURPLE;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::foreman_i].SlotType = ship_module_slot_type::science;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::foreman_i].Foreman.ReductionMinutes = 5;

		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::cargo_expansion_i].ID = ship_module_id::cargo_expansion_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::cargo_expansion_i].ItemID = item_id::cargo_expansion_i;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::cargo_expansion_i].DisplayName = "Cargo Expansion MKI";
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::cargo_expansion_i].SlotType = ship_module_slot_type::structural;
		Globals->AssetsList.ShipModuleDefinitions[(int)ship_module_id::cargo_expansion_i].CargoAddition = 20;
	}

	// Ships
	{
		static_assert(ArrayCount(Globals->AssetsList.ShipDefinitions) > gen_ship_id_count, "Ship definitions array not large enough.");

		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].ID = ship_id::advent;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].Icon = assets::GetImage("Ship_Advent");
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].FuelRateMassPerSecond = 0.5f;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].Mass = 170;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].RadarRadius = 75;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].FuelTankMassLimit = 150;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].HoldMass = 20;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].SlotsCount = 3;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].SlotTypes[0] = ship_module_slot_type::industrial;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].SlotTypes[1] = ship_module_slot_type::structural;
		Globals->AssetsList.ShipDefinitions[(int)ship_id::advent].SlotTypes[2] = ship_module_slot_type::science;
	}

	// Items
	{
		static_assert(ArrayCount(Globals->AssetsList.ItemDefinitions) > gen_item_id_count, "Item definitions list too small.");

		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].ID = item_id::venigen;
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].DisplayName = "Venigen";
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].DisplayDescription =
			"Base metal used for construction.";
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].Icon = assets::GetImage("Icon_Venigen");
		Globals->AssetsList.ItemDefinitions[(int)item_id::venigen].Stackable = true;

		Globals->AssetsList.ItemDefinitions[(int)item_id::pyrexium].ID = item_id::pyrexium;
		Globals->AssetsList.ItemDefinitions[(int)item_id::pyrexium].DisplayName = "Pyrexium";
		Globals->AssetsList.ItemDefinitions[(int)item_id::pyrexium].DisplayDescription = 
			"Base metal used for construction.";
		Globals->AssetsList.ItemDefinitions[(int)item_id::pyrexium].Mass = 5;
		Globals->AssetsList.ItemDefinitions[(int)item_id::pyrexium].Icon = assets::GetImage("Icon_Pyrexium");
		Globals->AssetsList.ItemDefinitions[(int)item_id::pyrexium].Stackable = true;

		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].ID = item_id::sm_asteroid_miner;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].DisplayName = "Asteroid Miner";
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].DisplayDescription = 
			"Basic asteroid miner. Mines asteroids into raw materials.";
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].ShipModuleID = ship_module_id::asteroid_miner;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].Icon = 
			Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::asteroid_miner];

		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].ID = item_id::sm_salvager_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].DisplayName = "Salvager MK1";
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].DisplayDescription = 
			"Converts salvage into knowledge, which is used to unlock nodes on the skill tree.";
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].ShipModuleID = ship_module_id::salvager_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].Icon = 
			Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::asteroid_miner];

		Globals->AssetsList.ItemDefinitions[(int)item_id::cargo_expansion_i].ID = item_id::cargo_expansion_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::cargo_expansion_i].DisplayName = "Cargo Expansion I";
		Globals->AssetsList.ItemDefinitions[(int)item_id::cargo_expansion_i].DisplayDescription = 
			"Expands cargo. Gives a ship more cargo space.";
		Globals->AssetsList.ItemDefinitions[(int)item_id::cargo_expansion_i].ShipModuleID = ship_module_id::cargo_expansion_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::cargo_expansion_i].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::cargo_expansion_i].Icon = 
			Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::cargo_expansion_i];

		Globals->AssetsList.ItemDefinitions[(int)item_id::foreman_i].ID = item_id::foreman_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::foreman_i].DisplayName = "Forman I";
		Globals->AssetsList.ItemDefinitions[(int)item_id::foreman_i].DisplayDescription = 
			"Decreases activation time of all industrial modules within a radius.";
		Globals->AssetsList.ItemDefinitions[(int)item_id::foreman_i].ShipModuleID = ship_module_id::foreman_i;
		Globals->AssetsList.ItemDefinitions[(int)item_id::foreman_i].Mass = 1;
		Globals->AssetsList.ItemDefinitions[(int)item_id::foreman_i].Icon = 
			Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::foreman_i];

		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].ID = item_id::stl;
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].DisplayName = "STL Fuel";
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].DisplayDescription = 
			"Slow Than Light fuel used to move ships at sub-light-speed speeds.";
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].Mass = 0.25f;
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].Icon = assets::GetImage("Icon_STL");
		Globals->AssetsList.ItemDefinitions[(int)item_id::stl].Stackable = true;
	}

	// Recipes
	{
		static_assert(ArrayCount(Globals->AssetsList.RecipeDefinitions) > (int)recipe_id::count, "Recipe definitions asset not large enough.");
		static_assert(ArrayCount(Globals->AssetsList.RecipesByService) > (int)station_service::count, "Recipes by service not large enough.");
		static_assert(ArrayCount(Globals->AssetsList.RecipesByService[0].IDs) > (int)recipe_id::count, "Service ids array not large enough");

		// venigen_stl
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].ID = recipe_id::venigen_stl;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].DisplayName = Globals->AssetsList.ItemDefinitions[(int)item_id::stl].DisplayName.Array();
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].RegisterInput(item_id::venigen, 5.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].RegisterOutput(item_id::stl, 15.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].DurationMS = MinutesToMilliseconds(1);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].ServiceRequired = station_service::refinery;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::venigen_stl].Icon = assets::GetImage("Icon_STL");

		// ship_advent
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::ship_advent].ID = recipe_id::ship_advent;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::ship_advent].DisplayName = "Ship Advent";
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::ship_advent].RegisterInput(item_id::venigen, 5.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::ship_advent].RegisterOutput(ship_id::advent, 1.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::ship_advent].DurationMS = HoursToMilliseconds(10);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::ship_advent].ServiceRequired = station_service::shipyard;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::ship_advent].Icon = assets::GetImage("Ship_Advent");


		// sm_salvager_i
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::sm_salvager_i].ID = recipe_id::sm_salvager_i;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::sm_salvager_i].RegisterInput(item_id::pyrexium, 20.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::sm_salvager_i].DisplayName = Globals->AssetsList.ItemDefinitions[(int)item_id::sm_salvager_i].DisplayName.Array();
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::sm_salvager_i].RegisterOutput(item_id::sm_salvager_i, 1.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::sm_salvager_i].DurationMS = MinutesToMilliseconds(60);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::sm_salvager_i].ServiceRequired = station_service::manufacturing;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::sm_salvager_i].Icon = 
				Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::salvager_i];

		// asteroid_miner_i
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::asteroid_miner_i].ID = recipe_id::asteroid_miner_i;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::asteroid_miner_i].DisplayName = Globals->AssetsList.ItemDefinitions[(int)item_id::sm_asteroid_miner].DisplayName.Array();
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::asteroid_miner_i].RegisterInput(item_id::pyrexium, 20.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::asteroid_miner_i].RegisterOutput(item_id::sm_asteroid_miner, 1.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::asteroid_miner_i].DurationMS = MinutesToMilliseconds(60);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::asteroid_miner_i].ServiceRequired = station_service::manufacturing;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::asteroid_miner_i].Icon = 
				Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::asteroid_miner];

		// cargo_expansion_i
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::cargo_expansion_i].ID = recipe_id::cargo_expansion_i;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::cargo_expansion_i].DisplayName = Globals->AssetsList.ItemDefinitions[(int)item_id::cargo_expansion_i].DisplayName.Array();
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::cargo_expansion_i].RegisterInput(item_id::pyrexium, 20.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::cargo_expansion_i].RegisterOutput(item_id::cargo_expansion_i, 1.0f);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::cargo_expansion_i].DurationMS = MinutesToMilliseconds(60);
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::cargo_expansion_i].ServiceRequired = station_service::manufacturing;
		Globals->AssetsList.RecipeDefinitions[(int)recipe_id::cargo_expansion_i].Icon = 
				Globals->AssetsList.ShipModuleIcons[(int)ship_module_id::cargo_expansion_i];

		// Organize recipes by service
		{
			for (int i = 0; i < (int)recipe_id::count; i++) {
				station_service Service = Globals->AssetsList.RecipeDefinitions[i].ServiceRequired;

				recipe_list* List = &Globals->AssetsList.RecipesByService[(int)Service];
				List->IDs[List->Count++] = Globals->AssetsList.RecipeDefinitions[i].ID;
				Assert(List->Count < ArrayCount(List->IDs));
			}
		}
	}
}
