using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Module
{
	private float activationTimeWorldMinutes = 120;
	private float currentActivationMinutes;

	public Ship ship;
	public Mineable target;

	public float PercentageActivation()
	{
		return currentActivationMinutes / activationTimeWorldMinutes;
	}

	public void Step(float time)
	{
		if (target != null) {

			if (Vector2.Distance(ship.uniPos.Get(), target.pos.Get()) < 0.25f) {

				currentActivationMinutes += time;
				if (currentActivationMinutes >= activationTimeWorldMinutes) {
					currentActivationMinutes = currentActivationMinutes - activationTimeWorldMinutes;
					// activate
				}
			} else {
				currentActivationMinutes = 0;
			}
		} else {
			currentActivationMinutes = 0;
		}
	}
}
