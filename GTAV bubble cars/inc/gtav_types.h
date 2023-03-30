#ifndef GTA_TYPES
#define GTA_TYPES

/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#include <windows.h>
#include <string>
#include <tuple>

using GameHandle	= int;
using Hplayer		= GameHandle;
using Hobject		= GameHandle;
using Hentity		= Hobject;
using Hvehicle		= Hentity;
using Hped			= Hentity;
using Hprop			= Hentity;
using Hblip			= Hobject;
using Hpickup		= Hobject;
using Hash			= DWORD;
using HashVehicle	= Hash;
using HashPed		= Hash;
using HashProp		= Hash;
using HashWeapon	= Hash;
using HashPickup	= Hash;
using Void			= DWORD;
using Any			= DWORD;
using Hfire			= GameHandle;
using Cam			= int;
using Interior		= int;
using ScriptHandle	= GameHandle;

enum class EntityType
{
	Invalid,
	Ped,
	Vehicle,
	Object,
};

enum class PedType
{
	PlayerZero,
	PlayerOne,
	NetworkPlayer,
	PlayerTwo,
	CivMale,
	CivFemale,
	Cop,
	GangAlbanian,
	GangBikerOne,
	GangBikerTwo,
	GangItalian,
	GangRussianOne,
	GangRussianTwo,
	GangIrish,
	GangJamaican,
	GangAfricanAmerican,
	GangKorean,
	GangChineseJapanese,
	GangPuertoRican,
	Dealer,
	Medic,
	Fireman,
	Criminal,
	Bum,
	Prostitute,
	Special,
	Mission,
	Swat,
	Animal,
	Army,
};

enum class PopulationType
{
	Unknown,
	RandomPermanent,
	RandomParked,
	RandomPatrol,
	RandomScenario,
	RandomAmbient,
	Permanent,
	Mission,
	Replay,
	Cache,
	Tool,
};

enum class VehicleSeat
{
	None = -3,
	Any,
	Driver,
	Passenger,
	LeftRear,
	RightRear,
	ExtraSeat1,
	ExtraSeat2,
	ExtraSeat3,
	ExtraSeat4,
	ExtraSeat5,
	ExtraSeat6,
	ExtraSeat7,
	ExtraSeat8,
	ExtraSeat9,
	ExtraSeat10,
	ExtraSeat11,
	ExtraSeat12,
};

enum class PedFiringPattern : Hash
{
	BurstFire = 0xD6FF6D61,
	BurstFireInCover = 0x026321F1,
	BurstFireDriveBy = 0xD31265F2,
	FromGround = 0x2264E5D6,
	DelayFireByOneSec = 0x7A845691,
	FullAuto = 0xC6EE6B4C,
	SingleShot = 0x5D60E4E0,
	BurstFirePistol = 0xA018DB8A,
	BurstFireSMG = 0xD10DADEE,
	BurstFireRifle = 0x9C74B406,
	BurstFireMG = 0xB573C5B4,
	BurstFirePumpShotgun = 0x00BAC39B,
	BurstFireHeli = 0x914E786F,
	BurstFireMicro = 0x42EF03FD,
	BurstFireShortBursts = 0x1A92D7DF,
	BurstFireTank = 0xE2CA3A71,
};

enum class PedCombatMovement
{
	Stationary,
	Defensive,
	WillAdvance,
	WillRetreat,
};

enum class VehicleClass
{
	Compacts,
	Sedans,
	SUVs,
	Coupes,
	Muscle,
	SportsClassics,
	Sports,
	Super,
	Motorcycles,
	OffRoad,
	Industrial,
	Utility,
	Vans,
	Cycles,
	Boats,
	Helicopters,
	Planes,
	Service,
	Emergency,
	Military,
	Commercial,
	Trains,
};

enum class BlipDisplayBehaviour
{
	NotShown,
	MainMapMiniMapSelectableOnMap = 2,
	MainMapSelectableOnMap,
	Minimap = 5,
	MainMapMiniMapNoSelectable = 8,
};

// There are unk flags
enum class LeaveVehicleFlags
{
	CloseDoor,
	TeleportOutside = 16,
	CloseDoorSlower = 64,
	KeepDoorOpen = 256,
	ThrowingHimselfOut = 4160,
	ExitFromPassengerSeat = 262144,
};

enum class VehicleMissionType
{
	Cruise = 1,
	Ram = 2,
	Block = 3,
	GoTo = 4,
	Stop = 5,
	Attack = 6,
	Follow = 7,
	Flee = 8,
	Circle = 9,
	Escort = 12,
	FollowRecording = 15,
	PoliceBehaviour = 16,
	Land = 19,
	Land2 = 20,
	Crash = 21,
	PullOver = 22,
	HeliProtect = 23
};

// There are unk flags
enum class HeliMissionBehaviorFlags : DWORD
{
	ForceHeadingToFaceE = 0x1,
	TightCirclesAroundDestination = 0x4,		// Works if target are coords
	CircleAroundDestinationFacingTowards = 0x10,
	NormalAndLand = 0x20,						// Used in VehicleMissionType::GoTo
	IgnoreAllObstacles = 0x40,
	DirectionCirclingToClockwise = 0x800,		// Used in VehicleMissionType::Circle
	FlyingCloserToGround = 0x1000,
};

// There are unk flags
enum class DrivingStyles : DWORD
{
	StopBeforeVehicles = 0x1,
	StopBeforePeds = 0x2,
	AvoidVehicles = 0x4,
	AvoidEmptyVehicles = 0x8,
	AvoidPeds = 0x10,
	AvoidObjects = 0x20,
	StopAtTrafficLight = 0x80,
	UseBlinkers = 0x100,
	AllowGoingWrongWay = 0x200,
	GoBackwards = 0x400,
	TakeShortestPath = 0x40000,
	IgnoreRoads = 0x400000,
	IgnoreAllPath = 0x1000000,
	AvoidHighwaysWhenPossible = 0x20000000,

	AvoidTrafficExtremely = StopBeforePeds | AvoidVehicles,
	SometimesOvertakeTraffic = StopBeforeVehicles | AvoidVehicles,
	Rushed = StopBeforeVehicles | AvoidVehicles | AvoidObjects,
	Normal = StopBeforeVehicles | StopBeforePeds,
	AvoidTraffic = AvoidVehicles | AvoidObjects,
};

constexpr HeliMissionBehaviorFlags operator |(HeliMissionBehaviorFlags left, HeliMissionBehaviorFlags right)
{
	return static_cast<HeliMissionBehaviorFlags>(static_cast<int>(left) | static_cast<int>(right));
}

constexpr DrivingStyles operator |(DrivingStyles left, DrivingStyles right)
{
	return static_cast<DrivingStyles>(static_cast<int>(left) | static_cast<int>(right));
}

#endif // !GTA_TYPES