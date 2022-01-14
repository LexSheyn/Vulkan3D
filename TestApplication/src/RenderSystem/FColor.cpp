#include "../PrecompiledHeaders/stdafx.h"
#include "FColor.h"

namespace t3d
{
// Constructors:

	FColor::FColor()
		: R(1.0f), G(1.0f), B(1.0f), A(1.0f)
	{
	}

	FColor::FColor(float32 Red, float32 Green, float32 Blue)
		: R(Red), G(Green), B(Blue), A(1.0f)
	{
	}

	FColor::FColor(float32 Red, float32 Green, float32 Blue, float32 Alpha)
		: R(Red), G(Green), B(Blue), A(Alpha)
	{
	}

// Predefined Colors:

	static const FColor Transparent = FColor( 0.0f, 0.0f, 0.0f, 0.0f );

	static const FColor White   = FColor( 1.0f, 1.0f, 1.0f );
	static const FColor Black   = FColor( 0.0f, 0.0f, 0.0f );

	static const FColor Red     = FColor( 1.0f, 0.0f, 0.0f );
	static const FColor Green   = FColor( 0.0f, 1.0f, 0.0f );
	static const FColor Blue    = FColor( 0.0f, 0.0f, 1.0f );

	static const FColor Yellow  = FColor( 1.0f, 1.0f, 0.0f );
	static const FColor Cyan    = FColor( 0.0f, 1.0f, 1.0f );
	static const FColor Magenta = FColor( 1.0f, 0.0f, 1.0f );

} // t3d