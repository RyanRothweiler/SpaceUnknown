using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Mineable : MonoBehaviour
{
	public UniversalPosition pos;

	public void Awake()
	{
		pos = this.GetComponent<UniversalPositionMono>().pos;
	}

	public void Start()
	{
		GameManager.Instance.mineables.Add(this);
	}
}