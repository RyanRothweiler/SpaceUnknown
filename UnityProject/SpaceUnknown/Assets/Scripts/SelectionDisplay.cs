using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SelectionDisplay : MonoBehaviour
{
	public static SelectionDisplay instance;

	void Awake()
	{
		instance = this;
		//this.gameObject.SetActive(false);
	}

}
