using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager : MonoBehaviour
{
	public static UIManager Instance;

	public List<PanelDefinition> allPanels;
	public GameObject safeArea;

	private class PanelInstance
	{
		public PanelDefinition def;
		public GameObject inst;
	};

	private Dictionary<PanelDefinition.ID, PanelInstance> panels = new Dictionary<PanelDefinition.ID, PanelInstance>();

	void Start()
	{
		Instance = this;

		for (int i = 0; i < allPanels.Count; i++) {
			PanelDefinition panelDef = allPanels[i];
			Debug.Assert(!panels.ContainsKey(panelDef.id));

			PanelInstance inst = new PanelInstance();
			inst.def = panelDef;
			panels[panelDef.id] = inst;
		}
	}

	public void ShowPanel(PanelDefinition.ID panelID)
	{
		Debug.Assert(panels.ContainsKey(panelID));
	}
}