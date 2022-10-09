using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ProgressBar : MonoBehaviour
{
	[Range(0, 1)]
	public float val;
	private RectTransform container;
	public RectTransform bar;

	void Start()
	{
		container = this.GetComponent<RectTransform>();
	}

	void Update()
	{
		Vector2 offsetMax = bar.offsetMax;
		offsetMax.x = (-container.rect.width) + (container.rect.width * val);
		bar.offsetMax = offsetMax;
	}
}
