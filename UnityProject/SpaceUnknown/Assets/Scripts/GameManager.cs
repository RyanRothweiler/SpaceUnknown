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

	public List<AsteroidInstance> asteroids = new List<AsteroidInstance>();

	void Awake()
	{
		Instance = this;

		Application.targetFrameRate = 60;
		Input.simulateMouseWithTouches = false;
	}

	void Update()
	{
		StepAll(Time.deltaTime);

#if UNITY_EDITOR

		// framerate testing
		{
			if (Input.GetKeyDown(KeyCode.O)) {
				Debug.Log("Setting fps to 30");
				Application.targetFrameRate = 30;
			}

			if (Input.GetKeyDown(KeyCode.P)) {
				Debug.Log("Setting fps to 60");
				Application.targetFrameRate = 60;
			}
		}

		// time skip editing tools
		{
			int stepsToDo = 0;

			int oneMinuteOfSteps = Application.targetFrameRate * 60;

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
				StepAll(1.0f / Application.targetFrameRate);
			}
		}
#endif
	}

	public void StepAll(float deltaTime)
	{
		stepTime++;

		// One step is one minute of world time
		for (int i = 0; i < simulationActors.Count; i++) {
			simulationActors[i].Step(deltaTime);
		}
	}

	public static long RealtimeMinutesToSteps(float realtimeMinutes)
	{
		// running at 1 step is one frame. so 60 frames per second * 60 seconds in a minute
		return (long)(realtimeMinutes * 60.0f * 60.0f);
	}
}
