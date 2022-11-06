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

	private List<GameObject> lastLines = new List<GameObject>();

	private static List<Line> linesToDraw = new List<Line>();
	private static string poolID = "lines";

	void Start()
	{
		linesToDraw = new List<Line>();
		Pooler.Setup(poolID, fab, 0, this.transform);
	}

	void LateUpdate()
	{
		for (int i = 0; i < lastLines.Count; i++) {
			Pooler.Return(lastLines[i]);
		}
		lastLines.Clear();

		for (int i = 0; i < linesToDraw.Count; i++) {
			GameObject lineObj = Pooler.Get(poolID);
			lastLines.Add(lineObj);

			Line line = linesToDraw[i];
			RectTransform rtrans = lineObj.GetComponent<RectTransform>();

			Vector2 center = new Vector2(
			    (line.start.x + line.end.x) * 0.5f,
			    (line.start.y + line.end.y) * 0.5f
			);
			rtrans.anchoredPosition = center;

			Vector2 dir = (line.start - line.end).normalized;
			rtrans.right = dir;

			Vector2 sd = rtrans.sizeDelta;
			sd.x = Vector2.Distance(line.start, line.end);
			rtrans.sizeDelta = sd;

			lineObj.GetComponent<Image>().color = line.color;
		}

		linesToDraw.Clear();
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

	public static void Draw(UniversalPosition start, UniversalPosition end, Color color)
	{
		Vector3 startScreen = Camera.main.WorldToScreenPoint(start.UniverseToUnity());
		Vector3 endScreen = Camera.main.WorldToScreenPoint(end.UniverseToUnity());
		Draw(startScreen, endScreen, color);
	}
}