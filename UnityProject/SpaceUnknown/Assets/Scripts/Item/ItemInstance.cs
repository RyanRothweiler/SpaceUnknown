using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ItemInstance
{
	public ItemDefinition definition;
	public int count;

	public ItemInstance(ItemDefinition def)
	{
		definition = def;
	}

	public float GetWeightTons()
	{
		return count * definition.weightTons;
	}
}