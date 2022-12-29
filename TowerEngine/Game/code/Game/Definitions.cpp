void CreateDefinitions()
{
	Globals->AssetsList.Definition_Module_AsteroidMiner.DisplayName = "Asteroid Miner";
	Globals->AssetsList.Definition_Module_AsteroidMiner.ActivationTimeMS = SecondsToMilliseconds(60.0f * 2.0f);
	Globals->AssetsList.Definition_Module_AsteroidMiner.ActivationRange = 40.0f;

	Globals->AssetsList.Definition_Ship_First.FuelRateGallonsPerSecond = 1.0f;
	Globals->AssetsList.Definition_Ship_First.Mass = 200;
	Globals->AssetsList.Definition_Ship_First.CargoMassLimit = 100;
	Globals->AssetsList.Definition_Ship_First.FuelTankGallons = 300;

	Globals->AssetsList.ItemDefinitions[0].ID = item_id::venigen;
	Globals->AssetsList.ItemDefinitions[0].DisplayName = "Venigen";
	Globals->AssetsList.ItemDefinitions[0].Mass = 1;
	Globals->AssetsList.ItemDefinitions[0].Stackable = true;
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