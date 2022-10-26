using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ModuleInstance
{
	public ModuleDefinition definition;

	private float currentActivationMinutes;

	private Ship ship;
	private Mineable target;

	public ModuleInstance(ModuleDefinition definition, Ship ship)
	{
		this.definition = definition;
		this.ship = ship;
	}

	public float PercentageActivation()
	{
		return currentActivationMinutes / definition.activationTimeWorldMinutes;
	}

	public void Step(float time)
	{
		if (definition.type == ModuleDefinition.Type.Active)  {
			if (target != null) {

				if (Vector2.Distance(ship.pos.Get(), target.pos.Get()) < definition.range) {
					DrawWorldLine.Draw(ship.pos.UniverseToUnity(), target.pos.UniverseToUnity(), Color.red);

					currentActivationMinutes += time;
					while (currentActivationMinutes >= definition.activationTimeWorldMinutes) {
						currentActivationMinutes = currentActivationMinutes - definition.activationTimeWorldMinutes;
						ship.GiveItem(definition.completionReward, definition.comletionCount);
					}
				} else {
					target = null;
					currentActivationMinutes = 0;
				}
			} else {
				currentActivationMinutes = 0;

				// Find target
				foreach (Mineable mineable in GameManager.Instance.mineables) {
					if (Vector2.Distance(mineable.pos.Get(), ship.pos.Get()) < definition.range) {
						target = mineable;
						break;
					}
				}
			}
		}
	}
}
