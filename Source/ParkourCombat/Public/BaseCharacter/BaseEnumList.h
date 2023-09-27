// Copyright © 2023 by Jim Chen. All rights reserved.


#pragma once

// damage type enum
UENUM(BlueprintType)
enum class CharacterDamageType: uint8
{
	LightDamage = 0 UMETA(DisplayName = "LightDamage"),
	HeavyDamage = 1 UMETA(DisplayName = "HeavyDamage"),
	FlyingDamage = 2 UMETA(DisplayName = "FlyingDamage"),
};