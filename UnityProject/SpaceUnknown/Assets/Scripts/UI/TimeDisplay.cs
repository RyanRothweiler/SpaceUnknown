using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class TimeDisplay : MonoBehaviour, IActor
{
	public TMP_Text timeDisplay;

	public float seconds;

	public void Start()
	{
		GameManager.RegisterActor(this);
	}

	public void Step(float stepTime)
	{
		seconds += stepTime;
		TimeSpan span = new TimeSpan(0, 0, (int)seconds);
		timeDisplay.text = span.ToString(@"dd\:hh\:mm\:ss");
	}
}