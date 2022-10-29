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
		//StepAll();

#if UNITY_EDITOR
		// time skip editing tools. These times are in real world time
		{
			int stepsToDo = 0;

			int oneMinuteOfSteps = 60 * 60;

			// one real world minute
			if (Input.GetKeyDown(KeyCode.Alpha1)) {
				stepsToDo = oneMinuteOfSteps;
			}

			// 15 minutes
			if (Input.GetKeyDown(KeyCode.Alpha2)) {
				stepsToDo = oneMinuteOfSteps * 15;
			}

			// one hour
			if (Input.GetKeyDown(KeyCode.Alpha3)) {
				stepsToDo = oneMinuteOfSteps * 60;
			}

			// one day
			if (Input.GetKeyDown(KeyCode.Alpha4)) {
				stepsToDo = oneMinuteOfSteps * 60 * 24;
			}

			for (int i = 0; i < stepsToDo; i++) {
				StepAll();
			}
		}
#endif
	}

	public void StepAll()
	{
		// One step is one minute of world time
		for (int i = 0; i < simulationActors.Count; i++) {
			simulationActors[i].Step(1.0f);
		}
	}
}
