using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "ModuleDefinition", menuName = "ScriptableObjects/Module/ModuleDefinition", order = 1)]
public class ModuleDefinition : ScriptableObject
{
	public enum Type { Active };

	[Header("Base")]
	public string displayName;
	public string id;
	public Sprite icon;
	public Type type;

	[Header("Active")]
	public float activationTimeWorldMinutes;
	public ItemDefinition completionReward;
	public int comletionCount;
}