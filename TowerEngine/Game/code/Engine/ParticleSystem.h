#pragma once

#ifndef ParticleSystemH
#define ParticleSystemH

enum component_type
{
	Comp_Scale,
	Comp_VelocityX, Comp_VelocityY,
	Comp_ColorR, Comp_ColorG, Comp_ColorB, Comp_ColorA,
	Comp_Rotation,
};



// -------------- Particles stuff

struct particle_component
{
	real64 CurrentValue;

	// What is needed to add in this frame to meet the next time value target
	real64 NextAddition;
	// When ChangeCounter == WhenToChange then ChangeCounter is set to 0 and NextAddition is recalculated
	uint32 ChangeCounter;
	uint32 WhenToChange;
	// The next index into TimeValues which is needed to calculate the NextAddition
	uint32 NextTimeValueIndex;
};

struct particle
{
	particle_component Components[8];

	vector2 Center;
	uint64 LifeLength;
	uint64 TStep;

	// Where this particle was given life.
	vector2 Origin;
};






// -------------- Particle system stuff


struct time_value
{
	bool32 Using;
	real64 Value;
	bool32 IsRandomized;
	range RandomRange;
};

struct particle_system_component
{
	string Name;
	time_value TimeValues[10];
};

enum emit_type
{
	Emit_Point,
	Emit_Circle,
};
string EmitTypeNames[] = {"Point", "Circle"};

const uint32 ParticleCount = 50;
struct particle_system
{
	string Name;
	bool32 Using;
	emit_type EmitType;
	bool32 Paused;

	real64 CircleEmitRadius;
	vector2 Center;

	uint64 Life;

	particle_system_component Components[8];

	loaded_image Image;
	particle Particles[50];

};

#endif