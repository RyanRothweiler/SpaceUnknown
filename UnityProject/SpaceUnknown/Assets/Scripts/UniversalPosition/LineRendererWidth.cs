using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LineRendererWidth : MonoBehaviour
{
	private LineRenderer liner;

	public void OnEnable()
	{
		RytUtil.GetIfNeed<LineRenderer>(ref liner, this);
		liner.widthMultiplier = 1.0f / CameraControls.unityUnitsToGameUnits;
	}
}