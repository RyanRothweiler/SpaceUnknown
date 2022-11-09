using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ModuleInstance
{
	public ModuleDefinition definition;

	private float currentActivationMinutes;

	private Ship ship;
	private AsteroidInstance target;

	public ModuleInstance(ModuleDefinition definition, Ship ship)
	{
		this.definition = definition;
		this.ship = ship;
	}

	public float PercentageActivation()
	{
		return currentActivationMinutes / definition.activationTimeWorldMinutes;
	}

	public void Update()
	{
		if (definition.type == ModuleDefinition.Type.Active)  {
			if (target != null && RytUtil.IsAlive(target.gameObject)) {
				if (Vector2.Distance(ship.physics.pos.Get(), target.pos.Get()) < definition.range) {
					DrawWorldLine.Draw(UniversalPosition.UniverseToUnity(ship.physics.pos), UniversalPosition.UniverseToUnity(target.pos), Color.red);
				}
			}
		}
	}

	public void Step(float time)
	{
		if (definition.type == ModuleDefinition.Type.Active)  {
			if (target != null && !target.gameObject.activeInHierarchy) {
				target = null;
			}

			if (target != null) {

				if (Vector2.Distance(ship.physics.pos.Get(), target.pos.Get()) < definition.range) {

					currentActivationMinutes += time;
					while (currentActivationMinutes >= definition.activationTimeWorldMinutes) {
						currentActivationMinutes = currentActivationMinutes - definition.activationTimeWorldMinutes;

						int amount = target.Withdraw(definition.comletionCount);
						ship.GiveItem(definition.completionReward, amount);
					}
				} else {
					target = null;
					currentActivationMinutes = 0;
				}
			} else {
				currentActivationMinutes = 0;

				// Find target
				foreach (AsteroidInstance asteroid in GameManager.Instance.asteroids) {
					if (Vector2.Distance(asteroid.pos.Get(), ship.physics.pos.Get()) < definition.range) {
						target = asteroid;
						break;
					}
				}
			}
		}
	}
}
