using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ModuleLine : MonoBehaviour
{
	public Module module;
	public ProgressBar activationProgress;

	void Update()
	{
		activationProgress.val = module.PercentageActivation();
	}
}