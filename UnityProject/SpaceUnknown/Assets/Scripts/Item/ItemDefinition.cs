using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "ItemDefinition", menuName = "ScriptableObjects/Item Definition", order = 1)]
public class ItemDefinition : ScriptableObject
{
	public string displayName;
	public string id;
	public bool stackable;
	public float weightTons;
	public Sprite icon;
}