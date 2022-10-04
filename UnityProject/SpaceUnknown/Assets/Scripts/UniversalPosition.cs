using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UniversalPosition : MonoBehaviour
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
		return pos;
	}

	void Update()
	{
		this.transform.position = UniverseToUnity();
	}

	public static UniversalPosition UnityToUniverse(Vector3 pos)
	{
		UniversalPosition ret = new UniversalPosition();
		ret.x = pos.x * CameraControls.unityUnitsToGameUnits;
		ret.y = pos.y * CameraControls.unityUnitsToGameUnits;
		return ret;
	}

}
