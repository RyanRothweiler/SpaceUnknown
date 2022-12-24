item_definition ItemDefinitions[100];

ship_module_definition Definition_Module_AsteroidMiner = {};

ship_definition Definition_Ship_First = {};

void CreateDefinitions()
{
	Definition_Module_AsteroidMiner.DisplayName = "Asteroid Miner";
	Definition_Module_AsteroidMiner.ActivationTimeMS = SecondsToMilliseconds(60.0f * 2.0f);
	Definition_Module_AsteroidMiner.ActivationRange = 40.0f;

	Definition_Ship_First.FuelRateGallonsPerSecond = 1.0f;
	Definition_Ship_First.Mass = 100;

	ItemDefinitions[0].ID = item_id::venigen;
	ItemDefinitions[0].DisplayName = "Venigen";
	ItemDefinitions[0].Weight = 1;
	ItemDefinitions[0].Stackable = false;
}

item_definition GetItemDefinition(item_id ID)
{
	for (int i = 0; i < ArrayCount(ItemDefinitions); i++) {
		if (ItemDefinitions[i].ID == ID) {
			return ItemDefinitions[i];
		}
	}

	Assert(false); // Couldn't find that item ID
	return {};
}