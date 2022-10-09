using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ProgressBarText : MonoBehaviour
{
	public ProgressBar progressBar;
	public TMP_Text text;

	public void Set(float current, float max, string units)
	{
		progressBar.val = current / max;
		text.text = string.Format("{0} / {1} ({2})", current, max, units);
	}
}
