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
			float dataTime = 0;
			float stepSeconds = 1.0f / Application.targetFrameRate;

			Vector2 startPos = ship.physics.pos.Get();

			int c = 0;
			while (Ship.SimulateMovement(ref ship.physics, ship.def, ship.TotalMass(), destination, stepSeconds)) {
				c++;
				//if (c > 10000) break;
				dataTime += stepSeconds;

				//float distToTarget = Vector2.Distance(physics.pos.Get(), destination.Get());
				//Debug.Log(physics.velocity + " dist " + distToTarget);

				//yield return null;
			}
			time.Set(string.Format("{0:0,0.0}", dataTime), "s");

			ship.physics.pos.Set(startPos);
		}
	}

	public void Submit()
	{
		ship.SetTargetPosition(destination);
	}
}