using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

[ExecuteInEditMode]
public class ThemeFontApply : MonoBehaviour
{
	public ThemeFontStyle style;

	private TMP_Text text;

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
		        text.color != style.color ||
		        text.fontSize != style.size ||
		        text.font != style.font ||
		        text.lineSpacing != style.lineSpacing ||
		        text.characterSpacing != style.characterSpacing
		    )
		) {
			UpdateTheme();
		}
	}
#endif

	public void UpdateTheme()
	{
		RytUtil.GetIfNeed<TMP_Text>(ref text, this);

		if (style && text) {
			text.color = style.color;
			text.fontSize = style.size;
			text.font = style.font;
			text.characterSpacing = style.characterSpacing;
			text.lineSpacing = style.lineSpacing;
		}
	}
}