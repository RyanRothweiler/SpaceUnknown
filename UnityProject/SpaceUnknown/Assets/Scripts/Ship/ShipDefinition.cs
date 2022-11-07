using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "ShipDefinition", menuName = "ScriptableObjects/ShipDefinition")]
public class ShipDefinition : ScriptableObject
{
	public float massTons;
	public float storageTons;
	public float fuelTankGallons;
	public float fuelRateGallonsPerSecond;
}