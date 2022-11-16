#pragma once
#ifndef SortsCPP
#define SortsCPP

void
Exchange(real64 *Array, int FirstIndex, int SecondIndex)
{
	real64 Saved = Array[FirstIndex];
	Array[FirstIndex] = Array[SecondIndex];
	Array[SecondIndex] = Saved;
}

int
Partition(real64 *Array, int low, int high)
{
	int i = low;
	int j = high + 1;
	while (true)
	{
		while (Array[++i] > Array[low])
		{
			if (i == high)
			{
				break;
			}
		}
		while (Array[low] > Array[--j])
		{
			if (j == low)
			{
				break;
			}
		}
		if (i >= j)
		{
			break;
		}

		Exchange(Array, i, j);
	}
	Exchange(Array, low, j);
	return (j);
}

void
QuickSort(real64 *Array, int low, int high)
{
	if (high <= low)
	{
		return;
	}

	int j = Partition(Array, low, high);
	QuickSort(Array, low, j - 1);
	QuickSort(Array, j + 1, high);
}










// // NOTE all of the below is basicaly a copy of the above, I don't know how to abstract out the sorting algorithm correctly
// real64
// CalcEntityValue(real64 CenterY, real64 ScaleY, real64 WorldCenterY, real64 ManualY)
// {
// 	return (2000 - ((ManualY + CenterY) - ((ScaleY / 2.0f) - (ScaleY * WorldCenterY))));
// }

// real64
// AllocateEntityLayerValue(render_entity* Entity)
// {
// 	if (Entity->Type == RenderEntity_Texture)
// 	{
// 		return (CalcEntityValue(Entity->Texture.Center.Y, Entity->Texture.Scale.Y,
// 		                        Entity->Texture.Image->CenterOffset.Y, Entity->GameManualOffset.Y));
// 	}
// 	if (Entity->Type == RenderEntity_Square)
// 	{
// 		return ((Entity->Square.TopLeft.Y + Entity->Square.BottomRight.Y) / 2.0f);
// 	}


// 	//Could not sort that entity type. Probably just add it to the if's above.
// 	Assert(0);
// 	return (0.0f);
// }

// void
// LayerExchange(render_entity* Entities, int32 FirstIndex, int32 SecondIndex)
// {
// 	render_entity SavedEntity = Entities[FirstIndex];
// 	Entities[FirstIndex] = Entities[SecondIndex];
// 	Entities[SecondIndex] = SavedEntity;
// }

// int32
// LayerPartition(render_entity* Entities, int32 low, int32 high)
// {
// 	int i = low;
// 	int j = high + 1;
// 	while (true)
// 	{
// 		while (AllocateEntityLayerValue(&Entities[++i]) > AllocateEntityLayerValue(&Entities[low]))
// 		{
// 			if (i == high)
// 			{
// 				break;
// 			}
// 		}
// 		while (AllocateEntityLayerValue(&Entities[low]) > AllocateEntityLayerValue(&Entities[--j]))
// 		{
// 			if (j == low)
// 			{
// 				break;
// 			}
// 		}
// 		if (i >= j)
// 		{
// 			break;
// 		}

// 		LayerExchange(Entities, i, j);
// 	}
// 	LayerExchange(Entities, low, j);
// 	return (j);
// }

// void
// LayerSort(render_entity* Entities, int Low, int High)
// {
// 	if (High <= Low)
// 	{
// 		return;
// 	}

// 	int j = LayerPartition(Entities, Low, High);
// 	LayerSort(Entities, Low, j - 1);
// 	LayerSort(Entities, j + 1, High);
// }

// void
// SortRenderLayer(render_layer* Layer)
// {
// 	LayerSort(Layer->Entities, 0, (int)Layer->EntitiesCount - 1);
// }

#endif