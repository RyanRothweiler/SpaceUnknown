using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class UniversalPosition
{
	// Units here are miles, but don't take that too seriously
	public float x;
	public float y;

	// just for convenience
	public void Set(Vector3 pos)
	{
		x = pos.x;
		y = pos.y;
	}

	public Vector2 Get()
	{
		return new Vector2((float)x, (float)y);
	}

	public Vector3 ToUnity()
	{
		return UniverseToUnity(this);
	}

	public static Vector3 UniverseToUnity(UniversalPosition uniPos)
	{
		Vector3 pos = new Vector3();
		pos.x = (float)(uniPos.x / CameraControls.unityUnitsToGameUnits);
		pos.y = (float)(uniPos.y / CameraControls.unityUnitsToGameUnits);
		pos.z = 0;
		return pos;
	}

	public static UniversalPosition UnityToUniverse(Vector3 pos)
	{
		UniversalPosition ret = new UniversalPosition();
		ret.x = pos.x * CameraControls.unityUnitsToGameUnits;
		ret.y = pos.y * CameraControls.unityUnitsToGameUnits;
		return ret;
	}
}