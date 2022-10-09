using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class RytUtil
{

	public static void GetIfNeed<T>(ref T component, MonoBehaviour self)
	{
		if (component == null) { component = self.GetComponent<T>(); }
	}
}