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
//ENUM_RANGE_BY_COUNT(ParkourStatus, ParkourStatus::Count);