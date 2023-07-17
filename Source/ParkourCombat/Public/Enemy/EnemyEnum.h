// Copyright © 2023 by Jim Chen. All rights reserved.


#pragma once

UENUM(BlueprintType)
enum class EnemyAttackType: uint8
{
	Blockable = 0 UMETA(DisplayName = "Blockable"),
	UnBlockable = 1 UMETA(DisplayName = "UnBlockable"),
};