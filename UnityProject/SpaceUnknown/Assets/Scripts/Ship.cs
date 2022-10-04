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
	private Vector2 velocity;

	// amount of siulation time for one world step
	private float timeStepsMinutes = 1.0f;

	private UniversalPosition uniPos;

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
	}

	public void Step()
	{
		// how much push the fuel provides
		float fuelForce = 1.0f;

		float stepsNeededToStop = velocity.magnitude / fuelForce;
		//float distToTarget = Vector3.Distance(uniPos.Get());
		//float stepsToTarget = velocity.magnitude / V

		Vector2 force = (targetPosition.Get() - uniPos.Get()).normalized * fuelForce;
		Vector2 acceleration = force / massTons;

		velocity = velocity + acceleration;
		Vector2 distMoved = velocity * timeStepsMinutes;

		Vector3 newPos = uniPos.Get() + distMoved;
		uniPos.Set(newPos);
	}
}