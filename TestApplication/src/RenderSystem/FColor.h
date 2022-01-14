#ifndef FCOLOR_H
#define FCOLOR_H

namespace t3d
{
	/// <summary>
	/// <para> 4 component 32-bit float type color. </para>
	/// Red, Green, Blue, Alpha, which corresponds to VK_FORMAT_R32G32B32A32_SFLOAT.
	/// </summary>
	class FColor
	{
	public:

	// Constructors:

		/// <summary>
		/// Opaque White color by default.
		/// </summary>
		FColor();
		/// <summary>
		/// Opaque by default.
		/// </summary>
		FColor(float32 Red, float32 Green, float32 Blue);
		FColor(float32 Red, float32 Green, float32 Blue, float32 Alpha);

	// Predefined Colors:

		static const FColor Transparent;

		static const FColor White;
		static const FColor Black;

		static const FColor Red;
		static const FColor Green;
		static const FColor Blue;
		
		static const FColor Yellow;
		static const FColor Cyan;
		static const FColor Magenta;

	// Components:

		float32 R;
		float32 G;
		float32 B;
		float32 A;
	};

} // t3d

#endif // FCOLOR_H