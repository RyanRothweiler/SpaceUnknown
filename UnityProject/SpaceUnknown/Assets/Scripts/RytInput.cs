using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class RytInput : MonoBehaviour
{
	public class Touch
	{
		public bool isDown;
		public bool onDown;
		public bool isUp;
		public bool onUp;
		public bool moved;

		public bool startedOverUI;

		public Vector2 screenDelta;
		public Vector2 prevPos;
		public Vector2 startPos;
		public Vector2 currentPos;
	};

	//public static Touch[] touches;
	public static float scrollAmount;

	public static Touch leftTouch;
	public static Touch rightTouch;

	private Ship shipSelected;

	private float pcScrollRate = 2.0f;
	private GameObject shipInfoWindow;

	private const float distForMove = 5.0f;

	void Start()
	{
		//touches = new Touch[1];
		//touches[0] = new Touch();

		leftTouch = new Touch();
		rightTouch = new Touch();
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
			//Touch rytTouch = touches[0];

			UpdateTouchStates(leftTouch, Input.GetMouseButton(0), Input.mousePosition);
			UpdateTouchStates(rightTouch, Input.GetMouseButton(1), Input.mousePosition);

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

		// selection
		{
			if (leftTouch.onUp) {

				if (!leftTouch.moved) {
					shipSelected = null;

					Vector3 worldPos = Camera.main.ScreenToWorldPoint(new Vector3(leftTouch.currentPos.x, leftTouch.currentPos.y, Camera.main.transform.position.z * -1));

					Vector3 origin = Camera.main.transform.position;
					Vector3 dir = (worldPos - Camera.main.transform.position).normalized;
					RaycastHit hit;

					if (Physics.Raycast(origin, dir, out hit)) {

						GameObject hitObj = hit.collider.gameObject;
						Ship hitShip = hitObj.GetComponent<Ship>();

						if (hitShip != null) {
							shipSelected = hitShip;

							SelectionDisplay.instance.gameObject.SetActive(true);
							SelectionDisplay.instance.transform.position = hitObj.transform.position;

							shipInfoWindow = UIManager.Instance.ShowPanel(PanelDefinition.ID.ShipInfo);

							ShipInfoWindow siw = shipInfoWindow.GetComponent<ShipInfoWindow>();
							siw.Show(shipSelected);
							shipSelected.shipInfoWindow = siw;
						}
					}
				}

				// selection dispaly
				if (shipSelected != null) {
					SelectionDisplay.instance.gameObject.SetActive(true);
					SelectionDisplay.instance.transform.position = shipSelected.transform.position;
				} else {
					UIManager.Instance.Back();
					SelectionDisplay.instance.gameObject.SetActive(false);
				}
			}

			// ship movement
			if (shipSelected != null && rightTouch.onUp && !rightTouch.moved) {
				Vector3 worldPos = Camera.main.ScreenToWorldPoint(new Vector3(leftTouch.currentPos.x, leftTouch.currentPos.y, Camera.main.transform.position.z * -1));
				shipSelected.SetTargetPosition(worldPos);
			}
		}
	}

	private void UpdateTouchStates(Touch touch, bool newState, Vector2 position)
	{
		if (EventSystem.current.IsPointerOverGameObject()) {
			touch.startedOverUI = true;
		}
		if (!newState) {
			touch.startedOverUI = false;
		}


		// Don't process anything if we were over UI
		if (!touch.startedOverUI) {

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
				touch.startedOverUI = false;

				if (touch.isUp) {
					touch.moved = false;
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
}