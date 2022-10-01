using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ship : MonoBehaviour
{
	public static Ship shipSelected;

	private bool hasTarget;
	private Vector2 targetPosition;

	private LineRenderer pathLine;

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

	}

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
