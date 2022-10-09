using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

[CreateAssetMenu(fileName = "NewFontStyle", menuName = "ScriptableObjects/Theme Font Style", order = 1)]
public class ThemeFontStyle : ScriptableObject
{
	public Color color;
	public float size;
	public float characterSpacing;
	public float lineSpacing;
	public TMP_FontAsset font;
}
