using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 1 unit = 1000 miles

public class Ship : MonoBehaviour
{
	public static Ship shipSelected;

	private bool hasTarget;
	private UniversalPosition targetPosition;

	private LineRenderer pathLine;

	// 0 means no conservation, boost all the way until counter boost.
	//private float fuelConservation;

	private float massTons = 2250.0f;
	public Vector2 velocity;

	private UniversalPosition uniPos;

	private List<float> currentFlightPlanForce;

	public float maxStepsNeededToStop;
	public float distToTarget;
	public float stepsToTarget;
	public Vector2 force;

	public void Awake()
	{
		uniPos = this.GetComponent<UniversalPosition>();

		pathLine = this.GetComponent<LineRenderer>();
		pathLine.positionCount = 0;
	}

	void Update()
	{
		if (hasTarget) {
			pathLine.positionCount = 2;

			Vector3[] positions = new Vector3[2];
			positions[0] = this.transform.position;
			positions[1] = targetPosition.UniverseToUnity();
			pathLine.SetPositions(positions);

			Step();
		} else {
			pathLine.positionCount = 0;
		}
	}

	public void SetTargetPosition(Vector2 unityPos)
	{
		hasTarget = true;
		targetPosition = UniversalPosition.UnityToUniverse(unityPos);

		/*
		// how much push the fuel provides
		float fuelForce = 1.0f;

		int flightWorldMinutes = 120;
		currentFlightPlanForce = new List<float>();
		float flightDist = Vector2.Distance(uniPos.Get(), targetPos.Get());

		//int minSteps = (int)(flightDist / fuelForce) + 1;

		for (int i = 0; i < flightWorldMinutes; i++) {
		}
		*/
	}

	// one step is one minute game world time
	public void Step()
	{
		// how much push the fuel provides
		float fuelForce = 1.0f;
		float maxAcceleration = (1 / massTons) * fuelForce;

		//
		maxStepsNeededToStop = velocity.magnitude / maxAcceleration;
		distToTarget = Vector2.Distance(uniPos.Get(), targetPosition.Get());
		stepsToTarget = distToTarget / velocity.magnitude;
		//

		// close enough
		if (distToTarget < 0.01f) {
			hasTarget = false;
			velocity = new Vector2(0, 0);
			return;
		}

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

		Vector2 acceleration = force / massTons;
		velocity = velocity + acceleration;

		Vector3 newPos = uniPos.Get() + velocity;
		uniPos.Set(newPos);
	}
}