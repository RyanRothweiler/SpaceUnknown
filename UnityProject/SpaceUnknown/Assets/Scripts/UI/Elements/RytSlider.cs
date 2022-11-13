using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;
using UnityEngine.Events;

public class RytSlider : MonoBehaviour
{
	public Slider slider;
	public TMP_Text title;
	public TMP_Text data;

	public UnityEvent OnsValueChanged;

	public void SetValue(float value)
	{
		slider.value = value;
		data.text = RytUtil.HumanFloat(value);
	}

	public void ValueChanged()
	{
		int val = (int)(slider.value * 100.0f);
		data.text = val + "%";
		OnsValueChanged.Invoke();
	}
}