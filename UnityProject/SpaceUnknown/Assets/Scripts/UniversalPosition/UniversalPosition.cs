using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class UniversalPosition
{
	// Units here are just made up, not related to anything in the real world
	public double x;
	public double y;

	// just for convenience
	public void Set(Vector3 pos)
	{
		x = (double)pos.x;
		y = (double)pos.y;
	}

	public Vector2 Get()
	{
		return new Vector2((float)x, (float)y);
	}

	public Vector3 UniverseToUnity()
	{
		Vector3 pos = new Vector3();
		pos.x = (float)(x / CameraControls.unityUnitsToGameUnits);
		pos.y = (float)(y / CameraControls.unityUnitsToGameUnits);
		pos.z = 0;
		return pos;
	}

	public static void UnityToUniverse(UniversalPosition dest, Vector3 pos)
	{
		dest.x = pos.x * CameraControls.unityUnitsToGameUnits;
		dest.y = pos.y * CameraControls.unityUnitsToGameUnits;
	}

}
