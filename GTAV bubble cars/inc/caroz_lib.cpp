#include "caroz_lib.h"

namespace pk99
{
	RGBColor::RGBColor(USHORT red, USHORT green, USHORT blue)
		: r{ red }, g{ green }, b{ blue }
	{
		if (red > max_col_value || green > max_col_value || blue > max_col_value)
			throw std::invalid_argument{ "Invalid color value" };
	}

	USHORT RGBColor::operator[](int index) const
	{
		switch (index)
		{
		case 0: return r;
		case 1: return g;
		case 2: return b;
		}
		throw std::out_of_range{ "Invalid color index" };
	}

	RGBAColor::RGBAColor(USHORT red, USHORT green, USHORT blue, USHORT alpha)
		: RGBColor(red, green, blue), a{ alpha }
	{
		if (alpha > RGBColor::max_col_value)
			throw std::invalid_argument{ "Invalid alpha value" };
	}

	USHORT RGBAColor::operator[](int index) const
	{
		switch (index)
		{
		case 0:
		case 1:
		case 2:
			return RGBColor::operator[](index);
		case 3:
			return a;
		}
		throw std::out_of_range{ "Invalid index" };
	}

	COLORREF colorAsInteger(const RGBColor& rgbCol)
	{
		return RGB(rgbCol.getRed(), rgbCol.getGreen(), rgbCol.getBlue());
	}

	void wstrUpper(std::wstring& ws)
	{
		for (auto& c : ws)
			c = towupper(c);
	}
}
