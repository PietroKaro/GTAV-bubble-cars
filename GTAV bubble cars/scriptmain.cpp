#include <Windows.h>
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "inc/main.h"
#include "inc/keyboard.h"
#include "inc/native_wrapper.h"
#include "inc/gta_pool.h"
#include "inc/gta_pool_get.cpp"
#include "inc/gta_math.h"

using namespace std;

constexpr short BUBBLECARS_MAX = 256;
constexpr short TOUCHED_MAX = 50;

struct TouchedVehicle
{
	Vehicle vehicle;
	bool first_touch;

	explicit TouchedVehicle(Vehicle veh) : vehicle{ veh }, first_touch{ true } {}

	bool operator ==(TouchedVehicle other) const { return vehicle == other.vehicle; }
};

// Simple "read value" function
float readVelParam(const string& line)
{
	istringstream iss{ line };
	iss.ignore(11);
	float velocity;
	if (!(iss >> velocity) || velocity < 0.5f || velocity > 50.0f)
	{
		hud::showNotification("Up velocity config value is invalid. Default will be used.");
		velocity = 1.0f;
	}
	return velocity;
}

// Simple "read value" function
bool readClearParam(const string& line)
{
	istringstream iss{ line };
	iss.ignore(6);
	string valstr;
	if (iss >> valstr)
	{
		transform(valstr.begin(), valstr.end(), valstr.begin(), toupper);
		if (valstr == "TRUE")
			return true;
		else if (valstr == "FALSE")
			return false;
	}
	hud::showNotification("Clear config value is invalid. Default will be used.");
	return true;
}

pair<float, bool> readScriptData()
{
	ifstream ifs{ "GTAVBubbleCars.ini" };
	if (!ifs)
	{
		hud::showNotification("Unable to read script config file. Default config will be used");
		return pair<float, bool>(1.0f, false);
	}
	string line1, line2;
	if (!getline(ifs, line1) || !getline(ifs, line2))
		hud::showNotification("Error while reading config values. Default values will be used");
	return pair<float, bool>{ readVelParam(line1), readClearParam(line2) };
}

inline void removeInvalidVehs(list<TouchedVehicle>& vehList)
{ 
	vehList.remove_if([](TouchedVehicle tv) { return !tv.vehicle.exist(); });
}

void resetTouchedVehs(list<TouchedVehicle>& vehList)
{
	for (TouchedVehicle& tv : vehList)
		tv.first_touch = true;
}

void uniquePushBack(list<TouchedVehicle>& vehList, TouchedVehicle item)	// Avoid duplicate elements
{
	if (find(vehList.begin(), vehList.end(), item) != vehList.end())
		return;
	vehList.push_back(item);
}

void touchCheck(list<TouchedVehicle>& vehList)
{
	auto world_veh = getAllVehicles<BUBBLECARS_MAX>();
	Vehicle player_veh = Ped::player().getCurrentVehicle();
	if (!player_veh)
		return;
	for (Vehicle v : world_veh)
		if (v != player_veh && v.isTouching(player_veh))
			uniquePushBack(vehList, TouchedVehicle{ v });
}

void bubbleCars(list<TouchedVehicle>& veh_list, float upVelocity)
{
	if (veh_list.size() >= TOUCHED_MAX)
	{
		removeInvalidVehs(veh_list);
		hud::showNotification("Superata soglia 50 veicoli, lista pulita.");
	}
	for (TouchedVehicle& tv : veh_list)
	{
		if (!tv.vehicle)
			continue;			// Skip non persistent vehicles deleted by RAGE
		Vector3 velocity = tv.vehicle.getVelocity(false);
		float up_velocity = upVelocity;
		if (tv.first_touch)		// first_touch checking force vehicle to go up even if the player applies a force in down direction (example: player hit vehicle while is on a downhill road)
			tv.first_touch = false;
		else if (velocity.z <= 0.0f)
			up_velocity *= -1;
		velocity.z = up_velocity;
		tv.vehicle.setVelocity(velocity);
	}
}

void mainLoop()
{
	bool is_cheat_running = false;
	int key_start = VK_NUMPAD1;
	int key_stop = VK_NUMPAD0;
	int key_bubble_all = VK_NUMPAD3;
	auto script_data = readScriptData();
	hud::showNotification(to_string(script_data.first) + "   " + to_string(script_data.second));
	list<TouchedVehicle> touched_vehs;
	while (true)
	{
		if (IsKeyDown(key_start) && !is_cheat_running)
		{
			scriptWait(200);
			is_cheat_running = true;
			if (!script_data.second)
			{
				removeInvalidVehs(touched_vehs);
				
				hud::showNotification("Lista pulita da veicoli invalidi.");
			}
			hud::printMessage("BUBBLECARS cheat activated!", 2000, true);
		}


		if (IsKeyDown(VK_NUMPAD6))
		{
			scriptWait(200);
			hud::showNotification((int)touched_vehs.size());
		}


		if (is_cheat_running)
		{
			if (IsKeyDown(key_bubble_all))
			{
				scriptWait(200);
				touched_vehs.clear();
				for (Vehicle v : getAllVehicles<BUBBLECARS_MAX>())
					if (v != Ped::player().getCurrentVehicle())
						touched_vehs.push_back(TouchedVehicle{ v });
				bubbleCars(touched_vehs, script_data.first);
				continue;
			}
			touchCheck(touched_vehs);
			bubbleCars(touched_vehs, script_data.first);
			if (IsKeyDown(key_stop))
			{
				scriptWait(200);
				if (script_data.second)
				{
					touched_vehs.clear();
					hud::showNotification("Lista pulita da tutti i veicoli.");
				}
				else
				{
					resetTouchedVehs(touched_vehs);
					hud::showNotification("Lista resettata.");
				}
				hud::printMessage("BUBBLECARS cheat deactivated!", 2000, true);
				is_cheat_running = false;
			}
		}
		scriptWait(0);
	}
}

void scriptMain()
{
	mainLoop();
}

BOOL APIENTRY DllMain(HMODULE hDll, DWORD reason, PVOID)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		scriptRegister(hDll, &scriptMain);
		keyboardHandlerRegister(&OnKeyboardMessage);
		break;
	case DLL_PROCESS_DETACH:
		keyboardHandlerUnregister(&OnKeyboardMessage);
		scriptUnregister(hDll);
		break;
	}
	return TRUE;
}