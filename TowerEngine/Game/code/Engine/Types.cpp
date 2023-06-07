#pragma once
#ifndef TypesCPP
#define TypesCPP

#include <stdint.h>

#define BytesToKilobytes(value) (value / 1024)
#define BytesToMegabytes(value) (BytesToKilobytes(value) / 1024)
#define BytesToGigabytes(value) (BytesToMegabytes(value) / 1024)

#define Thousand(value) ((value) * 1000)

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terrabytes(value) (Gigabytes(value) * 1024LL)

#define MinutesToMilliseconds(value) (value * 1000.0f * 60.0f)
#define SecondsToMilliseconds(value) (value * 1000.0f)

#define MillisecondsToSeconds(value) (value / 1000.0f)
#define MillisecondsToMinutes(value) (value / 1000.0f / 60.0f)
#define MicrosecondsToSeconds(value) (value / 1000000.0f)

#define INVALID_DEFAULT default: Assert(0);

int8_t 				typedef int8;
int16_t 			typedef int16;
int32_t 			typedef int32;
int64_t 			typedef int64;
unsigned char 		typedef uint8;
unsigned short 		typedef uint16;
unsigned int 		typedef uint32;
unsigned long long 	typedef uint64;
int32 				typedef bool32;
float 				typedef real32;
double 				typedef real64;

int8_t 				typedef i8;
int16_t 			typedef i16;
int32_t 			typedef i32;
int64_t 			typedef i64;
unsigned char 		typedef ui8;
unsigned short 		typedef ui16;
unsigned int 		typedef ui32;
unsigned long long 	typedef ui64;
int32 				typedef b32;
float 				typedef r32;
double 				typedef r64;

#endif
