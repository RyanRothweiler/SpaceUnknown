using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class IssueMovementWindow : MonoBehaviour
{
	public Ship ship;

	public RytSlider effeciencySlider;
	public WarningTicker problemDisplay;
	public GameObject submitButton;
	public DataLine fuel;
	public DataLine time;
	public DataLine distance;

	private bool hasDestination;
	private Vector2 destinationUniverse;

	private float distanceMiles;
	private float dataTime;

	void Update()
	{
		if (!ship.IsMoving()) {
			if (RytInput.leftTouch.onUp && !RytInput.leftTouch.moved) {
				SetDestination(RytInput.leftTouch.GetCurrentUniversal());
			}

			if (hasDestination) {
				DrawScreenLine.DrawFromWorld(
				    UniversalPosition.UniverseToUnity(ship.physics.pos),
				    UniversalPosition.UniverseToUnity(destinationUniverse),
				    Color.green);
			} else {
				problemDisplay.Display("No target destination. \n Tap to set target destination.");
			}
		} else {
			problemDisplay.Display("Cannot issue movment command while ship is moving.");
		}
	}

	void OnEnable()
	{
		hasDestination = false;

		effeciencySlider.gameObject.SetActive(false);
		fuel.gameObject.SetActive(false);
		time.gameObject.SetActive(false);
		distance.gameObject.SetActive(false);
		submitButton.SetActive(false);
	}

	public void SliderChanged()
	{
		if (hasDestination) {
			SetDestination(destinationUniverse);
		}
	}

	public void SetDestination(Vector2 dest)
	{
		hasDestination = true;
		destinationUniverse = dest;

		problemDisplay.Clear();

		effeciencySlider.gameObject.SetActive(true);
		fuel.gameObject.SetActive(true);
		time.gameObject.SetActive(true);
		distance.gameObject.SetActive(true);

		// distance
		distanceMiles = Vector2.Distance(ship.physics.pos.Get(), destinationUniverse) * Units.UnityToMiles;
		distance.Set(distanceMiles, "mi");

		fuel.data.text = "";

		// time and fuel
		{
			float dataTimeSeconds = 0;
			float stepSeconds = 1.0f / Application.targetFrameRate;

			Vector2 startPos = ship.physics.pos.Get();
			float fuelStart = ship.physics.fuelGallons;

			int counter = 0;

			Ship.JourneySettings settings = Ship.GetJourneySettings(ship, effeciencySlider.Value(), UniversalPosition.UniverseToUnity(destinationUniverse));
			while (Ship.SimulateMovement(ref ship.physics, ship.def, ship.TotalMass(), destinationUniverse, stepSeconds, settings)) {
				dataTimeSeconds += stepSeconds;

				counter++;
				if (counter > 10000) {
					Debug.LogError("Error calculating movement");
					break;
				}
			}

			float fuelUsed = fuelStart - ship.physics.fuelGallons;
			fuel.Set(fuelUsed, "gal");

			TimeSpan timeSpan = TimeSpan.FromSeconds(dataTimeSeconds);
			time.Set(ToReadableString(timeSpan), "");

			ship.physics.pos.Set(startPos);
			ship.physics.fuelGallons = fuelStart;
			ship.physics.velocity = new Vector2();

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
		effeciencySlider.gameObject.SetActive(false);
		fuel.gameObject.SetActive(false);
		time.gameObject.SetActive(false);
		distance.gameObject.SetActive(false);
		submitButton.SetActive(false);

		hasDestination = false;

		ship.SetTargetPosition(UniversalPosition.UniverseToUnity(destinationUniverse), 1.0f);
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