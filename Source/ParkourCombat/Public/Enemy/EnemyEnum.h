// Copyright © 2023 by Jim Chen. All rights reserved.


#pragma once

UENUM(BlueprintType)
enum class EnemyAttackType: uint8
{
	Blockable = 0 UMETA(DisplayName = "Blockable"),
	UnBlockable = 1 UMETA(DisplayName = "UnBlockable"),
};

UENUM(BlueprintType)
enum class EnemyDifficulty: uint8
{
	Easy = 0 UMETA(DisplayName = "Easy"),
	Normal = 1 UMETA(DisplayName = "Normal"),
	Hard = 2 UMETA(DisplayName = "Hard"),
};

UENUM(BlueprintType)
enum class EnemyActionCommand: uint8
{
	Attack = 0 UMETA(DisplayName = "Attack"),
	Idle = 1 UMETA(DisplayName = "Idle"),
	Counter = 2 UMETA(DisplayName = "Counter"),
	Heal = 3 UMETA(DisplayName = "Heal"),
};

UENUM(BlueprintType)
enum class EnemyAttackCategory: uint8
{
	NormalAttack = 0 UMETA(DisplayName = "NormalAttack"),
	ChargeAttack = 1 UMETA(DisplayName = "ChargeAttack"),
	LaunchAttack = 2 UMETA(DisplayName = "LaunchAttack"),
	LongRangeAttack = 3 UMETA(DisplayName = "LongRangeAttack"),
	None = 4 UMETA(DisplayName = "None"),
};
