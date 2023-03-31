#include <Windows.h>
#include <list>
#include <algorithm>
#include "inc/main.h"
#include "inc/keyboard.h"
#include "inc/native_wrapper.h"
#include "inc/gta_pool.h"
#include "inc/gta_pool_get.cpp"
#include "inc/gta_math.h"

using namespace std;

bool test1;

struct TouchedVehicle
{
	Vehicle vehicle;
	bool first_touch;

	explicit TouchedVehicle(Vehicle veh) : vehicle{ veh }, first_touch{ true } {}

	bool operator ==(TouchedVehicle other) const { return vehicle == other.vehicle; }
};

inline void removeInvalidVeh(list<TouchedVehicle>& veh_list)
{ 
	veh_list.remove_if([](TouchedVehicle tv) { return !tv.vehicle.exist(); });
}

void resetTouchedVehs(list<TouchedVehicle>& veh_list)
{
	for (TouchedVehicle& tv : veh_list)
		tv.first_touch = true;
}

void uniquePushBack(list<TouchedVehicle>& veh_list, TouchedVehicle item)	// Avoid duplicate elements
{
	if (find(veh_list.begin(), veh_list.end(), item) != veh_list.end())
		return;
	veh_list.push_back(item);
}

void touchCheck(list<TouchedVehicle>& veh_list)
{
	auto world_veh = getAllVehicles<256>();
	Vehicle player_veh = Ped::player().getCurrentVehicle();
	if (!player_veh)
		return;
	for (Vehicle v : world_veh)
		if (v != player_veh && v.isTouching(player_veh))
			uniquePushBack(veh_list, TouchedVehicle{ v });
}

void bubbleCars(list<TouchedVehicle>& veh_list, float up_velocity)
{
	if (veh_list.size() >= 50)
	{
		removeInvalidVeh(veh_list);
		hud::showNotification("Superata soglia 50 veicoli, lista pulita.");
	}
	for (TouchedVehicle& tv : veh_list)
	{
		if (!tv.vehicle)
			continue;	// Skip non persistent vehicles deleted by RAGE
		Vector3 velocity = tv.vehicle.getVelocity(false);
		float uv = up_velocity;
		if (tv.first_touch)	// first_touch checking force vehicle to go up even if the player applies a force in down direction (example: player hit vehicle while is on a downhill road)
			tv.first_touch = false;
		else if (velocity.z <= 0.0f)
			uv *= -1;
		velocity.z = uv;
		tv.vehicle.setVelocity(velocity);
	}
}

void mainLoop()
{
	bool is_cheat_running = false;
	int key_start = VK_NUMPAD1;
	int key_stop = VK_NUMPAD0;
	int key_bubble_all = VK_NUMPAD3;
	list<TouchedVehicle> touched_vehs;
	float up_velocity = 1.0f;
	bool clear_list = true;
	while (true)
	{
		if (IsKeyDown(key_start) && !is_cheat_running)
		{
			scriptWait(200);
			is_cheat_running = true;
			if (!clear_list)
			{
				removeInvalidVeh(touched_vehs);
				
				hud::showNotification("Lista pulita.");
			}
			hud::printMessage("BUBBLECARS cheat activated!", 2000, true);
		}


		if (IsKeyDown(VK_NUMPAD4))
		{
			scriptWait(200);
			Vehicle bar = createVehicle("Barracks", Ped::player().getPosition() + Ped::player().getForwardVector() * 5);
			bar.modifyTopSpeed(130.0f);
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
				for (Vehicle v : getAllVehicles<256>())
					if (v != Ped::player().getCurrentVehicle())
						touched_vehs.push_back(TouchedVehicle{ v });
				bubbleCars(touched_vehs, up_velocity);
				continue;
			}
			touchCheck(touched_vehs);
			bubbleCars(touched_vehs, up_velocity);
			if (IsKeyDown(key_stop))
			{
				scriptWait(200);
				if (clear_list)
				{
					touched_vehs.clear();
					hud::showNotification("Lista pulita.");
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