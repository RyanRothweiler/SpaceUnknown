using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Module
{
	private float activationTimeWorldMinutes = 120;
	private float currentActivationMinutes;

	public float PercentageActivation()
	{
		return currentActivationMinutes / activationTimeWorldMinutes;
	}

	public void Step(float time)
	{
		currentActivationMinutes += time;
		if (currentActivationMinutes >= activationTimeWorldMinutes) {
			currentActivationMinutes = currentActivationMinutes - activationTimeWorldMinutes;
			// activate
		}
	}
}
