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
	private UniversalPosition destination;

	void Update()
	{
		if (RytInput.leftTouch.onUp && !RytInput.leftTouch.moved) {
			SetDestination(RytInput.leftTouch.GetCurrentUniversal());
		}

		if (hasDestination) {
			DrawScreenLine.Draw(ship.physics.pos, destination, Color.green);

			float distanceMiles = Vector2.Distance(ship.physics.pos.Get(), destination.Get());

			time.data.text = "";
			fuel.data.text = "";
			distance.Set(distanceMiles.ToString("0.0"), "mi");
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

	public void SetDestination(UniversalPosition uniPos)
	{

		hasDestination = true;
		destination = uniPos;

		selectDestinationText.SetActive(false);

		fuel.gameObject.SetActive(true);
		time.gameObject.SetActive(true);
		distance.gameObject.SetActive(true);
	}
}