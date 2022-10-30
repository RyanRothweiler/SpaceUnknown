using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
	public static GameManager Instance;

	private static List<IActor> simulationActors = new List<IActor>();
	public static void RegisterActor(IActor actor) { simulationActors.Add(actor); }

	// starts at 0, incremented for each game step
	private static long stepTime;
	public static long StepTime() { return stepTime; }

	public List<Mineable> mineables = new List<Mineable>();

	void Awake()
	{
		Instance = this;

		Application.targetFrameRate = 60;
		Input.simulateMouseWithTouches = false;
	}

	void Update()
	{
		StepAll();

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
		stepTime++;

		// One step is one minute of world time
		for (int i = 0; i < simulationActors.Count; i++) {
			simulationActors[i].Step(1.0f);
		}
	}

	public static long RealtimeMinutesToSteps(float realtimeMinutes)
	{
		// running at 1 step is one frame. so 60 frames per second * 60 seconds in a minute
		return (long)(realtimeMinutes * 60.0f * 60.0f);
	}
}
