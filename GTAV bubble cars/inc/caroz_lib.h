#ifndef CAROZLIB
#define CAROZLIB

#include <string>
#include <stdexcept>
#include <Windows.h>

namespace pk99
{
	class RGBColor
	{
		USHORT r, g, b;

	public:
		static const USHORT max_col_value = 255;

		RGBColor(USHORT red, USHORT green, USHORT blue);
		RGBColor() : RGBColor{ 0, 0, 0 } {}

		USHORT getRed() const { return r; }
		USHORT getGreen() const { return g; }
		USHORT getBlue() const { return b; }

		USHORT operator [](int index) const;
	};

	class RGBAColor : public RGBColor
	{
		USHORT a;

	public:
		RGBAColor(USHORT red, USHORT green, USHORT blue, USHORT alpha);
		RGBAColor() : RGBAColor{ 0, 0, 0, 0 } {}

		USHORT getAlpha() const { return a; }

		USHORT operator[](int index) const;
	};

	COLORREF colorAsInteger(const RGBColor& rgbCol);

	void wstrUpper(std::wstring& ws);
}

#endif // !CAROZLIB

