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
	BeforeRecovering = 4 UMETA(DisplayName = "BeforeRecovering"),
	Parry = 5 UMETA(DisplayName = "Parry"),
	BeginParry = 6 UMETA(DisplayName = "BeginParry"),
	CancelParry = 7 UMETA(DisplayName = "CancelParry"),
	Block = 8 UMETA(DisplayName = "Block"),
	SpecialAttack = 9 UMETA(DisplayName = "SpecialAttack"),
	ReceiveDamage = 10 UMETA(DisplayName = "ReceiveDamage"),
	ChargeAttack = 11 UMETA(DisplayName = "ChargeAttack"),
	LaunchAttack = 12 UMETA(DisplayName = "LaunchAttack"),
	RangeAttack = 13 UMETA(DisplayName = "RangeAttack"),
	Healing = 13 UMETA(DisplayName = "Healing"),
};


// UENUM(BlueprintType)
// enum class ParryStatus: uint8
// {
// 	Idle = 0 UMETA(DisplayName = "Idle"),
// 	BeginParry = 1 UMETA(DisplayName = "BeginParry"),
// 	Parrying = 2 UMETA(DisplayName = "Parrying"),
// 	CancelParry = 3 UMETA(DisplayName = "CancelParry"),
// };





//ENUM_RANGE_BY_COUNT(ParkourStatus, ParkourStatus::Count);