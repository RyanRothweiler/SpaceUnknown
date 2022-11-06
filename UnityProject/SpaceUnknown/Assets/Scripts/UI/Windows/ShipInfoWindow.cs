using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ShipInfoWindow : MonoBehaviour
{
	public Ship ship;

	public GameObject windowBG;

	public ModuleLine moduleLinePrefab;
	public GameObject marginHolder;
	public GameObject modulesHeader;

	public CargoItemLine cargoItemPrefab;
	public GameObject cargoGridLayout;

	public ProgressBarText storageDisplay;
	public ProgressBarText fuelDisplay;

	private List<GameObject> currentLines = new List<GameObject>();

	void Update()
	{
		storageDisplay.Set(ship.CurrentStorageTons(), ship.def.storageTons, "t");
		fuelDisplay.Set(ship.fuelGallons, ship.def.fuelTankGallons, "gal");

		UIManager.Instance.DrawSelectionLine(ship.gameObject);
	}

	public void Show(Ship ship)
	{
		this.gameObject.SetActive(true);
		windowBG.SetActive(true);

		this.ship = ship;

		// remove old module lines
		for (int i = 0; i < currentLines.Count; i++) {
			Destroy(currentLines[i]);
		}
		currentLines = new List<GameObject>();

		// add new modules
		for (int i = 0; i < ship.modules.Count; i++) {
			ModuleLine modLine = Instantiate(moduleLinePrefab, marginHolder.transform);
			modLine.module = ship.modules[i];
			modLine.transform.SetSiblingIndex(modulesHeader.transform.GetSiblingIndex() + 1);

			currentLines.Add(modLine.gameObject);
		}

		// add cargo
		for (int i = 0; i < ship.cargo.Count; i++) {
			AddNewItem(ship.cargo[i]);
		}
	}

	public void ShowCommands()
	{
		UIManager.Instance.ShowPanel(PanelDefinition.ID.Commands);
	}

	public void AddNewItem(ItemInstance item)
	{
		CargoItemLine cargoLine = Instantiate(cargoItemPrefab, cargoGridLayout.transform);
		cargoLine.SetItem(item);

		currentLines.Add(cargoLine.gameObject);
	}

	public void IssueMovementWindow()
	{
		GameObject issueWindowObj = UIManager.Instance.ShowPanel(PanelDefinition.ID.IssueMovement);
		IssueMovementWindow imw = issueWindowObj.GetComponent<IssueMovementWindow>();
		imw.ship = ship;
	}
}