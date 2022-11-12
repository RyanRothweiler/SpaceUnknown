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

	public Vector2 targetPosition;
	public bool hasTarget;
	public Physics physics;

	public List<ModuleInstance> modules;
	public List<ItemInstance> cargo;

	//private LineRenderer pathLine;

	// 0 means no conservation, boost all the way until counter boost.
	//private float fuelConservation;

	private const float fuelForcePerGallon = 10000.0f;

	private List<float> currentFlightPlanForce;
	public ShipInfoWindow shipInfoWindow;

	public void Awake()
	{
		GameManager.RegisterActor(this);

		physics.pos = this.GetComponent<UniversalPosition>();

		modules = new List<ModuleInstance>();
		modules.Add(new ModuleInstance(testModuleDef, this));

		cargo = new List<ItemInstance>();

		physics.fuelGallons = def.fuelTankGallons;
	}

	void Update()
	{
		/*
		if (hasTarget) {

		pathLine.positionCount = 2;

		Vector3[] positions = new Vector3[2];
		positions[0] = this.transform.position;
		positions[1] = targetPosition.UniverseToUnity();
		pathLine.SetPositions(positions);

		} else {
		pathLine.positionCount = 0;
		}
		*/

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
		Debug.Log("Setting target position");
		hasTarget = true;

		targetPosition = UniversalPosition.UnityToUniverse(unityPos);

		testSettings.startPos = physics.pos.Get();
		testSettings.distFromSidesToCoast = Vector2.Distance(physics.pos.Get(), targetPosition) * 0.5f * 0.5f;
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
			hasTarget = SimulateMovement(ref physics, def, TotalMass(), targetPosition, time, new Ship.JourneySettings());
		}

		// Update modules
		for (int i = 0; i < modules.Count; i++) {
			modules[i].Step(time);
		}
	}

	// Efficiency of 1.0 means using fuel for the entirety of the journey. 0.0 means use fuel for only 10% of the journey, 5% speeding up, 5% slowingdown.
	public struct JourneySettings {
		public Vector3 startPos;
		public float distFromSidesToCoast;
	};

	static JourneySettings testSettings;

	// Returns if is still moving
	public static bool SimulateMovement(ref Physics physics, ShipDefinition def, float mass, Vector2 targetPosition, float time, JourneySettings settings)
	{
		float fuelToUse = def.fuelRateGallonsPerSecond * time;
		float fuelForce = fuelToUse * fuelForcePerGallon;

		// close enough
		if (Vector2.Distance(physics.pos.Get(), targetPosition) < 0.01f) {
			physics.velocity = new Vector2(0, 0);
			return false;
		}

		Vector2 force = new Vector2();

		// Speed up
		if (Vector2.Distance(physics.pos.Get(), testSettings.startPos) < testSettings.distFromSidesToCoast) {
			physics.fuelGallons -= fuelToUse;
			force = (targetPosition - physics.pos.Get()).normalized * fuelForce;
		}

		// Slow down
		if (Vector2.Distance(physics.pos.Get(), targetPosition) < testSettings.distFromSidesToCoast) {
			physics.fuelGallons -= fuelToUse;
			force = (targetPosition - physics.pos.Get()).normalized * fuelForce * -1;

			// slow enough
			if (physics.velocity.magnitude < 0.1f) {
				physics.velocity = new Vector2(0, 0);
				return false;
			}
		}

		Vector2 acceleration = force / mass;
		physics.velocity = physics.velocity + acceleration;

		Vector3 newPos = physics.pos.Get() + (physics.velocity * time);
		physics.pos.Set(newPos);

		return true;
	}
}