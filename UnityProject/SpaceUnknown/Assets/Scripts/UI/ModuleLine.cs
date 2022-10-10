using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ModuleLine : MonoBehaviour
{
	public ModuleInstance module;
	public ProgressBar activationProgress;

	void Update()
	{
		activationProgress.val = module.PercentageActivation();
	}
}