using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UniversalPositionApply : MonoBehaviour
{

	public UniversalPosition univPos;

	private Vector3 origScale;
	public static List<UniversalPositionApply> allUniversals = new List<UniversalPositionApply>();

	void Awake()
	{
		univPos = UniversalPosition.UnityToUniverse(this.transform.position);
		origScale = this.transform.localScale;

		allUniversals.Add(this);
	}

	void Update()
	{
		this.transform.position = UniversalPosition.UniverseToUnity(univPos);
		this.transform.localScale = new Vector3((origScale.x / CameraControls.unityUnitsToGameUnits),
		                                        (origScale.y / CameraControls.unityUnitsToGameUnits),
		                                        (origScale.z / CameraControls.unityUnitsToGameUnits));
	}

	void OnDestroy()
	{
		allUniversals.Remove(this);
	}
}