void ItemTransferStep(void* SelfData, real64 Time, game::state* State)
{
	item_instance* Inst = (item_instance*)SelfData;

	UnregisterStepper(&Inst->TransferStepper, State);
}

void ItemTransferToStation(item_instance* Inst, station* Station, game::state* State)
{
	Inst->TransferTarget = item_transfer_target::station;
	Inst->ToStationTransfer = Station;
	RegisterStepper(&Inst->TransferStepper,
	                &ItemTransferStep,
	                (void*)Inst,
	                State);
}