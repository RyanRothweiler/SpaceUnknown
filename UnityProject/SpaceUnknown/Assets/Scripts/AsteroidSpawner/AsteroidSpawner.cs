using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AsteroidSpawner : MonoBehaviour, IActor
{
	public List<GameObject> fabs;

	public float realtimeMinutesUntilAsteroid;
	public float randomWindow;

	private UniversalPositionMono mpos;

	private List<UniversalPositionMono> allPositions = new List<UniversalPositionMono>();
	private List<UniversalPositionMono> available = new List<UniversalPositionMono>();

	private long nextStepSpawn;

	private const string poolID = "asteroids_spawner";

	void Start()
	{
		GameManager.RegisterActor(this);
		Pooler.Setup(poolID, fabs, 0, null);

		mpos = this.GetComponent<UniversalPositionMono>();

		UniversalPositionMono[] comps = this.GetComponentsInChildren<UniversalPositionMono>();
		allPositions.AddRange(comps);
		for (int x = 0; x < allPositions.Count; x++) {
			available.Add(allPositions[x]);
		}

		/*
		for (int x = 0; x < allPositions.Count; x++) {
			Spawn();
		}
		*/

		SetNextStepTime();
	}

	public void Step(float time)
	{
		if (GameManager.StepTime() == nextStepSpawn) {
			Spawn();
			SetNextStepTime();
		}
	}

	private void SetNextStepTime()
	{
		nextStepSpawn = GameManager.StepTime() + GameManager.RealtimeMinutesToSteps(realtimeMinutesUntilAsteroid);
	}

	private void Spawn()
	{
		if (available.Count > 0) {
			int choice = UnityEngine.Random.Range(0, available.Count - 1);
			UniversalPositionMono destMono = available[choice];
			available.RemoveAt(choice);

			GameObject roid = Pooler.Get(poolID);
			roid.transform.localEulerAngles = new Vector3(0, 0, UnityEngine.Random.Range(0, 360));
			UniversalPosition roidPos = roid.GetComponent<UniversalPositionMono>().pos;

			roidPos.x = destMono.pos.x;
			roidPos.y = destMono.pos.y;
		}
	}
}