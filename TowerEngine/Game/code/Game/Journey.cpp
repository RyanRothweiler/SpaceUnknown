namespace journey {

	journey_step* AddStep(ship_journey* SJ) {
		journey_step* Step = &SJ->Steps[SJ->StepsCount++];
		Assert(SJ->StepsCount < ArrayCount(SJ->Steps));

		return Step;
	}

	void Execute(ship_journey* SJ) {
		SJ->InProgress = true;
		SJ->CurrentStep = -1;
	}

}
