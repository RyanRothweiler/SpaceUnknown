using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class TimeDisplay : MonoBehaviour, IActor
{
	public TMP_Text timeDisplay;

	private float minutes;
	private float hours;
	private float days;
	private float years;

	public void Start()
	{
		GameManager.RegisterActor(this);
	}

	void Update()
	{
		timeDisplay.text = $"{(int)years}(y) {(int)days}(d) {(int)hours}(h) {(int)minutes}(m)";
	}

	public void Step(float stepTime)
	{
		minutes += stepTime;
		if (minutes >= 60.0f) {
			minutes -= 60.0f;
			hours++;
		}
		if (hours >= 24.0f) {
			hours = 0;
			days++;
		}
		if (days > 360) {
			days = 1;
			years++;
		}
	}
}