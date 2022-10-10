using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ShipInfoWindow : MonoBehaviour
{
	public static ShipInfoWindow Instance;

	public Ship ship;

	public ModuleLine moduleLinePrefab;
	public GameObject marginHolder;
	public GameObject modulesHeader;

	public CargoItemLine cargoItemPrefab;
	public GameObject cargoGridLayout;

	public ProgressBarText storageDisplay;

	private List<GameObject> currentLines = new List<GameObject>();

	void Awake()
	{
		Instance = this;
		this.gameObject.SetActive(false);
	}

	void Update()
	{
		storageDisplay.Set(ship.CurrentStorageTons(), ship.storageTons, "t");
	}

	public void Show(Ship ship)
	{
		this.gameObject.SetActive(true);
		this.ship = ship;

		// remove old lines
		for (int i = 0; i < currentLines.Count; i++) {
			Destroy(currentLines[i]);
		}
		currentLines = new List<GameObject>();

		// add new modules
		for (int i = 0; i < ship.modules.Count; i++) {
			ModuleLine modLine = Instantiate(moduleLinePrefab, marginHolder.transform);
			modLine.module = ship.modules[i];
			modLine.transform.SetSiblingIndex(modulesHeader.transform.GetSiblingIndex() + 1);
		}
	}

	public void AddNewItem(ItemInstance item)
	{
		CargoItemLine cargoLine = Instantiate(cargoItemPrefab, cargoGridLayout.transform);
		cargoLine.SetItem(item);
	}
}