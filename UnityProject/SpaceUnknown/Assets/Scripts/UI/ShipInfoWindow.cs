using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ShipInfoWindow : MonoBehaviour
{
	public Ship ship;

	public CargoItemLine cargoItemPrefab;
	public GameObject cargoGridLayout;

	public ModuleLine moduleLine;
	public ProgressBarText storageDisplay;

	//private List<

	void Start()
	{
		moduleLine.module = ship.modules[0];
	}

	void Update()
	{
		storageDisplay.Set(ship.CurrentStorageTons(), ship.storageTons, "t");
	}

	public void Show(Ship ship)
	{
		this.ship = ship;

	}

	public void AddNewItem(ItemInstance item)
	{
		CargoItemLine cargoLine = Instantiate(cargoItemPrefab, cargoGridLayout.transform);
		cargoLine.SetItem(item);
	}
}