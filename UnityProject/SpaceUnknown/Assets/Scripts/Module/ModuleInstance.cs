using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ModuleInstance
{
	public ModuleDefinition definition;

	private float currentActivationMinutes;

	public Ship ship;
	public Mineable target;

	public ModuleInstance(ModuleDefinition definition)
	{
		this.definition = definition;
	}

	public float PercentageActivation()
	{
		return currentActivationMinutes / definition.activationTimeWorldMinutes;
	}

	public void Step(float time)
	{
		if (definition.type == ModuleDefinition.Type.Active)  {
			if (target != null) {

				if (Vector2.Distance(ship.uniPos.Get(), target.pos.Get()) < 0.25f) {

					currentActivationMinutes += time;
					if (currentActivationMinutes >= definition.activationTimeWorldMinutes) {
						currentActivationMinutes = currentActivationMinutes - definition.activationTimeWorldMinutes;
						ship.GiveItem(definition.completionReward, definition.comletionCount);
					}
				} else {
					currentActivationMinutes = 0;
				}
			} else {
				currentActivationMinutes = 0;
			}
		}
	}
}
