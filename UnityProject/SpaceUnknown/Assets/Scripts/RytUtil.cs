using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class RytUtil
{
	public static void GetIfNeed<T>(ref T component, MonoBehaviour self)
	{
		if (component == null) { component = self.GetComponent<T>(); }
	}

	public static bool IsAlive(GameObject target)
	{
		return target != null && target.activeInHierarchy;
	}

	public static string HumanFloat(float value)
	{
		return string.Format("{0:0,0.0}", value);
	}
}