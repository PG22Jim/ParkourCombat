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


UENUM(BlueprintType)
enum class EEnemyReplayCommand: uint8
{
	NormalAttack = 0 UMETA(DisplayName = "NormalAttack"),
	ChargeAttack = 1 UMETA(DisplayName = "ChargeAttack"),
	LaunchAttack = 3 UMETA(DisplayName = "LaunchAttack"),
	LongRangeAttack = 4 UMETA(DisplayName = "LongRangeAttack"),
	Counter = 5 UMETA(DisplayName = "Counter"),
	Heal = 6 UMETA(DisplayName = "Heal"),
	None = 7 UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EEnemyTaskStatus: uint8
{
	None = 0 UMETA(DisplayName = "NormalAttack"),
	Decided = 1 UMETA(DisplayName = "Decided"),
	Executing = 2 UMETA(DisplayName = "Executing"),
};

struct EnemyNormalAttackSequence
{
private:
	int32 NAttackComboExecutionCount = 0;
	int32 IndexToChargeAttackSetting = 99;
	int32 StoredCurrentNAttackCount = 0;
	

public:

	EnemyNormalAttackSequence()
	{
		NAttackComboExecutionCount = 0;
		IndexToChargeAttackSetting = 99;
		StoredCurrentNAttackCount = 0;
	}
	EnemyNormalAttackSequence(int32 StoringNAComboCount, int32 StoringIndexToCA, int32 StoringCurrentNACount)
	{
		NAttackComboExecutionCount = StoringNAComboCount;
		IndexToChargeAttackSetting = StoringIndexToCA;
		StoredCurrentNAttackCount = StoringCurrentNACount;
	}

	int32 GetNAttackComboExecutionCount() const {return NAttackComboExecutionCount;}
	int32 GetIndexToChargeAttackSetting() const {return IndexToChargeAttackSetting;}
	int32 GetStoredCurrentNAttackCount() const {return StoredCurrentNAttackCount;}
	
};