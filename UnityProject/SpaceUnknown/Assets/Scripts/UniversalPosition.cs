using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UniversalPosition : MonoBehaviour
{
	// Units here are just made up, not related to anything in the real world
	public long x;
	public long y;

	void Update()
	{
		Vector3 pos = new Vector3();
		pos.x = (float)(x / CameraControls.unityUnitsToGameUnits);
		pos.y = (float)(y / CameraControls.unityUnitsToGameUnits);
		this.transform.position = pos;
	}
}
