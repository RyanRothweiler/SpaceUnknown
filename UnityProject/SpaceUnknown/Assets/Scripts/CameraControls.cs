using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CameraControls : MonoBehaviour
{
	// divide by this. a 1 here is 1 unity unit == 1 mile. 10 here means 1 unity unit = 10 miles
	public static float unityUnitsToGameUnits = 1;

	private static float convTarget;

	[Serializable]
	public class ZoomStop
	{
		public float zoom;
		public float stopPosition;
	};

	public ZoomStopDisplay zoomStopFab;
	public GameObject zoomHolder;

	public List<ZoomStop> zoomStopsOrdered;
	// Zoom labels is assumed to be 1 - zoomStopsOrdered.count. The labest sit between the stops
	public List<string> zoomLabels;

	public Slider zoomSlider;
	private float maxStopPosition;

	void Awake()
	{
		maxStopPosition = zoomStopsOrdered[zoomStopsOrdered.Count - 1].stopPosition;

		// place stop labels
		{
			for (int i = 1; i < zoomStopsOrdered.Count; i++) {
				ZoomStop lowStop = zoomStopsOrdered[i - 1];
				ZoomStop highStop = zoomStopsOrdered[i];

				float lowPerc = (lowStop.stopPosition / maxStopPosition);
				float highPerc = (highStop.stopPosition / maxStopPosition);

				ZoomStopDisplay disp = Instantiate(zoomStopFab, zoomHolder.transform);
				disp.text.text = zoomLabels[i - 1];

				RectTransform rtrans = disp.GetComponent<RectTransform>();
				rtrans.anchoredPosition3D = new Vector3(0, 0, 0);
				rtrans.anchorMin = new Vector2(1, 1 - highPerc);
				rtrans.anchorMax = new Vector2(1, 1 - lowPerc);
				rtrans.SetTop(0);
				rtrans.SetBottom(0);
			}
		}
	}

	void Update()
	{
		//convTarget = Mathf.Clamp(convTarget + (RytInput.scrollAmount * -0.25f), 1, 100000);
		//unityUnitsToGameUnits = Mathf.Lerp(unityUnitsToGameUnits, convTarget, Time.deltaTime * 10.0f);
		//Debug.Log(unityUnitsToGameUnits);
		zoomSlider.value += RytInput.scrollAmount * -0.01f;

		Vector3 newPos = this.transform.position;
		//newPos.z = Mathf.Clamp(newPos.z + RytInput.scrollAmount, -1000.0f, -10.0f);
		if (RytInput.leftTouch.moved) {
			newPos.x += RytInput.leftTouch.screenDelta.x * 0.05f;
			newPos.y += RytInput.leftTouch.screenDelta.y * 0.05f;
		}
		//this.transform.position = newPos;

		// slider zoom
		{
			float sliderStopPosition = zoomSlider.value * maxStopPosition;
			ZoomStop lowStop = null;
			ZoomStop highStop = null;
			for (int i = 1; i < zoomStopsOrdered.Count; i++) {
				if (sliderStopPosition <= zoomStopsOrdered[i].stopPosition &&
				        sliderStopPosition >= zoomStopsOrdered[i - 1].stopPosition
				   ) {
					lowStop = zoomStopsOrdered[i - 1];
					highStop = zoomStopsOrdered[i];
				}
			}
			float fenceAmount = highStop.stopPosition - lowStop.stopPosition;
			float valueFenceNormalized = (sliderStopPosition - lowStop.stopPosition) / fenceAmount;

			unityUnitsToGameUnits = Mathf.Lerp(lowStop.zoom, highStop.zoom, valueFenceNormalized);
		}
	}
}