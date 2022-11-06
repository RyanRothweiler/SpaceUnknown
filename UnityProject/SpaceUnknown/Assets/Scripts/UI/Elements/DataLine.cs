using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class DataLine : MonoBehaviour
{
	public TMP_Text title;
	public TMP_Text data;

	public void Set(string value, string units)
	{
		data.text = $"{value}({units})";
	}

}