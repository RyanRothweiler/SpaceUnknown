using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class AsteroidInfoWindow : MonoBehaviour
{
	public DataLine dataLine;
	public AsteroidInstance asteroid;

	void Update()
	{
		UIManager.Instance.DrawSelectionLine(asteroid.gameObject);

		dataLine.title.text = "Venigen";
		dataLine.data.text = "x" + asteroid.contentsAmount;

		if (!asteroid.gameObject.activeInHierarchy) {
			UIManager.Instance.Back();
		}
	}
}