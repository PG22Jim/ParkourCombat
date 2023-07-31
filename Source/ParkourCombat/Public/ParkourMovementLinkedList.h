// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerEnums.h"


struct PlayerFrameData
{
private:
	FTransform TransformData;
	ParkourStatus CurrentParkourStatus;
	CombatStatus CurrentCombatStatus;
	UAnimMontage* CurrentMontage;
	float CurrentAnimationTime;
	

public:

	PlayerFrameData();
	PlayerFrameData(FTransform StoringTransform, ParkourStatus CurrentStatus, CombatStatus StoringCombatStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime);

	FTransform GetTransformData() const {return TransformData;}
	ParkourStatus GetCurrentParkourStatus() const {return CurrentParkourStatus;}
	CombatStatus GetCurrentCombatStatus() const {return CurrentCombatStatus;}
	UAnimMontage* GetCurrentMontage() const {return CurrentMontage;}
	float GetCurrentAnimationTime() const {return CurrentAnimationTime;}
	
};

class ParkourPositionData
{
protected:
	
	
public:


	ParkourPositionData* NextFrameData;
	ParkourPositionData* PreviousFrameData;
	PlayerFrameData CurrentPlayerFrameData;

	

	ParkourPositionData();
	ParkourPositionData(const FTransform StoringTransform);
	ParkourPositionData(const FTransform StoringTransform, ParkourPositionData* StoringNextTransform);
	ParkourPositionData(FTransform StoringTransform, ParkourStatus CurrentStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime);
	ParkourPositionData(FTransform StoringTransform, ParkourStatus CurrentStatus, CombatStatus CurrentCombatStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime);
		
	
};



/**
 * 
 */
class PARKOURCOMBAT_API ParkourMovementLinkedList
{
public:
	ParkourMovementLinkedList();
	~ParkourMovementLinkedList();
	
	void AddAtTail(const FTransform DataToAdd);
	void AddAtHead(const FTransform DataToAdd);
	
	ParkourPositionData* GetHead() const {return HeadData;}
	ParkourPositionData* GetTail() const {return TailData;}
	int32 GetSizeNum() const {return ListSize;}

	ParkourPositionData* GetAt(const int32 IndexToGet);

	void RemoveTailData();
	
	void ClearList();

	

private:

	ParkourPositionData* HeadData;
	ParkourPositionData* TailData;

	int32 ListSize;
	
};


class PARKOURCOMBAT_API BackTrackData_Stack
{
public:
	BackTrackData_Stack();
	~BackTrackData_Stack();
	
	ParkourPositionData* Peek() const {return TopData;}
	int32 GetSizeNum() const {return ListSize;}

	bool IsEmpty() const {return TopData == nullptr;} 
	
	void Push(const FTransform DataToAdd, const ParkourStatus StoringParkourStatus, const CombatStatus StoringCombatStatus, UAnimMontage* StoringAnimation ,const float StoringAnimationTime);
	
	void Pop();
	void RemoveTailData();

	void ClearList();

	

private:

	ParkourPositionData* TopData = nullptr;
	ParkourPositionData* TailData = nullptr;


	int32 ListSize;

	int32 MaxListSize = 700;
	
};
