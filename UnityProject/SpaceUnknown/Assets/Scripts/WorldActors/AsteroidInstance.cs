using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AsteroidInstance : MonoBehaviour
{
	public UniversalPosition pos;

	public void Awake()
	{
		pos = this.GetComponent<UniversalPositionMono>().pos;
	}

	public void OnEnabled()
	{
		GameManager.Instance.asteroids.Add(this);
	}

	public void Remove()
	{
		PoolTag pool = this.GetComponent<PoolTag>();
		if (pool != null) {
			Pooler.Return(this.gameObject);
		} else {
			Destroy(this.gameObject);
		}
	}
}