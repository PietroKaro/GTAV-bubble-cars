#ifndef GTA_POOL
#define GTA_POOL

#include <vector>
#include <list>
#include <initializer_list>
#include <stdexcept>
#include <typeinfo>
#include "gtav_types.h"
#include "gta_math.h"
#include "natives.h"
#include "caroz_lib.h"

class Vehicle;
class Blip;
class TaskHandler;

class Model
{
protected:
	Hash hash;

public:
	// Hashes or strings can be implicitly convertible as object's hash
	Model(Hash h) : hash{ h } {}
	Model(const std::string& name) : hash{ MISC::GET_HASH_KEY(name.c_str()) } {}
	Model(const char* name) : Model{ std::string{ name } } {}
	Model() : Model{ Hash{} } {}	// Initialize hash param due to conflict ctors

	Hash getHash() const { return hash; }

	virtual bool isValid() const { return STREAMING::IS_MODEL_VALID(hash); }
	virtual bool hasLoaded() const { return STREAMING::HAS_MODEL_LOADED(hash); }
	virtual void markAsNoLongerNeeded() const { STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash); }

	virtual void request() const { STREAMING::REQUEST_MODEL(hash); }

	// The script yield until the model has loaded or return false if
	// the maximum time for loading is exceeded
	virtual bool request(DWORD maxTime) const;

	virtual bool operator ==(Model other) const { return hash == other.hash; }
	virtual bool operator !=(Model other) const { return hash != other.hash; }
	explicit virtual operator bool() const { return isValid(); }
};

class WeaponAsset : public Model
{
public:
	// Hashes or strings can be implicitly convertible as weapon's hash
	WeaponAsset(HashWeapon hw) : Model{ hw } {}

	// Weapon name like "weapon_..."
	WeaponAsset(const std::string& weaponName) : Model{ weaponName } {}
	WeaponAsset(const char* weaponName) : Model{ weaponName } {}
	WeaponAsset(const std::string& weaponName, DWORD maxTime);
	WeaponAsset(const char* weaponName, DWORD maxTime) : WeaponAsset{ std::string{ weaponName }, maxTime } {}
	WeaponAsset() : WeaponAsset{ HashWeapon{} } {}	// Initialize hash param due to conflict ctors

	bool isValid() const override { return WEAPON::IS_WEAPON_VALID(hash); }
	bool hasLoaded() const override { return WEAPON::HAS_WEAPON_ASSET_LOADED(hash); }
	void markAsNoLongerNeeded() const override { WEAPON::REMOVE_WEAPON_ASSET(hash); }

	void request() const override { WEAPON::REQUEST_WEAPON_ASSET(hash, 31, 26); }
	bool request(DWORD maxTime) const override;

	explicit operator bool() const override { return isValid(); }
};

class GameObject
{
public:
	virtual Hobject getHandle() const = 0;
	virtual void delete_() = 0;
	virtual bool exist() const = 0;

	virtual explicit operator bool() const = 0;

	virtual ~GameObject() {}
};

// Some entity methods (like speed and velocity methods) don't work for prop
class Entity : public GameObject
{
protected:
    Hentity handle;

	// A game handle can be implicitly convertible as entity handle
	Entity(Hentity he) : handle{ he } {}
	Entity() : Entity{ 0 } {}

public:
	Hentity getHandle() const override { return handle; }
	bool exist() const override { return ENTITY::DOES_ENTITY_EXIST(handle); }
	EntityType getType() const { return static_cast<EntityType>(ENTITY::GET_ENTITY_TYPE(handle)); }
	PopulationType getPopoulationType() { return static_cast<PopulationType>(ENTITY::GET_ENTITY_POPULATION_TYPE(handle)); }
	Model getModel() const { return Model{ ENTITY::GET_ENTITY_MODEL(handle) }; }
	Blip getAttachedBlip() const;
	Blip attachBlip(int spriteID = 1, int colorID = 1, const std::string& name = "Enemy") const;

	void delete_() override;

	// Value * 3.6 = kmh, * 2.236936 = mph
	float getSpeed() const { return ENTITY::GET_ENTITY_SPEED(handle); }

	virtual void setMaxSpeed(float speed) const { ENTITY::SET_ENTITY_MAX_SPEED(handle, speed); }
	float getHeading() const { return ENTITY::GET_ENTITY_HEADING(handle); }
	void setHeading(float heading) const { return ENTITY::SET_ENTITY_HEADING(handle, heading); }
	float getHeightAboveGround() const { return ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(handle); }
	Vector3 getVelocity(bool relative) const { return ENTITY::GET_ENTITY_SPEED_VECTOR(handle, relative); }
	Vector3 getPosition() const { return ENTITY::GET_ENTITY_COORDS(handle, TRUE); }
	Vector3 getForwardVector() const { return ENTITY::GET_ENTITY_FORWARD_VECTOR(handle); }

	// Yaw, pitch, roll order in XYZ vector. 0 ZYX, 1 YZY, 2 ZXY, more details on native in natives header
	Vector3 getRotation(int rotationOrder = 2) const { return ENTITY::GET_ENTITY_ROTATION(handle, rotationOrder); }
    void setRotation(const Vector3& rot, int rotationOrder = 2) const { ENTITY::SET_ENTITY_ROTATION(handle, rot.x, rot.y, rot.z, rotationOrder, TRUE); }

	bool isPersistent() const { return ENTITY::IS_ENTITY_A_MISSION_ENTITY(handle); }
	void markPersistent() const { ENTITY::SET_ENTITY_AS_MISSION_ENTITY(handle, FALSE, FALSE); }

	// If true, handle won't be set null so the object will still refers to 
	// a non-persistent entity. For more details see related native
	void markAsNoLongerNeeded(bool leaveHandle);

	// Return true even if the entity is behind another object in screen
	bool isOnScreen() const { return ENTITY::IS_ENTITY_ON_SCREEN(handle); }

	// Player[0 to 200], Ped[100 to 200], Vehicle[0 to 1000], Object[0 to 1000]. If max true, return the max health
	int getHealth(bool max = false) const { return max ? ENTITY::GET_ENTITY_MAX_HEALTH(handle) : ENTITY::GET_ENTITY_HEALTH(handle); }

	// Male[100 - 200], female[0 - 100]
	void setHealth(int health, bool max = false) const { max ? ENTITY::SET_ENTITY_MAX_HEALTH(handle, health) : ENTITY::SET_ENTITY_HEALTH(handle, health, 0); }

	bool isDead() const { return ENTITY::IS_ENTITY_DEAD(handle, FALSE); }
	bool isInWater() const { return ENTITY::IS_ENTITY_IN_WATER(handle); }
	bool isTouching(Entity other) const { return ENTITY::IS_ENTITY_TOUCHING_ENTITY(handle, other.getHandle()); }
	bool isTouching(Model other) const { return ENTITY::IS_ENTITY_TOUCHING_MODEL(handle, other.getHash()); }
	bool isInAir() const { return ENTITY::IS_ENTITY_IN_AIR(handle); }
	bool isVisible() const { return ENTITY::IS_ENTITY_VISIBLE(handle); }

	void toggleVisibility(bool toggle) const { ENTITY::SET_ENTITY_VISIBLE(handle, toggle, FALSE); }
	void toggleInvincibility(bool toggle) const { ENTITY::SET_ENTITY_INVINCIBLE(handle, toggle); }
	void toggleFreezeState(bool toggle) const { ENTITY::FREEZE_ENTITY_POSITION(handle, toggle); }

	void setPosition(const Vector3& pos, bool clearArea = true, bool disablePhysicsDeadPed = false) const
	{ 
		return ENTITY::SET_ENTITY_COORDS(handle, pos.x, pos.y, pos.z, TRUE, disablePhysicsDeadPed, FALSE, clearArea);
	}

	void setVelocity(const Vector3& velocity, bool angular = false) const
	{
		angular ? ENTITY::SET_ENTITY_ANGULAR_VELOCITY(handle, velocity.x, velocity.y, velocity.z) : ENTITY::SET_ENTITY_VELOCITY(handle, velocity.x, velocity.y, velocity.z);
	}

	bool operator ==(Entity other) const { return handle == other.handle; }
	bool operator !=(Entity other) const { return handle != other.handle; }
	explicit operator bool() const override { return exist(); }
};

class Ped : public Entity
{
public:
	// A game handle can be implicitly convertible as ped handle
	Ped(Hped hp) : Entity{ hp } {}
	Ped() : Ped{ 0 } {}

	static Ped player() { return Ped{ PLAYER::PLAYER_PED_ID() }; }

	PedType getType() const { return static_cast<PedType>(PED::GET_PED_TYPE(handle)); }
	Ped clone() const { return PED::CLONE_PED(handle, FALSE, TRUE, TRUE); }
	Vehicle getCurrentVehicle(bool includeLastVehicle = false) const;
	bool isPlayer() const { return *this == player(); }
	bool isHuman() const { return PED::IS_PED_HUMAN(handle); }
	void setIntoVehicle(Vehicle vehicle, VehicleSeat seat) const;
	void setFiringPattern(PedFiringPattern pattern) const { PED::SET_PED_FIRING_PATTERN(handle, static_cast<Hash>(pattern)); }
	void toggleAsEnemy(bool toggle) const { PED::SET_PED_AS_ENEMY(handle, toggle); }
	void resurrect() const { PED::RESURRECT_PED(handle); }
	Model getCauseOfDeath() const { return Model{ PED::GET_PED_CAUSE_OF_DEATH(handle) }; }
	TaskHandler getTaskHandler() const;
	void kill() const { setHealth(0); }

	void setMoney(unsigned short amount) const { PED::SET_PED_MONEY(handle, amount); }
	int getMoney() const { return PED::GET_PED_MONEY(handle); }

	void setSeeingRange(float range) const { PED::SET_PED_SEEING_RANGE(handle, range); }
	void setHearingRange(float range) const { PED::SET_PED_HEARING_RANGE(handle, range); }

	void setCombatMovement(PedCombatMovement cm) const { PED::SET_PED_COMBAT_MOVEMENT(handle, static_cast<int>(cm)); }
	PedCombatMovement getCombatMovement() const { return static_cast<PedCombatMovement>(PED::GET_PED_COMBAT_MOVEMENT(handle)); }
	
	void removeAllWeapons() const { WEAPON::REMOVE_ALL_PED_WEAPONS(handle, TRUE); }
	void setCurrentWeapon(WeaponAsset vehicleWeapon) const { WEAPON::SET_CURRENT_PED_VEHICLE_WEAPON(handle, vehicleWeapon.getHash()); }
	void reloadWeapon() const { WEAPON::MAKE_PED_RELOAD(handle); }
	bool hasWeapon(WeaponAsset weapon) const { return WEAPON::HAS_PED_GOT_WEAPON(handle, weapon.getHash(), FALSE); }
	void toggleInfiniteAmmo(WeaponAsset weapon, bool toggle) const { WEAPON::SET_PED_INFINITE_AMMO(handle, toggle, weapon.getHash()); }
	int getWeaponAmmo(WeaponAsset weapon) const { return WEAPON::GET_AMMO_IN_PED_WEAPON(handle, weapon.getHash()); }
	void setWeaponAmmo(WeaponAsset weapon, int ammo, bool add) const { add ? WEAPON::ADD_AMMO_TO_PED(handle, weapon.getHash(), ammo) : WEAPON::SET_PED_AMMO(handle, weapon.getHash(), ammo, FALSE); }
	WeaponAsset getBestWeapon() const { return WeaponAsset{ WEAPON::GET_BEST_PED_WEAPON(handle, FALSE) }; }
	WeaponAsset getCurrentWeapon(bool isVehicleWeapon = false) const;

	// forceInHand force the ped to change weapon with the given one
	void giveWeapon(WeaponAsset weapon, int ammo, bool forceInHand, bool isVisible = true) const { WEAPON::GIVE_WEAPON_TO_PED(handle, weapon.getHash(), ammo, isVisible, forceInHand); }	// Original native has "isHidden" instead of "isVisible" but the behaviour seems the opposite

	// forceInHand force the ped to keep weapon in hands
	void setCurrentWeapon(WeaponAsset weapon, bool forceInHand) const { WEAPON::SET_CURRENT_PED_WEAPON(handle, weapon.getHash(), forceInHand); }

	// If nullptr, the func checks for any Vehicle.
	// validIfInSeat, if true, will not return true until the Ped is sat in Vehicle
	bool isInVehicle(Vehicle* vehicle, bool validIfInSeat = true) const;

	// Range 0 - 100
	void setArmour(int amount) const { PED::SET_PED_ARMOUR(handle, amount); }
	int getArmour() const { return PED::GET_PED_ARMOUR(handle); }

	// Level of ability from 0 to 2
	void setCombactAbility(int ability) const { PED::SET_PED_COMBAT_ABILITY(handle, ability); }

	// Max 1.0f
	void setDriverAbility(float ability) const { PED::SET_DRIVER_ABILITY(handle, ability); }

	// Max 1.0f
	void setDriverAggressiveness(float aggressiveness) const { PED::SET_DRIVER_AGGRESSIVENESS(handle, aggressiveness); }

	// Range 0 - 1000
	void setShootRate(int rate) const { PED::SET_PED_SHOOT_RATE(handle, rate); }

	// 0 Female, 1 Male
	int getGender() const { return PED::IS_PED_MALE(handle); }
};

class Vehicle : public Entity
{
public:
	// A game handle can be implicitly convertible as vehicle handle
	Vehicle(Hvehicle hv) : Entity{ hv } {}
	Vehicle() : Vehicle{ 0 } {}

	Ped createPedInside(Model pedModel, VehicleSeat seat, DWORD maxTimeModel = 3000, PedType type = PedType::Mission, bool noPersistent = false) const;
	Ped getPedInSeat(VehicleSeat seat) const { return VEHICLE::GET_PED_IN_VEHICLE_SEAT(handle, static_cast<int>(seat), FALSE); }
	bool hasWeapons() const { return VEHICLE::DOES_VEHICLE_HAVE_WEAPONS(handle); }
	VehicleClass getClass() const { return static_cast<VehicleClass>(VEHICLE::GET_VEHICLE_CLASS(handle)); }
	void placeOnGroundProperly() const { VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(handle, 5.0f); }
	int getNumberOfOccupants(bool includeDriver = true, bool includeDead = true) const { return VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(handle, includeDriver, includeDead); }
	bool isSeatFree(VehicleSeat seat, bool isTaskRunningOnSeat = false) const { return VEHICLE::IS_VEHICLE_SEAT_FREE(handle, static_cast<int>(seat), isTaskRunningOnSeat); }
	bool isOnAllWheels() const { return VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(handle); }
	bool isDriveable(bool isOnFireCheck = false) const { return VEHICLE::IS_VEHICLE_DRIVEABLE(handle, isOnFireCheck); }

	void setMaxSpeed(float speed) const override { VEHICLE::SET_VEHICLE_MAX_SPEED(handle, speed); }
	void resetMaxSpeed() const { setMaxSpeed(0.0f); }
	void setForwardSpeed(float speed) const { VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, speed); }

	// Min: -4000  Max: 1000, for details see native in header 
	void setEngineHealth(float health) const { VEHICLE::SET_VEHICLE_ENGINE_HEALTH(handle, health); }
	float getEngineHealth() const { return VEHICLE::GET_VEHICLE_ENGINE_HEALTH(handle); }
	void toggleEngine(bool toggle, bool instantly, bool disableAutostart = false) const { VEHICLE::SET_VEHICLE_ENGINE_ON(handle, toggle, instantly, disableAutostart); }
	bool isEngineRunning() const { return VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(handle); }

	void startAlarm() const { VEHICLE::START_VEHICLE_ALARM(handle); }
	bool isAlarmStateActive() const { return VEHICLE::IS_VEHICLE_ALARM_ACTIVATED(handle); }
	void toggleSiren(bool toggle) const { VEHICLE::SET_VEHICLE_SIREN(handle, toggle); }
	bool isSirenOn() const { return VEHICLE::IS_VEHICLE_SIREN_ON(handle); }
	void toggleSirenSound(bool toggle) const { VEHICLE::SET_VEHICLE_HAS_MUTED_SIRENS(handle, !toggle); }
	bool isSirenSoundOn() const { return VEHICLE::IS_VEHICLE_SIREN_AUDIO_ON(handle); }

	pk99::RGBColor getCustomPrimaryColor() const;
	pk99::RGBColor getCustomSecondaryColor() const;
	void setCustomPrimaryColor(const pk99::RGBColor& color) const { VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(handle, color[0], color[1], color[2]); }
	void setCustomSecondaryColor(const pk99::RGBColor& color) const { VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(handle, color[0], color[1], color[2]); }

	// Must be used before startAlarm()
	void setAlarmState(bool state) const { VEHICLE::SET_VEHICLE_ALARM(handle, state); }

	// Mode: NORMAL or HELDDOWN
	void soundHorn(int duration, std::string mode, bool forever = false) { VEHICLE::START_VEHICLE_HORN(handle, duration, MISC::GET_HASH_KEY(mode.c_str()), forever); }

	// Driver not included
	int getMaxNumberOfPassengersAllowed() const { return VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(handle); }

	// Modify engine power
	void modifyTopSpeed(float speed) const { VEHICLE::MODIFY_VEHICLE_TOP_SPEED(handle, speed); }
};

class Prop : public Entity
{
public:
	// A game handle can be implicitly convertible as prop handle
	Prop(Hprop hp) : Entity{ hp } {}
	Prop() : Prop{ 0 } {}

	void placeOnGroundProperly() const { OBJECT::PLACE_OBJECT_ON_GROUND_PROPERLY(handle); }
	void toggleAsBall(bool toggle) const { OBJECT::SET_IS_OBJECT_BALL(handle, toggle); }
};

class Blip : public GameObject
{
	// All instances are added here. They will be deleted once this script call delete_()
	// This is for try keep a kind of tracking of all blips in game
	static std::list<Blip> all;
	Hblip handle;

public:
	// A game handle can be implicitly convertible as blip handle
	Blip(Hblip hb) : handle{ hb } { all.push_back(*this); }
	Blip() : Blip{ 0 } {}

	// Always need a check for a valid Blip object
	static std::vector<Blip> getAll() { return std::vector<Blip>{ all.begin(), all.end() }; }

	Hblip getHandle() const override { return handle; }
	bool exist() const override { return HUD::DOES_BLIP_EXIST(handle); }

	// It seems works similar as MarkAsNoLongerNeeded()
	void delete_() override;

	// Value 0 - 255
	void setAlpha(int alpha) const { HUD::SET_BLIP_ALPHA(handle, alpha); }
	int getAlpha() const { return HUD::GET_BLIP_ALPHA(handle); }
	
	// Return an integer rot instead of float
	int getRotation() const { return HUD::GET_BLIP_ROTATION(handle); }
	void setRotation(float rot) const { HUD::SET_BLIP_ROTATION_WITH_FLOAT(handle, rot); }
	
	void setPosition(const Vector3& newPos) const { HUD::SET_BLIP_COORDS(handle, newPos.x, newPos.y, newPos.z); }
	Vector3 getPosition() const { return HUD::GET_BLIP_COORDS(handle); }
	void setSprite(int spriteID) const { HUD::SET_BLIP_SPRITE(handle, spriteID); }
	int getSprite() const { return HUD::GET_BLIP_SPRITE(handle); }
	void setColor(int colorID) const { HUD::SET_BLIP_COLOUR(handle, colorID); }
	int getColor() const { return HUD::GET_BLIP_COLOUR(handle); }

	bool isOnMinimap() const { return HUD::IS_BLIP_ON_MINIMAP(handle); }
	bool isFlashing() const { return HUD::IS_BLIP_FLASHING(handle); }

	void toggleVisibilityInLegend(bool toggle) const { HUD::SET_BLIP_HIDDEN_ON_LEGEND(handle, !toggle); }
	void toggleFlash(bool toggle) const { HUD::SET_BLIP_FLASHES(handle, toggle); }
	void toggleRoute(bool toggle) const { HUD::SET_BLIP_ROUTE(handle, toggle); }

	void modifyName(const std::string& newName) const;
	void setRouteColor(int colorID) const { HUD::SET_BLIP_ROUTE_COLOUR(handle, colorID); }
	void setPriority(int priority) const { HUD::SET_BLIP_PRIORITY(handle, priority); }
	void setDisplayBehaviour(BlipDisplayBehaviour beh) const { HUD::SET_BLIP_DISPLAY(handle, static_cast<int>(beh)); }

	bool operator ==(Blip other) const { return handle == other.handle; }
	bool operator !=(Blip other) const { return handle != other.handle; }
	explicit operator bool() const override { return exist(); }
};

class Pickup : public GameObject
{
	Hpickup handle;

public:
	// A game handle can be implicitly convertible as pickup handle
	Pickup(Hpickup hp) : handle{ hp } {}
	Pickup() : Pickup{ 0 } {}

	Hpickup getHandle() const override { return handle; }
	bool exist() const override { return OBJECT::DOES_PICKUP_EXIST(handle); }
	void delete_() override { OBJECT::REMOVE_PICKUP(handle); }
	Vector3 getPosition() const { return OBJECT::GET_PICKUP_COORDS(handle); }
	void forceRegeneration() const { OBJECT::FORCE_PICKUP_REGENERATE(handle); }
	void setRegenerationTime(int time) const;
 
	bool operator ==(Pickup other) const { return handle == other.handle; }
	bool operator !=(Pickup other) const { return handle != other.handle; }
	explicit operator bool() const override { return exist(); }
};

class TaskHandler
{
	Ped owner;

	std::tuple<Hentity, Hentity, int> startMissionGetParams(const Entity& target, VehicleMissionType missionType) const;

public:
	TaskHandler(Ped ped);

	// Change properties while ped doing drive tasks
	void setDriveTaskSpeed(float speed) const { TASK::SET_DRIVE_TASK_CRUISE_SPEED(owner.getHandle(), speed); }
	void setDriveTaskDrivingMode(DrivingStyles style) const { TASK::SET_DRIVE_TASK_DRIVING_STYLE(owner.getHandle(), static_cast<int>(style)); }

	bool isTaskActive(int cTaskIndex) const { return TASK::GET_IS_TASK_ACTIVE(owner.getHandle(), cTaskIndex); }
	void standStill(int ms) const { TASK::TASK_STAND_STILL(owner.getHandle(), ms); }
	void leaveVehicle(Vehicle vehicle, LeaveVehicleFlags flag) const { TASK::TASK_LEAVE_VEHICLE(owner.getHandle(), vehicle.getHandle(), static_cast<int>(flag)); }
	void toggleBlockingOfNonTemporaryEvents(bool toggle) const { TASK::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(owner.getHandle(), toggle); }
	void clearTasks(bool immediately) const { immediately ? TASK::CLEAR_PED_TASKS_IMMEDIATELY(owner.getHandle()) : TASK::CLEAR_PED_TASKS(owner.getHandle()); }
	void combatPed(Ped target, int timeout = -1) const { timeout == -1 ? TASK::TASK_COMBAT_PED(owner.getHandle(), target.getHandle(), 0, 16) : TASK::TASK_COMBAT_PED_TIMED(owner.getHandle(), target.getHandle(), timeout, 0); }

	void driveToCoord(Vehicle vehicle, const Vector3& destination, float speed, DrivingStyles mode, float stopRange) const;
	void followToOffsetOfEntity(const Entity& target, const Vector3& offset, float speed, float stoppingRange, bool persistFollowing = false, int timeout = -1) const;
	void goStraightToCoord(const Vector3& destination, float speed, float targetHeading, float distanceToSlide, int timeout = -1) const;
	void goToEntity(const Entity& target, float distance, float moveBlendRatio, int timeout = -1) const;
	void shootAt(const Vector3& target, PedFiringPattern pattern, int timeout = -1) const;
	void shootAt(const Entity& target, PedFiringPattern pattern, int timeout = -1) const;
	
	void startVehicleMission(Vehicle vehicle, Vehicle target, VehicleMissionType missionType, float speed, DrivingStyles drivingStyle, float targetRadius, float straightLineDistance = 5.0f, bool driveAgainstTraffic = false) const;
	void startHeliMission(Vehicle heli, const Entity& target, VehicleMissionType missionType, float maxSpeed, float radius, float targetHeading, int maxHeight, int minHeight, float slowDownDistance, HeliMissionBehaviorFlags behaviorFlags) const;
	void startHeliMission(Vehicle heli, const Vector3& coords, VehicleMissionType missionType, float maxSpeed, float radius, float targetHeading, int maxHeight, int minHeight, float slowDownDistance, HeliMissionBehaviorFlags behaviorFlags) const;
	void startPlaneMission(Vehicle plane, const Entity& target, VehicleMissionType missionType, float angularDrag, float maxHeight, float minHeight, float targetRadius = 0.0f, float targetHeading = 0.0f, bool precise = true) const;
	void startPlaneMission(Vehicle plane, const Vector3& coords, VehicleMissionType missionType, float angularDrag, float maxHeight, float minHeight, float targetRadius = 0.0f, float targetHeading = 0.0f, bool precise = true) const;
	void startBoatMission(Vehicle boat, const Entity& target, VehicleMissionType missionType, float maxSpeed, DrivingStyles drivingStyle, float targetRadius = 10.0f) const;
	void startBoatMission(Vehicle boat, const Vector3& coords, VehicleMissionType missionType, float maxSpeed, DrivingStyles drivingStyle, float targetRadius = 10.0f) const;

	// mode: 1 Normal, 3 Teleport near vehicle, 16 Warp
	void enterVehicle(Vehicle vehicle, VehicleSeat seat, bool running = false, int mode = 1, int timeout = -1) const;

	// mode: 0 Ignore heading, 1 Park forward, 2 Park backwards
	void parkVehicle(Vehicle vehicle, const Vector3& position, float heading, float radius, int mode = 1, bool keepEngineOn = false) const;
};

// maxTimeModel: return invalid ped if the time to request model exceeds this param
Ped createPed(Model pedModel, const Vector3& position, float heading = 0.0f, DWORD maxTimeModel = 3000, PedType type = PedType::Mission, bool noPersistent = false);
Vehicle createVehicle(Model vehicleModel, const Vector3& position, float heading = 0.0f, DWORD maxTimeModel = 3000, bool noPersistent = false);
Prop createProp(Model propModel, const Vector3& position, float heading, bool dynamic, DWORD maxTimeModel = 3000, bool noPersistent = false);
Pickup createPickup(Hash type, Model pickupModel, const Vector3& position, const Vector3& rotation, int amount, DWORD maxTimeModel = 3000);

Blip createBlip(const Vector3& position, float radius, int spriteID = 1, int colorID = 1, const std::string& name = "Enemy");
Blip createBlip(const Vector3& position, int spriteID = 1, int colorID = 1, const std::string& name = "Enemy");
#endif // !GTA_POOL