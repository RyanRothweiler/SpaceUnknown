using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ship : MonoBehaviour, IActor
{
	public ItemDefinition testDef;
	public ShipInfoWindow shipInfoWindow;

	private bool hasTarget;
	private UniversalPosition targetPosition;

	public List<Module> modules;
	public List<ItemInstance> cargo;

	private LineRenderer pathLine;

	// 0 means no conservation, boost all the way until counter boost.
	//private float fuelConservation;

	private float massTons = 2250.0f;
	public Vector2 velocity;

	private UniversalPosition uniPos;

	private List<float> currentFlightPlanForce;

	public float storageTons = 1000;

	public void Awake()
	{
		GameManager.RegisterActor(this);

		uniPos = this.GetComponent<UniversalPosition>();

		pathLine = this.GetComponent<LineRenderer>();
		pathLine.positionCount = 0;

		modules = new List<Module>();
		modules.Add(new Module());

		cargo = new List<ItemInstance>();
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

		if (Input.GetKeyDown(KeyCode.I)) {
			GiveItem(testDef, 80);
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

		shipInfoWindow.AddNewItem(inst);
	}

	public void GiveMaxAllowed(ItemInstance inst, int countGiving)
	{
		float weightCurrent = CurrentStorageTons();
		float weightGiving = countGiving * inst.definition.weightTons;
		if (weightCurrent + weightGiving <= storageTons) {
			inst.count += countGiving;
		} else {
			float weightExtra = (weightCurrent + weightGiving) - storageTons;
			int countExtra = (int)(weightExtra / inst.definition.weightTons);
			int newCountGiving = countGiving - countExtra;

			// handle various roundings
			if ((newCountGiving * inst.definition.weightTons) + weightCurrent > storageTons) {
				newCountGiving--;
			}
			inst.count += newCountGiving;
		}
	}

	public void SetTargetPosition(Vector2 unityPos)
	{
		hasTarget = true;
		targetPosition = UniversalPosition.UnityToUniverse(unityPos);
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
		return massTons + CurrentStorageTons();
	}

	// one step is one minute game world time
	public void Step(float time)
	{
		// Ship movement
		if (hasTarget) {
			// how much push the fuel provides
			float fuelForce = 1.0f;
			float maxAcceleration = (1 / TotalMass()) * fuelForce;

			//
			float maxStepsNeededToStop = velocity.magnitude / maxAcceleration;
			float distToTarget = Vector2.Distance(uniPos.Get(), targetPosition.Get());
			float stepsToTarget = distToTarget / velocity.magnitude;
			//

			// close enough
			if (distToTarget < 0.01f) {
				hasTarget = false;
				velocity = new Vector2(0, 0);
				return;
			}

			Vector2 force = new Vector2();
			if (maxStepsNeededToStop > stepsToTarget) {
				// slow down, apply opposite force
				force = velocity.normalized * -1 * fuelForce;
			} else if (maxStepsNeededToStop > stepsToTarget - 2) {
				// coast, apply no force
				force = new Vector2(0, 0);
			} else {
				// push towards target
				force = (targetPosition.Get() - uniPos.Get()).normalized * fuelForce;
			}

			//Debug.DrawRay(uniPos.UniverseToUnity(), force, Color.red, 0.1f);
			//Debug.DrawRay(uniPos.UniverseToUnity(), velocity, Color.green, 0.1f);

			Vector2 acceleration = force / TotalMass();
			velocity = velocity + acceleration;

			Vector3 newPos = uniPos.Get() + velocity;
			uniPos.Set(newPos);
		}

		// Update modules
		{
			for (int i = 0; i < modules.Count; i++) {
				modules[i].Step(time);
			}
		}
	}
}