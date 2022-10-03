using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 1 unit = 1000 miles

public class Ship : MonoBehaviour
{
	public static Ship shipSelected;

	private bool hasTarget;
	private Vector2 targetPosition;

	private LineRenderer pathLine;

	// 0 means no conservation, boost all the way until counter boost.
	//private float fuelConservation;

	// this is in tons
	private float mass = 2250.0f;
	private Vector2 velocity;

	// amount of siulation time for one world step
	private float timeStepsMinutes = 1.0f;

	public void Awake()
	{
		pathLine = this.GetComponent<LineRenderer>();
		pathLine.positionCount = 0;
	}

	void Update()
	{
		if (hasTarget) {
			pathLine.positionCount = 2;

			Vector3[] positions = new Vector3[2];
			positions[0] = this.transform.position;
			positions[1] = new Vector3(targetPosition.x, targetPosition.y, 0);
			pathLine.SetPositions(positions);

			Step();
		} else {
			pathLine.positionCount = 0;
		}
	}


	public void SetTargetPosition(Vector2 pos)
	{
		hasTarget = true;
		targetPosition = pos;
	}

	public void Step()
	{
		Vector2 currentPos = new Vector2(this.transform.position.x, this.transform.position.y);
		Vector2 force = (targetPosition - currentPos).normalized * 0.1f;

		velocity = velocity + force;
		Vector2 distMoved = velocity * timeStepsMinutes;

		this.transform.position = currentPos + distMoved;
	}

	/*
	private double PhysicsCalc(double current, double velocity)
	{
		float time = 0.1;

		current = velocity + accele;


		return current;
	}
	*/

	/*
	void OnMouseUp()
	{
		if (!RytInput.touches[0].moved) {
			SelectionDisplay.instance.gameObject.SetActive(true);
			SelectionDisplay.instance.transform.position = this.transform.position;
		}
	}
	*/
}
