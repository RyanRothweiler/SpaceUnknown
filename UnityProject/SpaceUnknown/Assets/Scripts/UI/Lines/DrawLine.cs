using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DrawLine : MonoBehaviour
{
	public GameObject fab;

	public class Line
	{
		public Vector2 start, end;
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
		}
	}

	public static void Draw(Vector2 start, Vector2 end)
	{
		Line line = new Line();
		line.start = start;
		line.end = end;
		linesToDraw.Add(line);
	}
}