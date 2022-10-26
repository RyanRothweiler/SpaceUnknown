using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
	public static GameManager Instance;

	private static List<IActor> simulationActors = new List<IActor>();
	public static void RegisterActor(IActor actor) { simulationActors.Add(actor); }

	public List<Mineable> mineables = new List<Mineable>();

	void Awake()
	{
		Instance = this;

		Application.targetFrameRate = 60;
		Input.simulateMouseWithTouches = false;
	}

	void Update()
	{
		for (int i = 0; i < simulationActors.Count; i++) {
			simulationActors[i].Step(1.0f);
		}
	}
}
