using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UniversalPositionMono : MonoBehaviour
{
	public UniversalPosition pos;

	private Vector3 origScale;

	public static List<UniversalPositionMono> allUniversals = new List<UniversalPositionMono>();

	void Awake()
	{
		UniversalPosition.UnityToUniverse(pos, this.transform.position);
		origScale = this.transform.localScale;

		allUniversals.Add(this);
	}

	void Update()
	{
		this.transform.position = pos.UniverseToUnity();
		this.transform.localScale = new Vector3((origScale.x / CameraControls.unityUnitsToGameUnits),
		                                        (origScale.y / CameraControls.unityUnitsToGameUnits),
		                                        (origScale.z / CameraControls.unityUnitsToGameUnits));
	}
}