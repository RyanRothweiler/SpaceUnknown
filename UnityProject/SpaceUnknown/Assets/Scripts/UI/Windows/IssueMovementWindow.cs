using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class IssueMovementWindow : MonoBehaviour
{
	public Ship ship;

	public GameObject selectDestinationText;
	public DataLine fuel;
	public DataLine time;
	public DataLine distance;

	private bool hasDestination;
	private Vector2 destination;

	private float distanceMiles;
	private float dataTime;

	void Update()
	{
		if (RytInput.leftTouch.onUp && !RytInput.leftTouch.moved) {
			StartCoroutine(SetDestination(RytInput.leftTouch.GetCurrentUniversal()));
		}

		if (hasDestination) {
			DrawScreenLine.DrawFromWorld(
			    UniversalPosition.UniverseToUnity(ship.physics.pos),
			    UniversalPosition.UniverseToUnity(destination),
			    Color.green);
		}
	}

	void OnEnable()
	{
		hasDestination = false;
		selectDestinationText.SetActive(true);

		fuel.gameObject.SetActive(false);
		time.gameObject.SetActive(false);
		distance.gameObject.SetActive(false);
	}

	public IEnumerator SetDestination(Vector2 dest)
	{
		yield return null;

		hasDestination = true;
		destination = dest;

		selectDestinationText.SetActive(false);

		fuel.gameObject.SetActive(true);
		time.gameObject.SetActive(true);
		distance.gameObject.SetActive(true);

		// distance
		distanceMiles = Vector2.Distance(ship.physics.pos.Get(), destination) * Units.UnityToMiles;
		distance.Set(distanceMiles.ToString("0.0"), "mi");

		fuel.data.text = "";

		// time and fuel
		{
			float dataTimeSeconds = 0;
			float stepSeconds = 1.0f / Application.targetFrameRate;

			Vector2 startPos = ship.physics.pos.Get();
			float fuelStart = ship.physics.fuelGallons;

			while (Ship.SimulateMovement(ref ship.physics, ship.def, ship.TotalMass(), destination, stepSeconds)) {
				dataTimeSeconds += stepSeconds;
			}

			float fuelUsed = fuelStart - ship.physics.fuelGallons;
			fuel.Set(string.Format("{0:0,0.0}", fuelUsed), "gal");

			TimeSpan timeSpan = new TimeSpan(0, 0, (int)dataTimeSeconds);
			time.Set(ToReadableString(timeSpan), "");

			ship.physics.pos.Set(startPos);
			ship.physics.fuelGallons = fuelStart;
		}
	}

	public void Submit()
	{
		ship.SetTargetPosition(destination);
	}

	public static string ToReadableString(TimeSpan span)
	{
		string formatted = string.Format("{0}{1}{2}{3}",
		                                 span.Duration().Days > 0 ? string.Format("{0:0} d, ", span.Days) : string.Empty,
		                                 span.Duration().Hours > 0 ? string.Format("{0:0} h, ", span.Hours) : string.Empty,
		                                 span.Duration().Minutes > 0 ? string.Format("{0:0} m, ", span.Minutes) : string.Empty,
		                                 span.Duration().Seconds > 0 ? string.Format("{0:0} s", span.Seconds) : string.Empty);

		if (formatted.EndsWith(", ")) formatted = formatted.Substring(0, formatted.Length - 2);

		if (string.IsNullOrEmpty(formatted)) formatted = "0 seconds";

		return formatted;
	}
}