#include "native_wrapper.h"

GTAScriptError::GTAScriptError(const std::string& scriptName, const std::string& whatArg)
	: std::runtime_error{ "~w~in ~h~" + scriptName + ".asi~h~~w~\n" + whatArg } {}

void GTAScriptError::fatalErrorNotification(const std::string& scriptName)
{
	hud::showNotification("~r~Fatal unhandled exception in " + scriptName + ".asi");
}

void GTAScriptError::whatNotification()
{
	std::string what_str{ what() };
	hud::showNotification("~r~SCRIPT ERROR~n~" + what_str);
}

namespace hud
{
	void showNotification(const std::string& text, bool blink)
	{
		HUD::BEGIN_TEXT_COMMAND_THEFEED_POST("STRING");	// Notification entry type
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
		HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(blink, TRUE);
	}

	void printMessage(const std::string& text, int duration, bool drawImmediately)
	{
		HUD::BEGIN_TEXT_COMMAND_PRINT("STRING"); // Message entry type
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
		HUD::END_TEXT_COMMAND_PRINT(duration, drawImmediately);
	}
}

namespace graphics
{
	void drawLine(const Vector3& start, const Vector3& end, pk99::RGBAColor color)
	{
		GRAPHICS::DRAW_LINE(start.x, start.y, start.z, end.x, end.y, end.z, color[0], color[1], color[2], color[3]);
	}

	void drawBox(const Vector3& start, const Vector3& end, pk99::RGBAColor color)
	{
		GRAPHICS::DRAW_BOX(start.x, start.y, start.z, end.x, end.y, end.z, color[0], color[1], color[2], color[3]);
	}

	void drawPolygon(const Vector3& vertexA, const Vector3& vertexB, const Vector3& vertexC, pk99::RGBAColor color)
	{
		GRAPHICS::DRAW_POLY(vertexA.x, vertexA.y, vertexA.z, vertexB.x, vertexB.y, vertexB.z, vertexC.x, vertexC.y, vertexC.z, color[0], color[1], color[2], color[3]);
	}
}