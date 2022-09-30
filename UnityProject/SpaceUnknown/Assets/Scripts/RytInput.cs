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
	};

	public static Touch[] touches;
	public static float scrollAmount;

	private float pcScrollRate = 2.0f;

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
		if (touch.onDown) {
			touch.prevPos = position;
		}
		if (touch.isDown) {
			touch.screenDelta = touch.prevPos - position;
			touch.prevPos = position;
		}
	}
}