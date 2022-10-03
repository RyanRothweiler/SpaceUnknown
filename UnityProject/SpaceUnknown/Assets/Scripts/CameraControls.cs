using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraControls : MonoBehaviour
{
	// divide by this. a 1 here is 1 unity unit == 1 mile. 10 here means 1 unity unit = 10 miles
	public static double unityUnitsToGameUnits = 1;

	void Update()
	{
		unityUnitsToGameUnits = Math.Clamp(unityUnitsToGameUnits + (double)RytInput.scrollAmount * -0.5f, 1, 100000);

		Vector3 newPos = this.transform.position;
		//newPos.z = Mathf.Clamp(newPos.z + RytInput.scrollAmount, -1000.0f, -10.0f);
		if (RytInput.leftTouch.moved) {
			newPos.x += RytInput.leftTouch.screenDelta.x * 0.05f;
			newPos.y += RytInput.leftTouch.screenDelta.y * 0.05f;
		}
		this.transform.position = newPos;
	}
}
