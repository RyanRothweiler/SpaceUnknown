using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UniversalPositionMono : MonoBehaviour
{
	public UniversalPosition pos;

	void Update()
	{
		this.transform.position = pos.UniverseToUnity();
	}
}
