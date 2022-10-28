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

	public void Back()
	{
		if (breadcrumb.Count > 0) {
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
}