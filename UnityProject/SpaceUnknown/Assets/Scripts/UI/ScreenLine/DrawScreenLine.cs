using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DrawScreenLine : MonoBehaviour
{
	public GameObject fab;

	public class Line
	{
		public Vector2 start, end;
		public Color color;
	};

	private static List<Line> linesToDraw = new List<Line>();

	void Update()
	{
		linesToDraw = new List<Line>();
	}

	void LateUpdate()
	{
		fab.SetActive(false);

		for (int i = 0; i < linesToDraw.Count; i++) {
			fab.SetActive(true);

			Line line = linesToDraw[i];
			RectTransform rtrans = fab.GetComponent<RectTransform>();

			Vector2 center = new Vector2(
			    (line.start.x + line.end.x) * 0.5f,
			    (line.start.y + line.end.y) * 0.5f
			);
			rtrans.anchoredPosition = center;

			Vector2 dir = (line.start - line.end).normalized;
			rtrans.right = dir;

			Vector2 sd = rtrans.sizeDelta;
			sd.x = Vector2.Distance(line.start, line.end);;
			rtrans.sizeDelta = sd;

			fab.GetComponent<Image>().color = line.color;
		}
	}

	// 0,0 is bottom left of screen, Screen.Width,Screen.Height is top right
	public static void Draw(Vector2 start, Vector2 end, Color color)
	{
		Vector2 halfScreen = new Vector2(Screen.width * 0.5f, Screen.height * 0.5f);

		Line line = new Line();
		line.start = start - halfScreen;
		line.end = end - halfScreen;
		line.color = color;
		linesToDraw.Add(line);
	}
}