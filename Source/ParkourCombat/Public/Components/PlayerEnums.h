// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

UENUM(BlueprintType)
enum class ParkourStatus: uint8
{
	Idle = 0 UMETA(DisplayName = "Idle"),
	Walking = 1 UMETA(DisplayName = "Walking"),
	Running = 2 UMETA(DisplayName = "Running"),
	VaultThroughSmallWall = 3 UMETA(DisplayName = "VaultThroughSmallWall"),
	SlideThrough = 4 UMETA(DisplayName = "SlideThrough"),
	QuickWallClimb = 5 UMETA(DisplayName = "QuickClimb"),
	
	//Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class CombatStatus: uint8
{
	Idle = 0 UMETA(DisplayName = "Idle"),
	NormalAttack = 1 UMETA(DisplayName = "NormalAttack"),
	PreAttacking = 2 UMETA(DisplayName = "PreAttacking"),
	Recovering = 3 UMETA(DisplayName = "Recovering"),
	BeforeRecovering = 4 UMETA(DisplayName = "Recovering"),
	Parry = 5 UMETA(DisplayName = "Parry"),
};


UENUM(BlueprintType)
enum class ParryStatus: uint8
{
	Idle = 0 UMETA(DisplayName = "Idle"),
	BeginParry = 1 UMETA(DisplayName = "BeginParry"),
	Parrying = 2 UMETA(DisplayName = "Parrying"),
	CancelParry = 3 UMETA(DisplayName = "CancelParry"),
};





//ENUM_RANGE_BY_COUNT(ParkourStatus, ParkourStatus::Count);