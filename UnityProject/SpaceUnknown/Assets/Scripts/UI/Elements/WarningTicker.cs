using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class WarningTicker : MonoBehaviour
{
	public TMP_Text text;

	public void Display(string text)
	{
		this.text.text = text;
		this.gameObject.SetActive(true);
	}

	public void Clear()
	{
		this.gameObject.SetActive(false);
	}
}