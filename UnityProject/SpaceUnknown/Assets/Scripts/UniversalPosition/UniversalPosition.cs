using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UniversalPosition : MonoBehaviour
{
	// Units here are miles, but don't take that too seriously
	public float x;
	public float y;

	private Vector3 origScale;

	public static List<UniversalPosition> allUniversals = new List<UniversalPosition>();

	void Awake()
	{
		Vector2 versePos = UniversalPosition.UnityToUniverse(this.transform.position);
		x = versePos.x;
		y = versePos.y;

		origScale = this.transform.localScale;

		allUniversals.Add(this);
	}

	void Update()
	{
		this.transform.position = UniverseToUnity(this);
		this.transform.localScale = new Vector3((origScale.x / CameraControls.unityUnitsToGameUnits),
		                                        (origScale.y / CameraControls.unityUnitsToGameUnits),
		                                        (origScale.z / CameraControls.unityUnitsToGameUnits));
	}

	void OnDestroy()
	{
		allUniversals.Remove(this);
	}

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

	public static Vector3 UniverseToUnity(UniversalPosition uniPos)
	{
		Vector3 pos = new Vector3();
		pos.x = (float)(uniPos.x / CameraControls.unityUnitsToGameUnits);
		pos.y = (float)(uniPos.y / CameraControls.unityUnitsToGameUnits);
		pos.z = 0;
		return pos;
	}

	public static Vector3 UniverseToUnity(Vector2 uniPos)
	{
		Vector3 pos = new Vector3();
		pos.x = (float)(uniPos.x / CameraControls.unityUnitsToGameUnits);
		pos.y = (float)(uniPos.y / CameraControls.unityUnitsToGameUnits);
		pos.z = 0;
		return pos;
	}

	public static Vector2 UnityToUniverse(Vector3 pos)
	{
		Vector2 ret = new Vector2();
		ret.x = pos.x * CameraControls.unityUnitsToGameUnits;
		ret.y = pos.y * CameraControls.unityUnitsToGameUnits;
		return ret;
	}
}