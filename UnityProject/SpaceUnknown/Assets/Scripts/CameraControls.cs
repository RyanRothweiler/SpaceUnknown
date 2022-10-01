using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraControls : MonoBehaviour
{
	void Start()
	{

	}

	void Update()
	{
		Vector3 newPos = this.transform.position;
		newPos.z = Mathf.Clamp(newPos.z + RytInput.scrollAmount, -1000.0f, -10.0f);
		if (RytInput.leftTouch.moved) {
			newPos.x += RytInput.leftTouch.screenDelta.x * 0.05f;
			newPos.y += RytInput.leftTouch.screenDelta.y * 0.05f;
		}
		this.transform.position = newPos;
	}
}
