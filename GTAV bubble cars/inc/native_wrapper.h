#ifndef NATIVE_WRAPPER
#define NATIVE_WRAPPER

#include <stdexcept>
#include "gtav_types.h"
#include "natives.h"
#include "gta_math.h"
#include "caroz_lib.h"

struct GTAScriptError : std::runtime_error
{
	GTAScriptError(const std::string& scriptName, const std::string& whatArg);

	static void fatalErrorNotification(const std::string& scriptName);

	void whatNotification();
};

namespace hud
{
	void showNotification(const std::string& text, bool blink = false);
	inline void showNotification(int val, bool blink = false) { showNotification(std::to_string(val), blink); }
	inline void showNotification(float val, bool blink = false) { showNotification(std::to_string(val), blink); }

	void printMessage(const std::string& text, int duration = 3000, bool drawImmediately = false);
	inline void printMessage(int val, int duration = 3000, bool drawImmediately = false) { printMessage(std::to_string(val), duration, drawImmediately); }
	inline void printMessage(float val, int duration = 3000, bool drawImmediately = false) { printMessage(std::to_string(val), duration, drawImmediately); }
}

namespace graphics
{
	void drawLine(const Vector3& start, const Vector3& end, pk99::RGBAColor color);
	void drawBox(const Vector3& start, const Vector3& end, pk99::RGBAColor color);

	// Three vertex
	void drawPolygon(const Vector3& vertexA, const Vector3& vertexB, const Vector3& vertexC, pk99::RGBAColor color);
}

#endif // !NATIVE_WRAPPER
