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

struct TouchedVehicle
{
	Vehicle vehicle;
	Vector3 bubble_velocity;
	bool first_touch;

	bool operator ==(TouchedVehicle other) const { return vehicle == other.vehicle; }
};

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
			uniquePushBack(veh_list, TouchedVehicle{ v, player_veh.getVelocity(false), true });
}

void bubbleCars(list<TouchedVehicle>& veh_list, float velocityy)
{
	if (veh_list.size() >= 50)
	{
		veh_list.remove_if([](TouchedVehicle tv) { return !tv.vehicle.exist(); });	// Clear list from invalid vehicles deleted by RAGE
		hud::showNotification("Superata soglia 50 veicoli, lista pulita.");
	}
	for (TouchedVehicle& tv : veh_list)
	{
		if (!tv.vehicle)
			continue;	// Skip non persistent vehicles deleted by RAGE
		float velocity;
		Vector3 curr_velocity = tv.bubble_velocity;
		if (tv.first_touch)	// first_touch checking force vehicle to go up even if the player applies a force in down direction (example: player hit vehicle while is on a downhill road)
		{

			tv.first_touch = false;
		}
		
		if (curr_velocity.z > 0.0f)
		{

		}


		

		else if (curr_velocity.z < 0.0f)
			velocity *= -1;
		curr_velocity.z = velocity;
		tv.vehicle.setVelocity(curr_velocity);
	}
}

void mainLoop()
{
	bool is_cheat_running = false;
	list<TouchedVehicle> touched_vehs;
	float up_velocity = 1.0f;
	bool clear_list = true;
	while (true)
	{
		if (IsKeyDown(VK_NUMPAD1) && !is_cheat_running)
		{
			scriptWait(200);
			is_cheat_running = true;
			hud::printMessage("BUBBLECARS cheat activated!");
		}
		if (IsKeyDown(VK_NUMPAD4))
		{
			scriptWait(200);
			Vehicle bar = createVehicle("Barracks", Ped::player().getPosition() + Ped::player().getForwardVector() * 5);
			bar.modifyTopSpeed(100.0f);
		}
		if (is_cheat_running)
		{
			if (IsKeyDown(VK_NUMPAD0))
			{
				scriptWait(200);
				is_cheat_running = false;
				if (clear_list)
					touched_vehs.clear();
				hud::printMessage("BUBBLECARS cheat deactivated!");
				continue;
			}
			else if (IsKeyDown(VK_NUMPAD3))
			{
				scriptWait(200);
				touched_vehs.clear();
				for (Vehicle v : getAllVehicles<256>())
					if (v != Ped::player().getCurrentVehicle())
						//touched_vehs.push_back(TouchedVehicle{ v,  });
				bubbleCars(touched_vehs, up_velocity);
				continue;
			}
			touchCheck(touched_vehs);
			bubbleCars(touched_vehs, up_velocity);
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