using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ModuleLine : MonoBehaviour
{
	public ProgressBar activationProgress;
	public Module module;

	void Start()
	{

	}

	void Update()
	{
		activationProgress.val = module.PercentageActivation();
	}
}