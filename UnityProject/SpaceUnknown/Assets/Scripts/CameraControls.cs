using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraControls : MonoBehaviour
{
	// divide by this. a 1 here is 1 unity unit == 1 mile. 10 here means 1 unity unit = 10 miles
	public static float unityUnitsToGameUnits = 1;

	private static float convTarget;

	void Update()
	{
		convTarget = Mathf.Clamp(convTarget + (RytInput.scrollAmount * -0.25f), 1, 100000);
		unityUnitsToGameUnits = Mathf.Lerp(unityUnitsToGameUnits, convTarget, Time.deltaTime * 10.0f);
		//Debug.Log(unityUnitsToGameUnits);

		Vector3 newPos = this.transform.position;
		//newPos.z = Mathf.Clamp(newPos.z + RytInput.scrollAmount, -1000.0f, -10.0f);
		if (RytInput.leftTouch.moved) {
			newPos.x += RytInput.leftTouch.screenDelta.x * 0.05f;
			newPos.y += RytInput.leftTouch.screenDelta.y * 0.05f;
		}
		this.transform.position = newPos;
	}
}
