// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter/SwordProjectile.h"
#include "Components/PlayerEnums.h"
#include "Enemy/EnemyEnum.h"


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

struct EnemyFrameData
{
private:
	FTransform TransformData;
	CombatStatus CurrentCombatStatus;
	UAnimMontage* CurrentMontage;
	bool IsRunning;
	float CurrentAnimationTime;
	EEnemyReplayCommand FrameReplayCommand;
	EnemyNormalAttackSequence FrameNAData;
	EEnemyTaskStatus FrameTaskStatus;
	bool IsExecutingTask;

public:

	EnemyFrameData();
	EnemyFrameData(FTransform StoringTransform, CombatStatus StoringCombatStatus, UAnimMontage* PlayingMontage, bool bRunning, float StoringAnimationTime,
		EEnemyReplayCommand StoringReplayCommand, EnemyNormalAttackSequence StoringFrameNAData, EEnemyTaskStatus StoringFrameTaskStatus, bool CurrentExecuteTask);

	FTransform GetTransformData() const {return TransformData;}
	CombatStatus GetCurrentCombatStatus() const {return CurrentCombatStatus;}
	UAnimMontage* GetCurrentMontage() const {return CurrentMontage;}
	bool GetIsRunning() const {return IsRunning;}
	float GetCurrentAnimationTime() const {return CurrentAnimationTime;}
	EEnemyReplayCommand GetFrameReplayCommand() const {return FrameReplayCommand;}
	EnemyNormalAttackSequence GetFrameNAData() const {return FrameNAData;}
	EEnemyTaskStatus GetFrameTaskStatus() const {return FrameTaskStatus;}
	bool GetIsExecutingTask() const {return IsExecutingTask;}
	
};

class ParkourPositionData
{
protected:
	
	
public:


	ParkourPositionData* NextFrameData;
	ParkourPositionData* PreviousFrameData;
	PlayerFrameData CurrentPlayerFrameData;
	TArray<FProjectileFrameData> ProjectileListDataSet;
	EnemyFrameData CurrentEnemyFrameData;
	

	ParkourPositionData();
	ParkourPositionData(const FTransform StoringTransform);
	ParkourPositionData(const FTransform StoringTransform, ParkourPositionData* StoringNextTransform);
	ParkourPositionData(FTransform StoringTransform, ParkourStatus CurrentStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime, EnemyFrameData StoringEnemyFrameData);\
	
	ParkourPositionData(const FTransform StoringTransform, ParkourStatus CurrentStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime, TArray< FProjectileFrameData> StoringProjectileListDataSet, EnemyFrameData StoringEnemyFrameData);
	ParkourPositionData(FTransform StoringTransform, ParkourStatus CurrentStatus, CombatStatus CurrentCombatStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime,
		TArray<FProjectileFrameData> StoringProjectileListDataSet, EnemyFrameData StoringEnemyFrameData);
		
	
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
	ParkourPositionData* GetTailData() const {return TailData;}
	int32 GetSizeNum() const {return ListSize;}

	bool IsEmpty() const {return TopData == nullptr;} 
	
	void Push(const FTransform DataToAdd, const ParkourStatus StoringParkourStatus, const CombatStatus StoringCombatStatus, UAnimMontage* StoringAnimation,
		const float StoringAnimationTime, TArray< FProjectileFrameData> StoringProjectileListDataSet, EnemyFrameData StoringEnemyFrameData);
	
	void Pop();
	void RemoveTailData();

	void ClearList();

	

private:

	ParkourPositionData* TopData = nullptr;
	ParkourPositionData* TailData = nullptr;


	int32 ListSize;

	int32 MaxListSize = 700;
	
};
