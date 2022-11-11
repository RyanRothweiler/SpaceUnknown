using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class RytSlider : MonoBehaviour
{
	public Slider slider;
	public TMP_Text title;
	public TMP_Text data;

	public void SetValue(float value)
	{
		slider.value = value;
		data.text = RytUtil.HumanFloat(value);
	}
}