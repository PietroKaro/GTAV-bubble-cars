#include "gta_pool.h"

#pragma warning(disable: 28159)	// GetTickCount() 32 bit warning

// ---------- Model ----------

bool Model::request(DWORD maxTime) const
{
	DWORD ms_limit = GetTickCount() + maxTime;
	while (!hasLoaded())
	{
		if (GetTickCount() >= ms_limit)
			return false;
		request();
		scriptWait(0);
	}
	return true;
}

// ---------- WeaponAsset ----------

WeaponAsset::WeaponAsset(const std::string& weaponName, DWORD maxTime)
	: Model{ weaponName }
{ 
	request(maxTime);
}

bool WeaponAsset::request(DWORD maxTime) const
{
	DWORD ms_limit = GetTickCount() + maxTime;
	while (!hasLoaded())
	{
		if (GetTickCount() >= ms_limit)
			return false;
		request();
		scriptWait(0);
	}
	return true;
}

// ---------- Entity ----------

void Entity::delete_()
{
	if (!isPersistent())
		markPersistent();
	ENTITY::DELETE_ENTITY(&handle);
}

Blip Entity::getAttachedBlip() const
{
	return HUD::GET_BLIP_FROM_ENTITY(handle);
}

Blip Entity::attachBlip(int spriteID, int colorID, const std::string& name) const
{
	Blip bresult{ HUD::ADD_BLIP_FOR_ENTITY(handle) };
	if (!bresult)
		return 0;
	bresult.setSprite(spriteID);
	bresult.setColor(colorID);
	bresult.modifyName(name);
	return bresult;
}

void Entity::markAsNoLongerNeeded(bool leaveHandle)
{
	Hentity h = handle;
	ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&handle);
	if (leaveHandle)
		handle = h;
}

// ---------- Ped ----------

Vehicle Ped::getCurrentVehicle(bool includeLastVehicle) const
{
	return PED::GET_VEHICLE_PED_IS_IN(handle, includeLastVehicle);
}

TaskHandler Ped::getTaskHandler() const
{
	return TaskHandler{ *this };
}

bool Ped::isInVehicle(Vehicle* vehicle, bool validIfInSeat) const
{
	return vehicle ? PED::IS_PED_IN_VEHICLE(handle, vehicle->getHandle(), validIfInSeat) : PED::IS_PED_IN_ANY_VEHICLE(handle, validIfInSeat);
}

WeaponAsset Ped::getCurrentWeapon(bool isVehicleWeapon) const
{
	HashWeapon hw;
	isVehicleWeapon ? WEAPON::GET_CURRENT_PED_VEHICLE_WEAPON(handle, &hw) : WEAPON::GET_CURRENT_PED_WEAPON(handle, &hw, TRUE);
	return WeaponAsset{ hw };
}

void Ped::setIntoVehicle(Vehicle vehicle, VehicleSeat seat) const
{
	PED::SET_PED_INTO_VEHICLE(handle, vehicle.getHandle(), static_cast<int>(seat));
}

// ---------- Vehicle ----------

Ped Vehicle::createPedInside(Model pedModel, VehicleSeat seat, DWORD maxTimeModel, PedType type, bool noPersistent) const
{
	if (!pedModel.request(maxTimeModel))
		return 0;
	int itype = static_cast<int>(type);
	Ped presult{ PED::CREATE_PED_INSIDE_VEHICLE(handle, itype, pedModel.getHash(), static_cast<int>(seat), FALSE, TRUE) };
	if (presult && noPersistent)
		presult.markAsNoLongerNeeded(true);
	return presult;
}

pk99::RGBColor Vehicle::getCustomPrimaryColor() const
{
	int r, g, b;
	VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(handle, &r, &g, &b);
	return { static_cast<USHORT>(r), static_cast<USHORT>(g), static_cast<USHORT>(b) };
}

pk99::RGBColor Vehicle::getCustomSecondaryColor() const
{
	int r, g, b;
	VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(handle, &r, &g, &b);
	return { static_cast<USHORT>(r), static_cast<USHORT>(g), static_cast<USHORT>(b) };
}

// ---------- Blip ----------

std::list<Blip> Blip::all;

void Blip::delete_()
{
	all.remove(*this);
	HUD::REMOVE_BLIP(&handle);
}

void Blip::modifyName(const std::string& newName) const
{
	HUD::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(newName.c_str());
	HUD::END_TEXT_COMMAND_SET_BLIP_NAME(handle);
}

void Pickup::setRegenerationTime(int time) const
{ 
	OBJECT::SET_PICKUP_GLOW_OFFSET(handle, 0.51f);
	OBJECT::SET_PICKUP_REGENERATION_TIME(handle, time);
}

// ---------- TaskHandler ----------

TaskHandler::TaskHandler(Ped ped)
	: owner{ ped }
{
	if (!ped)
		throw std::invalid_argument{ "Invalid task owner" };
}


void TaskHandler::driveToCoord(Vehicle vehicle, const Vector3& destination, float speed, DrivingStyles mode, float stopRange) const
{
	int imode = static_cast<int>(mode);
	TASK::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(owner.getHandle(), vehicle.getHandle(), destination.x, destination.y, destination.z, speed, static_cast<int>(mode), stopRange);
}


void TaskHandler::followToOffsetOfEntity(const Entity& target, const Vector3& offset, float speed, float stoppingRange, bool persistFollowing, int timeout) const
{
	TASK::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(owner.getHandle(), target.getHandle(), offset.x, offset.y, offset.z, speed, timeout, stoppingRange, persistFollowing);
}


void TaskHandler::goStraightToCoord(const Vector3& destination, float speed, float targetHeading, float distanceToSlide, int timeout) const
{
	TASK::TASK_GO_STRAIGHT_TO_COORD(owner.getHandle(), destination.x, destination.y, destination.z, speed, timeout, targetHeading, distanceToSlide);
}


void TaskHandler::goToEntity(const Entity& target, float distance, float moveBlendRatio, int timeout) const
{
	TASK::TASK_GO_TO_ENTITY(owner.getHandle(), target.getHandle(), timeout, distance, moveBlendRatio, 0, 0);
}


void TaskHandler::shootAt(const Vector3& target, PedFiringPattern pattern, int timeout) const
{
	int ipattern = static_cast<int>(pattern);
	TASK::TASK_SHOOT_AT_COORD(owner.getHandle(), target.x, target.y, target.z, timeout, ipattern);
}


void TaskHandler::shootAt(const Entity& target, PedFiringPattern pattern, int timeout) const
{
	int ipattern = static_cast<int>(pattern);
	TASK::TASK_SHOOT_AT_ENTITY(owner.getHandle(), target.getHandle(), timeout, ipattern);
}


void TaskHandler::startVehicleMission(Vehicle vehicle, Vehicle target, VehicleMissionType missionType, float speed, DrivingStyles drivingStyle, float targetRadius, float straightLineDistance, bool driveAgainstTraffic) const
{
	int imission = static_cast<int>(missionType);
	int istyle = static_cast<int>(drivingStyle);
	TASK::TASK_VEHICLE_MISSION(owner.getHandle(), vehicle.getHandle(), target.getHandle(), imission, speed, istyle, targetRadius, straightLineDistance, driveAgainstTraffic);
}


void TaskHandler::startHeliMission(Vehicle heli, const Entity& target, VehicleMissionType missionType, float maxSpeed, float radius, float targetHeading, int maxHeight, int minHeight, float slowDownDistance, HeliMissionBehaviorFlags behaviorFlags) const
{
	auto params = startMissionGetParams(target, missionType);
	int iflag = static_cast<int>(behaviorFlags);
	TASK::TASK_HELI_MISSION(owner.getHandle(), heli.getHandle(), std::get<1>(params), std::get<0>(params), 0.0f, 0.0f, 0.0f, std::get<2>(params), maxSpeed, radius, targetHeading, maxHeight, minHeight, slowDownDistance, iflag);
}


void TaskHandler::startHeliMission(Vehicle heli, const Vector3& coords, VehicleMissionType missionType, float maxSpeed, float radius, float targetHeading, int maxHeight, int minHeight, float slowDownDistance, HeliMissionBehaviorFlags behaviorFlags) const
{
	int imission = static_cast<int>(missionType);
	int iflag = static_cast<int>(behaviorFlags);
	TASK::TASK_HELI_MISSION(owner.getHandle(), heli.getHandle(), 0, 0, coords.x, coords.y, coords.z, imission, maxSpeed, radius, targetHeading, maxHeight, minHeight, slowDownDistance, iflag);
}


void TaskHandler::startPlaneMission(Vehicle plane, const Entity& target, VehicleMissionType missionType, float angularDrag, float maxHeight, float minHeight, float targetRadius, float targetHeading, bool precise) const
{
	auto params = startMissionGetParams(target, missionType);
	TASK::TASK_PLANE_MISSION(owner.getHandle(), plane.getHandle(), std::get<1>(params), std::get<0>(params), 0.0f, 0.0f, 0.0f, std::get<2>(params), angularDrag, targetRadius, targetHeading, maxHeight, minHeight, precise);
}


void TaskHandler::startPlaneMission(Vehicle plane, const Vector3& coords, VehicleMissionType missionType, float angularDrag, float maxHeight, float minHeight, float targetRadius, float targetHeading, bool precise) const
{
	int imission = static_cast<int>(missionType);
	TASK::TASK_PLANE_MISSION(owner.getHandle(), plane.getHandle(), 0, 0, coords.x, coords.y, coords.z, imission, angularDrag, targetRadius, targetHeading, maxHeight, minHeight, precise);
}


void TaskHandler::startBoatMission(Vehicle boat, const Entity& target, VehicleMissionType missionType, float maxSpeed, DrivingStyles drivingStyle, float targetRadius) const
{
	auto params = startMissionGetParams(target, missionType);
	int istyle = static_cast<int>(drivingStyle);
	TASK::TASK_BOAT_MISSION(owner.getHandle(), boat.getHandle(), std::get<1>(params), std::get<0>(params), 0.0f, 0.0f, 0.0f, std::get<2>(params), maxSpeed, istyle, targetRadius, 7);
}


void TaskHandler::startBoatMission(Vehicle boat, const Vector3& coords, VehicleMissionType missionType, float maxSpeed, DrivingStyles drivingStyle, float targetRadius) const
{
	int imission = static_cast<int>(missionType);
	int istyle = static_cast<int>(drivingStyle);
	TASK::TASK_BOAT_MISSION(owner.getHandle(), boat.getHandle(), 0, 0, coords.x, coords.y, coords.z, imission, maxSpeed, istyle, targetRadius, 7);
}


void TaskHandler::enterVehicle(Vehicle vehicle, VehicleSeat seat, bool running, int mode, int timeout) const
{
	float enter_speed = running ? 2.0f : 1.0f;
	int iseat = static_cast<int>(seat);
	TASK::TASK_ENTER_VEHICLE(owner.getHandle(), vehicle.getHandle(), timeout, iseat, enter_speed, mode, 0);
}


void TaskHandler::parkVehicle(Vehicle vehicle, const Vector3& position, float heading, float radius, int mode, bool keepEngineOn) const
{
	TASK::TASK_VEHICLE_PARK(owner.getHandle(), vehicle.getHandle(), position.x, position.y, position.z, heading, mode, radius, keepEngineOn);
}


std::tuple<Hentity, Hentity, int> TaskHandler::startMissionGetParams(const Entity& target, VehicleMissionType missionType) const
{
	std::tuple<Hentity, Hentity, int> params{ target.getHandle(), 0, static_cast<int>(missionType) };
	if (target.getType() == EntityType::Ped)
		std::swap(std::get<0>(params), std::get<1>(params));
	return params;
}

// ---------- Helper ----------

Ped createPed(Model pedModel, const Vector3& position, float heading, DWORD maxTimeModel, PedType type, bool noPersistent)
{
	if (!pedModel.request(maxTimeModel))
		return 0;
	int itype = static_cast<int>(type);
	Ped presult{ PED::CREATE_PED(itype, pedModel.getHash(), position.x, position.y, position.z, heading, FALSE, TRUE) };
	if (presult && noPersistent)
		presult.markAsNoLongerNeeded(true);
	return presult;
}

Vehicle createVehicle(Model vehicleModel, const Vector3& position, float heading, DWORD maxTimeModel, bool noPersistent)
{
	if (!vehicleModel.request(maxTimeModel))
		return 0;
	Vehicle vresult{ VEHICLE::CREATE_VEHICLE(vehicleModel.getHash(), position.x, position.y, position.z, heading, FALSE, TRUE, TRUE) };
	if (vresult && noPersistent)
		vresult.markAsNoLongerNeeded(true);
	return vresult;
}

Prop createProp(Model propModel, const Vector3& position, float heading, bool dynamic, DWORD maxTimeModel, bool noPersistent)
{
	if (!propModel.request(maxTimeModel))
		return 0;
	Prop oresult{ OBJECT::CREATE_OBJECT(propModel.getHash(), position.x, position.y, position.z, FALSE, TRUE, dynamic) };
	if (!oresult)
		return 0;
	else if (noPersistent)
		oresult.markAsNoLongerNeeded(true);
	oresult.setHeading(heading);
	return oresult;
}

Blip createBlip(const Vector3& position, float radius, int spriteID, int colorID, const std::string& name)
{
	Blip bresult{ HUD::ADD_BLIP_FOR_RADIUS(position.x, position.y, position.z, radius) };
	if (!bresult)
		return 0;
	bresult.setSprite(spriteID);
	bresult.setColor(colorID);
	bresult.modifyName(name);
	return bresult;
}

Blip createBlip(const Vector3& position, int spriteID, int colorID, const std::string& name)
{
	Blip bresult{ HUD::ADD_BLIP_FOR_COORD(position.x, position.y, position.z) };
	if (!bresult)
		return 0;
	bresult.setSprite(spriteID);
	bresult.setColor(colorID);
	bresult.modifyName(name);
	return bresult;
}

Pickup createPickup(Hash type, Model pickupModel, const Vector3& position, const Vector3& rotation, int amount, DWORD maxTimeModel)
{
	if (!pickupModel.request(maxTimeModel))
		return 0;
	Pickup kresult = OBJECT::CREATE_PICKUP_ROTATE(type, position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, 0, amount, 2, TRUE, pickupModel.getHash());
	if (!kresult)
		return 0;
	return kresult;
}