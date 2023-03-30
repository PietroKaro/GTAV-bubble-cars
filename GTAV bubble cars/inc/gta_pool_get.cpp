#ifndef GET_FUNCS
#define GET_FUNCS

#include "gta_pool.h"

template<int array_size>
inline std::vector<Ped> getAllPeds()
{
	int ped_arr[array_size]{};
	int amount = worldGetAllPeds(ped_arr, array_size);
	std::vector<Ped> vPed;
	for (int i = 0; i < amount; ++i)
		vPed.push_back(ped_arr[i]);
	return vPed;
}

template<int array_size>
inline std::vector<Vehicle> getAllVehicles()
{
	int veh_arr[array_size]{};
	int amount = worldGetAllVehicles(veh_arr, array_size);
	std::vector<Vehicle> vVeh;
	for (int i = 0; i < amount; ++i)
		vVeh.push_back(veh_arr[i]);
	return vVeh;
}

template<int array_size>
inline std::vector<Prop> getAllProps()
{
	int prop_arr[array_size]{};
	int amount = worldGetAllObjects(prop_arr, array_size);
	std::vector<Prop> vProp;
	for (int i = 0; i < amount; ++i)
		vProp.push_back(prop_arr[i]);
	return vProp;
}

#endif // !GET_FUNCS