using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class IssueMovementWindow : MonoBehaviour
{
	public Ship ship;

	public Slider effeciencySlider;
	public WarningTicker problemDisplay;
	public GameObject submitButton;
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
			SetDestination(RytInput.leftTouch.GetCurrentUniversal());
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
		problemDisplay.Display("No target destination. \n Tap to set target destination.");

		fuel.gameObject.SetActive(false);
		time.gameObject.SetActive(false);
		distance.gameObject.SetActive(false);
		submitButton.SetActive(false);
	}

	public void SliderChanged()
	{
		if (hasDestination) {
			SetDestination(destination);
		}
	}

	public void SetDestination(Vector2 dest)
	{
		hasDestination = true;
		destination = dest;

		problemDisplay.Clear();

		fuel.gameObject.SetActive(true);
		time.gameObject.SetActive(true);
		distance.gameObject.SetActive(true);

		// distance
		distanceMiles = Vector2.Distance(ship.physics.pos.Get(), destination) * Units.UnityToMiles;
		distance.Set(distanceMiles, "mi");

		fuel.data.text = "";

		// time and fuel
		{
			float dataTimeSeconds = 0;
			float stepSeconds = 1.0f / Application.targetFrameRate;

			Vector2 startPos = ship.physics.pos.Get();
			float fuelStart = ship.physics.fuelGallons;

			Ship.JourneySettings settings = Ship.GetJourneySettings(ship, effeciencySlider.value, destination);
			while (Ship.SimulateMovement(ref ship.physics, ship.def, ship.TotalMass(), destination, stepSeconds, settings)) {
				dataTimeSeconds += stepSeconds;
			}

			float fuelUsed = fuelStart - ship.physics.fuelGallons;
			fuel.Set(fuelUsed, "gal");

			TimeSpan timeSpan = new TimeSpan(0, 0, (int)dataTimeSeconds);
			time.Set(ToReadableString(timeSpan), "");

			ship.physics.pos.Set(startPos);
			ship.physics.fuelGallons = fuelStart;

			if (ship.physics.fuelGallons < fuelUsed) {
				problemDisplay.Display("Not enough fuel for journey");
				submitButton.SetActive(false);
			} else {
				submitButton.SetActive(true);
			}
		}
	}

	public void Submit()
	{
		ship.SetTargetPosition(destination, 1.0f);
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