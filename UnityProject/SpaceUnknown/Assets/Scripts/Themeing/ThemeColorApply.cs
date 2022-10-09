using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

[ExecuteInEditMode]
public class ThemeColorApply : MonoBehaviour
{
	public ThemeColorStyle style;

	private Image image;

	void OnEnable()
	{
		UpdateTheme();
	}

#if UNITY_EDITOR
	private void Update()
	{
		if (
		    style != null &&
		    (
		        image.color != style.color
		    )
		) {
			UpdateTheme();
		}
	}
#endif

	public void UpdateTheme()
	{
		RytUtil.GetIfNeed<Image>(ref image, this);

		if (style && image) {
			image.color = style.color;
		}
	}
}