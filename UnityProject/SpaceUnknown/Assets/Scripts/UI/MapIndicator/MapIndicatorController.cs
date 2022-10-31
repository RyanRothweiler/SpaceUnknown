using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MapIndicatorController : MonoBehaviour
{
	public static MapIndicatorController Instance;

	public MapIndicator mapIndicatorFab;

	private RectTransform rootCanvasRect;
	private Dictionary<UniversalPositionMono, RectTransform> createdIndicator = new Dictionary<UniversalPositionMono, RectTransform>();

	private const string indicatorPool = "MapIndicator";

	void Awake()
	{
		Instance = this;
		rootCanvasRect = this.GetComponent<RectTransform>();

		Pooler.Setup(indicatorPool, mapIndicatorFab.gameObject, 0, mapIndicatorFab.transform.parent);
	}

	void Update()
	{
		mapIndicatorFab.gameObject.SetActive(false);

		// Create / destroy indicators
		for (int i = 0; i < UniversalPositionMono.allUniversals.Count; i++) {
			UniversalPositionMono uniPos = UniversalPositionMono.allUniversals[i];
			if (uniPos.transform.localScale.x < 0.01f) {
				if (!createdIndicator.ContainsKey(uniPos)) {
					createdIndicator[uniPos] = mapIndicatorFab.gameObject.GetComponent<RectTransform>();
				}
			} else {
				createdIndicator.Remove(uniPos);
			}
		}

		// Update indicator position
		foreach (var item in createdIndicator) {
			Vector2 viewportPos = Camera.main.WorldToViewportPoint(item.Key.gameObject.transform.position);
			Vector2 screenPos = new Vector2(
			    ((viewportPos.x * rootCanvasRect.sizeDelta.x) - (rootCanvasRect.sizeDelta.x * 0.5f)),
			    ((viewportPos.y * rootCanvasRect.sizeDelta.y) - (rootCanvasRect.sizeDelta.y * 0.5f)));

			//item.Value.anchoredPosition = screenPos;
			mapIndicatorFab.gameObject.SetActive(true);
			DrawScreenLine.Draw(item.Value.anchoredPosition, screenPos);
		}
	}
}