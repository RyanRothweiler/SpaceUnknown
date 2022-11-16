#pragma once

#ifndef ParticleSystemCPP
#define ParticleSystemCPP

#include "ParticleSystem.h"

// NOTE maybe particles don't need to be owned by a system. A particle system and grab a particle when it needs one
// and then it's owned by the system for the life of the particle. Once dead, the particle returns to the particle pool.

real64
GetParticleComponentValue(particle_system_component* Component)
{
	real64 Value = Component->TimeValues[0].Value;
	if (Component->TimeValues[0].IsRandomized) {
		Value = Component->TimeValues[0].RandomRange.Min;
		if (RangeValid(&Component->TimeValues[0].RandomRange)) {
			Value = RandomRangeFloat(Component->TimeValues[0].RandomRange);
		}
	}
	return (Value);
}

real64
GetNextStep(real64 Current, real64 Target, uint64 Steps)
{
	return ((Target - Current) / Steps);
}

void
InitParticleComponent(particle_component* ParticleComp, particle_system_component* SystemComp, uint64 TotalLife)
{
	ParticleComp->ChangeCounter = 0;
	ParticleComp->NextTimeValueIndex = 1;

	// When to change
	uint32 TotalTimeValues = 0;
	for (int32 Index = 0; Index < ARRAY_SIZE(SystemComp->TimeValues, time_value); Index++) {
		if (SystemComp->TimeValues[Index].Using) {
			TotalTimeValues++;
		}
	}
	ParticleComp->WhenToChange = (uint32)(TotalLife / ClampValue((uint64)1, (uint64)1000, (uint64)TotalTimeValues - 1));

	// Next addition
	if (TotalTimeValues == 1) {
		ParticleComp->NextAddition = 0;
	} else {
		ParticleComp->NextAddition = GetNextStep(SystemComp->TimeValues[0].Value, SystemComp->TimeValues[1].Value, TotalLife);
	}
}

void
InitParticle(particle* Part, particle_system* System)
{
	Part->TStep = 0;

	if (System->EmitType == Emit_Point) {
		Part->Center = System->Center;
	} else if (System->EmitType == Emit_Circle) {
		real64 Radius = System->CircleEmitRadius;
		if (Radius <= 0) {
			Radius = 1;
		}

		real64 T = 2 * PI * RandomRangeFloat(0.0f, Radius);
		real64 U = RandomRangeFloat(0.0f, Radius) + RandomRangeFloat(0.0f, Radius);
		real64 R = U;
		if (U < 1) {
			R = 2 - U;
		}

		Part->Center = vector2{R * Cos(T) * Radius, R * Sin(T) * Radius} + System->Center;
	}

	Part->Origin = Part->Center;

	// Life Length
	Part->LifeLength = System->Life;

	for (int32 Index = 0; Index < ARRAY_SIZE(System->Components, particle_system_component); Index++) {
		InitParticleComponent(&Part->Components[Index], &System->Components[Index], Part->LifeLength);
	}


	// NOTE it is because of this loop that particle components and particle system components must aligh
	for (int32 Index = 0; Index < ARRAY_SIZE(System->Components, particle_system_component); Index++) {
		Part->Components[Index].CurrentValue = GetParticleComponentValue(&System->Components[Index]);
	}
}

void
StepParticleSystem(particle_system* System)
{
	for (uint32 ParticleIndex = 0; ParticleIndex < ParticleCount; ParticleIndex++) {
		particle* Part = &System->Particles[ParticleIndex];

		// update component values
		for (int32 Index = 0; Index < ARRAY_SIZE(System->Components, particle_system_component); Index++) {
			particle_component* ParticleComp = &Part->Components[Index];
			particle_system_component* SystemComp = &System->Components[Index];

			if (ParticleComp->ChangeCounter == ParticleComp->WhenToChange) {
				ParticleComp->ChangeCounter = 0;

				// Next addition
				if (ParticleComp->NextTimeValueIndex + 1 < ARRAY_SIZE(SystemComp->TimeValues, time_value)) {
					ParticleComp->NextAddition =
					    GetNextStep(SystemComp->TimeValues[ParticleComp->NextTimeValueIndex].Value,
					                SystemComp->TimeValues[ParticleComp->NextTimeValueIndex + 1].Value,
					                System->Life);
				}
				ParticleComp->NextTimeValueIndex++;
			}
			ParticleComp->ChangeCounter++;

			ParticleComp->CurrentValue += ParticleComp->NextAddition;
		}

		Part->Center = Part->Center + vector2{Part->Components[Comp_VelocityX].CurrentValue,
		                                      Part->Components[Comp_VelocityY].CurrentValue};
		Part->TStep++;

		if (Part->TStep >= Part->LifeLength) {
			InitParticle(Part, System);
		}
	}
}

// void
// RenderParticleSystem(particle_system * System, render_layer * RenderLayer, memory_arena * Memory)
// {
// 	for (uint32 ParticleIndex = 0; ParticleIndex < ParticleCount; ParticleIndex++) {
// 		particle* Part = &System->Particles[ParticleIndex];

// 		render_entity* Entity = GetNextEntity(RenderLayer, RenderEntity_Texture);
// 		Entity->GameManualOffset = vector2{0, Part->Origin.Y - Part->Center.Y};

// 		gl_texture *TextureLinkData = &Entity->Texture;
// 		TextureLinkData->Image = &System->Image;
// 		TextureLinkData->Center = Part->Center;
// 		TextureLinkData->Scale = vector2{Part->Components[Comp_Scale].CurrentValue, Part->Components[Comp_Scale].CurrentValue};
// 		TextureLinkData->Color = color{(real32)Part->Components[Comp_ColorR].CurrentValue,
// 		                               (real32)Part->Components[Comp_ColorG].CurrentValue,
// 		                               (real32)Part->Components[Comp_ColorB].CurrentValue,
// 		                               (real32)Part->Components[Comp_ColorA].CurrentValue};
// 		TextureLinkData->RadiansAngle = Part->Components[Comp_Rotation].CurrentValue;
// 	}
// }

void
InitParticleSystem(particle_system * System)
{
	uint64 NextTimeStep = 0;
	uint64 ParticlesPerTimeStep = (uint64)((real64)System->Life / (real64)ParticleCount);
	if (ParticlesPerTimeStep <= 0) {
		ParticlesPerTimeStep = 1;
	}

	for (uint32 ParticleIndex = 0; ParticleIndex < ParticleCount; ParticleIndex++) {
		InitParticle(&System->Particles[ParticleIndex], System);

		if (ParticleIndex % ParticlesPerTimeStep == 0) {
			NextTimeStep++;
		}

		for (uint32 Index = 0; Index < NextTimeStep; Index++) {
			StepParticleSystem(System);
		}
	}
}

void
SaveParticleSystem(particle_system * System, string Path)
{
	PlatformApi.WriteFile((Path + System->Name).CharArray, System, sizeof(particle_system));
}

void
FillDefaultComponent(particle_system_component * Comp, string Name)
{
	(*Comp) = {};
	Comp->Name = Name;
	Comp->TimeValues[0].Using = true;
}

void
LoadParticleSystem(particle_system * SystemFilling, string RootAssetPath, loaded_image DefaultImage, memory_arena * Memory)
{
	string FilePath = RootAssetPath + SystemFilling->Name;
	if (PlatformApi.FileExists(FilePath.CharArray)) {
		read_file_result Result = PlatformApi.ReadFile(FilePath.CharArray, Memory);
		if (Result.ContentsSize == sizeof(particle_system)) {
			particle_system* System = (particle_system *)Result.Contents;
			*SystemFilling = *System;
			InitParticleSystem(SystemFilling);
			return;
		}
	}

	// NOTE these are the initial default particle values
	SystemFilling->Using = true;

	SystemFilling->Center = vector2{500, 500};

	SystemFilling->Life = 100;

	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_Scale], "Scale");
	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_VelocityX], "Velocity X");
	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_VelocityY], "Velocity Y");
	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_ColorR], "Color R");
	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_ColorG], "Color G");
	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_ColorB], "Color B");
	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_ColorA], "Color A");
	FillDefaultComponent(&SystemFilling->Components[(int32)Comp_Rotation], "Rotation");

	InitParticleSystem(SystemFilling);
}

#endif