using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ShipInfoWindow : MonoBehaviour
{
	public Ship ship;

	public ModuleLine moduleLine;
	public ProgressBarText storageDisplay;

	void Start()
	{
		moduleLine.module = ship.modules[0];
	}

	void Update()
	{
		storageDisplay.Set(ship.CurrentStorageTons(), ship.storageTons, "t");
	}
}
