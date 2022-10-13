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
	private SpriteRenderer spriteRend;

	void OnEnable()
	{
		UpdateTheme();
	}

#if UNITY_EDITOR
	private void Update()
	{
		/*
		bool update = false;
		if (style != null) {
		}

		if (
		    style != null &&
		    (
		        (
		            image &&
		            image.color != style.color
		        ) ||
		        (
		            spriteRend &&
		            spriteRend.color != style.color
		        ) ||

		    )
		) {
		}
		*/

		UpdateTheme();
	}
#endif

	public void UpdateTheme()
	{
		if (image == null || spriteRend == null) {
			RytUtil.GetIfNeed<Image>(ref image, this);
			RytUtil.GetIfNeed<SpriteRenderer>(ref spriteRend, this);
		}

		if (style) {
			if (image) {
				image.color = style.color;
			}
			if (spriteRend) {
				spriteRend.color = style.color;
			}
		}
	}
}