using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AsteroidInstance : MonoBehaviour
{
	public UniversalPosition pos;

	public int contentsAmount;

	public void Awake()
	{
		pos = this.GetComponent<UniversalPositionMono>().pos;
	}

	public void OnEnable()
	{
		GameManager.Instance.asteroids.Add(this);
	}

	public int Withdraw(int amount)
	{
		if (contentsAmount - amount > 0) {
			contentsAmount -= amount;
			return amount;
		} else {
			int ret = contentsAmount;
			contentsAmount = 0;
			Remove();
			return ret;
		}
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