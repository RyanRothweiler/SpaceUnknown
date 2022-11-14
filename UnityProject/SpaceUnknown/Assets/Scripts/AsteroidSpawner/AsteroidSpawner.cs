using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AsteroidSpawner : MonoBehaviour, IActor
{
	public List<GameObject> fabs;

	public float realtimeMinutesUntilAsteroid;
	public float randomWindow;

	private UniversalPositionApply pos;

	private class SpawnPosition
	{
		public UniversalPositionApply pos;
		public GameObject currentObj;
	};

	private List<SpawnPosition> allPositions = new List<SpawnPosition>();

	private long nextStepSpawn;

	private const string poolID = "asteroids_spawner";

	void Start()
	{
		GameManager.RegisterActor(this);
		Pooler.Setup(poolID, fabs, 0, null);

		pos = this.GetComponent<UniversalPositionApply>();

		UniversalPositionApply[] comps = this.GetComponentsInChildren<UniversalPositionApply>();
		for (int x = 0; x < comps.Length; x++) {
			SpawnPosition sp = new SpawnPosition();
			sp.pos = comps[x];
			allPositions.Add(sp);
		}

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
		// create list of available positions
		List<SpawnPosition> availablePositions = new List<SpawnPosition>();
		for (int i = 0; i < allPositions.Count; i++) {
			if (allPositions[i].currentObj == null) {
				availablePositions.Add(allPositions[i]);
			}
		}

		// pick one position randomly to spawn on
		if (availablePositions.Count > 0) {
			int choice = UnityEngine.Random.Range(0, availablePositions.Count - 1);
			UniversalPositionApply destMono = availablePositions[choice].pos;

			GameObject roid = Pooler.Get(poolID);
			availablePositions[choice].currentObj = roid;

			roid.transform.localEulerAngles = new Vector3(0, 0, UnityEngine.Random.Range(0, 360));
			UniversalPosition roidPos = roid.GetComponent<UniversalPositionApply>().univPos;

			roidPos.x = destMono.univPos.x;
			roidPos.y = destMono.univPos.y;

			roid.GetComponent<AsteroidInstance>().contentsAmount = 2;
		}
	}

#if UNITY_EDITOR
	private void SpawnAll()
	{
		for (int x = 0; x < allPositions.Count; x++) {
			Spawn();
		}
	}
#endif
}