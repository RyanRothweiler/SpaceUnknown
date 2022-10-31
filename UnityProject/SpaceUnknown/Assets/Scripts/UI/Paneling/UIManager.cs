using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager : MonoBehaviour
{
	public static UIManager Instance;

	public List<PanelDefinition> allPanels;
	public GameObject safeArea;
	public GameObject edgeGradientFab;
	public ThemeColorStyle selectionLineColor;

	private class PanelInstance
	{
		public PanelDefinition def;
		public GameObject inst;
		public GameObject edgeGradient;
	};

	private Dictionary<PanelDefinition.ID, PanelInstance> panels = new Dictionary<PanelDefinition.ID, PanelInstance>();
	private List<PanelInstance> breadcrumb = new List<PanelInstance>();

	private const string edgeGradientKey = "EdgeGradient";
	private const float edgePanelLayerOffset = 130;

	void Start()
	{
		Instance = this;

		Pooler.Setup(edgeGradientKey, edgeGradientFab, 0, safeArea.transform);

		for (int i = 0; i < allPanels.Count; i++) {
			PanelDefinition panelDef = allPanels[i];
			Debug.Assert(!panels.ContainsKey(panelDef.id));

			PanelInstance pi = new PanelInstance();
			pi.def = panelDef;
			panels[panelDef.id] = pi;
		}
	}

	public GameObject ShowPanel(PanelDefinition.ID panelID)
	{
		Debug.Assert(panels.ContainsKey(panelID));
		Debug.Log("PM: Showing panel " + panelID);

		PanelInstance pi = panels[panelID];
		if (pi.inst == null) {
			pi.inst = Instantiate(pi.def.fab, safeArea.transform);
		}
		pi.inst.SetActive(true);
		breadcrumb.Add(pi);

		// edge offset
		RectTransform rtrans = pi.inst.GetComponent<RectTransform>();
		Vector2 pos = rtrans.anchoredPosition;
		pos.x = (breadcrumb.Count - 1) * edgePanelLayerOffset;
		rtrans.anchoredPosition = pos;

		pi.edgeGradient = Pooler.Get(edgeGradientKey);
		RectTransform edgeTrans = pi.edgeGradient.GetComponent<RectTransform>();
		edgeTrans.pivot = new Vector2(1.0f, 0.5f);
		edgeTrans.anchoredPosition = rtrans.anchoredPosition;

		return pi.inst;
	}

	public void BackOutAll()
	{
		for (int i = 0 ; i < breadcrumb.Count; i++) {
			Back();
		}
	}

	public void Back()
	{
		if (breadcrumb.Count > 0) {
			Debug.Log("PM: Back");

			PanelInstance current = breadcrumb[breadcrumb.Count - 1];
			current.inst.SetActive(false);
			if (current.edgeGradient != null) {
				Pooler.Return(current.edgeGradient);
			}

			breadcrumb.RemoveAt(breadcrumb.Count - 1);

			if (breadcrumb.Count > 0) {
				PanelInstance next = breadcrumb[breadcrumb.Count - 1];
				next.inst.SetActive(true);
			}
		}
	}

	public void DrawSelectionLine(GameObject target)
	{
		RectTransform windowTrans = breadcrumb[breadcrumb.Count - 1].inst.GetComponent<RectTransform>();
		Vector3[] corners = new Vector3[4];
		windowTrans.GetWorldCorners(corners);

		Vector3 screenPoint = Camera.main.WorldToScreenPoint(corners[2]);
		Vector2 start = new Vector2(screenPoint.x, screenPoint.y);

		Vector3 objScreenPoint = Camera.main.WorldToScreenPoint(target.transform.position);
		Vector2 end = new Vector2(objScreenPoint.x, objScreenPoint.y);

		DrawScreenLine.Draw(start, end, selectionLineColor.color);
	}
}