using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DrawWorldLine : MonoBehaviour
{
	public GameObject fab;

	public class Line
	{
		public Vector2 start, end;
		public Color color;
		public GameObject obj;
	};

	private static string poolID = "WorldLines";
	private static List<Line> linesToDraw = new List<Line>();

	public void Start()
	{
		Pooler.Setup(poolID, fab, 0, this.transform);
	}

	void Update()
	{
		for (int i = 0; i < linesToDraw.Count; i++) {
			Pooler.Return(linesToDraw[i].obj);
		}

		linesToDraw = new List<Line>();
	}

	void LateUpdate()
	{
		fab.SetActive(false);

		for (int i = 0; i < linesToDraw.Count; i++) {
			Line lineData = linesToDraw[i];
			lineData.obj = Pooler.Get(poolID);
			LineRenderer lineRender = lineData.obj.GetComponent<LineRenderer>();

			Vector3[] points = new Vector3[2];
			points[0] = new Vector3(lineData.start.x, lineData.start.y, 0);
			points[1] = new Vector3(lineData.end.x, lineData.end.y, 0);
			lineRender.SetPositions(points);

			lineRender.startColor = lineData.color;
			lineRender.endColor = lineData.color;
		}
	}

	public static void Draw(Vector2 start, Vector2 end, Color color)
	{
		Line line = new Line();
		line.start = start;
		line.end = end;
		line.color = color;
		linesToDraw.Add(line);
	}
}