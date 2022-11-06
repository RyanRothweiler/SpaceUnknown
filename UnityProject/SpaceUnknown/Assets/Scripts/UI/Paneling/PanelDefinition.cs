using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "PanelDefinition", menuName = "ScriptableObjects/PanelDefinition")]
public class PanelDefinition : ScriptableObject
{
	public enum ID {
		ShipInfo, Commands, AsteroidInfo, IssueMovement
	};

	public GameObject fab;
	public ID id;
}