using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ship : MonoBehaviour, IActor
{
	[System.Serializable]
	public struct Physics {
		public Vector2 velocity;
		public UniversalPosition pos;
		public float fuelGallons;
	};

	public ShipDefinition def;
	public ModuleDefinition testModuleDef;

	private bool hasTarget;
	private UniversalPosition targetPosition = new UniversalPosition();
	public Physics physics;

	public List<ModuleInstance> modules;
	public List<ItemInstance> cargo;

	private LineRenderer pathLine;

	// 0 means no conservation, boost all the way until counter boost.
	//private float fuelConservation;

	private const float fuelForcePerGallon = 1.0f;

	private List<float> currentFlightPlanForce;
	public ShipInfoWindow shipInfoWindow;


	public void Awake()
	{
		GameManager.RegisterActor(this);

		physics.pos = this.GetComponent<UniversalPositionMono>().pos;

		pathLine = this.GetComponent<LineRenderer>();
		pathLine.positionCount = 0;

		modules = new List<ModuleInstance>();
		modules.Add(new ModuleInstance(testModuleDef, this));

		cargo = new List<ItemInstance>();

		physics.fuelGallons = def.fuelTankGallons;
	}

	void Update()
	{
		if (hasTarget) {
			pathLine.positionCount = 2;

			Vector3[] positions = new Vector3[2];
			positions[0] = this.transform.position;
			positions[1] = targetPosition.UniverseToUnity();
			pathLine.SetPositions(positions);

		} else {
			pathLine.positionCount = 0;
		}

		// Render modules
		for (int i = 0; i < modules.Count; i++) {
			modules[i].Update();
		}
	}

	public void GiveItem(ItemDefinition def, int count)
	{
		for (int i = 0; i < cargo.Count; i++) {
			if (cargo[i].definition.id == def.id) {
				GiveMaxAllowed(cargo[i], count);
				return;
			}
		}

		ItemInstance inst = new ItemInstance(def);
		GiveMaxAllowed(inst, count);
		cargo.Add(inst);

		if (shipInfoWindow != null) {
			shipInfoWindow.AddNewItem(inst);
		}
	}

	public void GiveMaxAllowed(ItemInstance inst, int countGiving)
	{
		float weightCurrent = CurrentStorageTons();
		float weightGiving = countGiving * inst.definition.weightTons;
		if (weightCurrent + weightGiving <= def.storageTons) {
			inst.count += countGiving;
		} else {
			float weightExtra = (weightCurrent + weightGiving) - def.storageTons;
			int countExtra = (int)(weightExtra / inst.definition.weightTons);
			int newCountGiving = countGiving - countExtra;

			// handle various roundings
			if ((newCountGiving * inst.definition.weightTons) + weightCurrent > def.storageTons) {
				newCountGiving--;
			}
			inst.count += newCountGiving;
		}
	}

	public void SetTargetPosition(Vector2 unityPos)
	{
		hasTarget = true;
		UniversalPosition.UnityToUniverse(targetPosition, unityPos);
	}

	public float CurrentStorageTons()
	{
		float ret = 0;
		for (int i = 0; i < cargo.Count; i++) {
			ret += cargo[i].GetWeightTons();
		}
		return ret;
	}

	public float TotalMass()
	{
		return def.massTons + CurrentStorageTons();
	}

	public void Step(float time)
	{
		// Ship movement
		if (hasTarget) {
			hasTarget = SimulateMovement(ref physics, def, TotalMass(), targetPosition, time);
		}

		// Update modules
		for (int i = 0; i < modules.Count; i++) {
			modules[i].Step(time);
		}
	}

	// Returns if at target
	public static bool SimulateMovement(ref Physics physics, ShipDefinition def, float mass, UniversalPosition targetPosition, float time)
	{
		float gallonsUsed = def.fuelRateGallonsPerMinute;
		physics.fuelGallons -= gallonsUsed;

		// how much push the fuel provides
		float fuelForce = gallonsUsed * fuelForcePerGallon;

		float maxAcceleration = (1 / mass) * fuelForce;

		float maxStepsNeededToStop = physics.velocity.magnitude / maxAcceleration;
		float distToTarget = Vector2.Distance(physics.pos.Get(), targetPosition.Get());
		float stepsToTarget = distToTarget / physics.velocity.magnitude;

		// close enough
		if (distToTarget < 0.01f) {
			physics.velocity = new Vector2(0, 0);
			return false;
		}

		Vector2 force = new Vector2();
		if (maxStepsNeededToStop > stepsToTarget) {
			// slow down, apply opposite force
			force = physics.velocity.normalized * -1 * fuelForce;
		} else if (maxStepsNeededToStop > stepsToTarget - 2) {
			// coast, apply no force
			force = new Vector2(0, 0);
		} else {
			// push towards target
			force = (targetPosition.Get() - physics.pos.Get()).normalized * fuelForce;
		}

		//Debug.DrawRay(pos.UniverseToUnity(), force, Color.red, 0.1f);
		//Debug.DrawRay(pos.UniverseToUnity(), velocity, Color.green, 0.1f);

		Vector2 acceleration = force / mass;
		physics.velocity = physics.velocity + acceleration;

		Vector3 newPos = physics.pos.Get() + (physics.velocity * time);
		physics.pos.Set(newPos);

		return true;
	}
}