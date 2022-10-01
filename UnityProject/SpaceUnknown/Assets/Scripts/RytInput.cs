using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RytInput : MonoBehaviour
{
	public class Touch
	{
		public bool isDown;
		public bool onDown;
		public bool isUp;
		public bool onUp;
		public bool moved;

		public Vector2 screenDelta;
		public Vector2 prevPos;
		public Vector2 startPos;
		public Vector2 currentPos;
	};

	public static Touch[] touches;
	public static float scrollAmount;

	private float pcScrollRate = 2.0f;

	private const float distForMove = 5.0f;

	void Start()
	{
		touches = new Touch[1];
		touches[0] = new Touch();
	}

	void Update()
	{
		// scrolling
		{
			scrollAmount = 0;
			scrollAmount += Input.mouseScrollDelta.y * pcScrollRate;
		}

		// touches
		{
			Touch rytTouch = touches[0];

			UpdateTouchStates(rytTouch, Input.GetMouseButton(0), Input.mousePosition);

			/*
			if (Input.touches.Length > 0) {
				UnityEngine.Touch unityTouch = Input.touches[0];

				rytTouch.O
				if (rytTouch.O) {
				}

				if (rytTouch.state == TouchState.None) {
					rytTouch.state = TouchState.OnDown;
					rytTouch.state = TouchState.IsDown;
				} else if (rytTouch.state == TouchState.OnDown) {
					rytTouch.state = TouchState.IsDown;
				}

			} else {
				//UpdateTouchStates(rytTouch, false);
			}
			*/
		}

		//string output = string.Format("isDown {0} onDown {1} isUp {2} onUp {3} ", touches[0].isDown, touches[0].onDown, touches[0].isUp, touches[0].onUp);
		//string output = string.Format("deltaPos {0} ", touches[0].screenDelta);
		//Debug.Log(output);

		// selection
		{
			if (touches[0].isDown) {

				Vector3 worldPos = Camera.main.ScreenToWorldPoint(new Vector3(touches[0].currentPos.x, touches[0].currentPos.y, Camera.main.transform.position.z * -1));
				//SelectionDisplay.instance.transform.position = worldPos;

				Vector3 origin = Camera.main.transform.position;
				Vector3 dir = (worldPos - Camera.main.transform.position).normalized;
				RaycastHit hit;

				Debug.Log(origin + " - " + worldPos);
				Debug.DrawRay(origin, dir * 1000.0f, Color.red, 0.01f);

				if (Physics.Raycast(origin, dir, out hit)) {

					// If it hits something...
					if (hit.collider != null) {
						//Debug.Break();
						Debug.Log("Hit something " + hit.collider.gameObject.name);
					}
				}
			}
		}
	}

	private void UpdateTouchStates(Touch touch, bool newState, Vector2 position)
	{
		if (newState) {
			if (touch.isDown) {
				touch.onDown = false;
			} else {
				touch.isDown = true;
				touch.onDown = true;
				touch.isUp = false;
				touch.onUp = false;
			}
		} else {
			touch.moved = false;

			if (touch.isUp) {
				touch.onUp = false;
			} else {
				touch.isDown = false;
				touch.onDown = false;
				touch.isUp = true;
				touch.onUp = true;
			}
		}

		touch.screenDelta.x = 0;
		touch.screenDelta.y = 0;
		touch.currentPos = position;
		if (touch.onDown) {
			touch.startPos = position;
			touch.prevPos = position;
		}
		if (touch.isDown) {
			touch.screenDelta = touch.prevPos - position;
			touch.prevPos = position;

			float dist = Vector2.Distance(touch.startPos, position);
			if (dist > distForMove) {
				touch.moved = true;
			}
		}
	}
}