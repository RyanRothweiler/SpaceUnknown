using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class CargoItemLine : MonoBehaviour
{
	public ItemInstance item;

	public Image icon;
	public TMP_Text title;
	public TMP_Text description;

	public void Update()
	{
		if (item != null) {
			description.text = "x " + item.count;
		}
	}

	public void SetItem(ItemInstance item)
	{
		this.item = item;

		icon.sprite = item.definition.icon;
		title.text = item.definition.displayName;
	}
}
