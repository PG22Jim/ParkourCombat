// Copyright © 2023 by Jim Chen. All rights reserved.


#include "ParkourMovementLinkedList.h"

#include "Kismet/KismetSystemLibrary.h"

PlayerFrameData::PlayerFrameData()
{
	TransformData = FTransform();
	CurrentMontage = nullptr;
	CurrentParkourStatus = ParkourStatus::Idle;
	CurrentCombatStatus = CombatStatus::Idle;
	CurrentAnimationTime = 0.0f;
	FramePlayerHealth = 0;
	FramePlayerStamina = 0;
	FramePlayerRevengeMeter = 0;
	FrameRageGage = 0;
	FramePlayerWeaponData = WeaponRewindData();
	FrameComboCount = 0;
}

PlayerFrameData::PlayerFrameData(FTransform StoringTransform, ParkourStatus CurrentStatus,
	CombatStatus StoringCombatStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime, float CurrentPlayerHealth, float CurrentPlayerStamina, float CurrentPlayerRevengeMeter, float CurrentRageGage, WeaponRewindData CurrentWeaponData, int32 ComboCount)
{
	TransformData = StoringTransform;
	CurrentParkourStatus = CurrentStatus;
	CurrentCombatStatus = StoringCombatStatus;
	CurrentMontage = PlayingMontage;
	CurrentAnimationTime = StoringAnimationTime;
	FramePlayerHealth = CurrentPlayerHealth;
	FramePlayerStamina = CurrentPlayerStamina;
	FramePlayerRevengeMeter = CurrentPlayerRevengeMeter;
	FrameRageGage = CurrentRageGage;
	FramePlayerWeaponData = CurrentWeaponData;
	FrameComboCount = ComboCount;
}


EnemyFrameData::EnemyFrameData()
{
	TransformData = FTransform();
	CurrentMontage = nullptr;
	CurrentCombatStatus = CombatStatus::Idle;
	IsRunning = false;
	CurrentAnimationTime = 0.0f;
	FrameReplayCommand = EEnemyReplayCommand::None;
	FrameNAData = EnemyNormalAttackSequence();
	FrameTaskStatus = EEnemyTaskStatus::None;
	IsExecutingTask = false;
	FrameHealth = 0;
	FrameRageValue = 0;
	FrameWeaponData = WeaponRewindData();
}

EnemyFrameData::EnemyFrameData(FTransform StoringTransform,
                               CombatStatus StoringCombatStatus, UAnimMontage* PlayingMontage, bool bRunning, float StoringAnimationTime,
                               EEnemyReplayCommand StoringReplayCommand, EnemyNormalAttackSequence StoringFrameNAData,
                               EEnemyTaskStatus StoringFrameTaskStatus, bool CurrentExecuteTask, float CurrentHealth, float CurrentRageValue, WeaponRewindData CurrentWeaponData)
{
	TransformData = StoringTransform;
	CurrentCombatStatus = StoringCombatStatus;
	CurrentMontage = PlayingMontage;
	IsRunning = bRunning;
	CurrentAnimationTime = StoringAnimationTime;
	FrameReplayCommand = StoringReplayCommand;
	FrameNAData = StoringFrameNAData;
	FrameTaskStatus = StoringFrameTaskStatus;
	IsExecutingTask = CurrentExecuteTask;
	FrameHealth = CurrentHealth;
	FrameRageValue = CurrentRageValue;
	FrameWeaponData = CurrentWeaponData;
}

ParkourPositionData::ParkourPositionData()
{
	NextFrameData = nullptr;
	PreviousFrameData = nullptr;
	CurrentPlayerFrameData = PlayerFrameData();
	CurrentEnemyFrameData = EnemyFrameData();
}

ParkourPositionData::ParkourPositionData(const FTransform StoringTransform)
{
	NextFrameData = nullptr;
	PreviousFrameData = nullptr;
	CurrentPlayerFrameData = PlayerFrameData(StoringTransform, ParkourStatus::Idle, CombatStatus::Idle, nullptr, 0.0f,0,0,0,0, WeaponRewindData(), 0);
	CurrentEnemyFrameData = EnemyFrameData();
}

ParkourPositionData::ParkourPositionData(const FTransform StoringTransform, ParkourPositionData* StoringNextTransform)
{
	NextFrameData = StoringNextTransform;
	PreviousFrameData = nullptr;
	CurrentPlayerFrameData = PlayerFrameData(StoringTransform, ParkourStatus::Idle, CombatStatus::Idle, nullptr, 0.0f,0,0,0,0, WeaponRewindData(), 0);
}

ParkourPositionData::ParkourPositionData(const FTransform StoringTransform,
	ParkourStatus CurrentStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime, EnemyFrameData StoringEnemyFrameData, float CurrentPlayerHealth, float CurrentPlayerStamina, float CurrentPlayerRevengeMeter, float CurrentRageGage, WeaponRewindData CurrentPlayerWeaponData, int32 PlayerFrameComboCount)
{
	NextFrameData = nullptr;
	PreviousFrameData = nullptr;
	CurrentPlayerFrameData = PlayerFrameData(StoringTransform, CurrentStatus, CombatStatus::Idle, PlayingMontage, StoringAnimationTime, CurrentPlayerHealth, CurrentPlayerStamina, CurrentPlayerRevengeMeter, CurrentRageGage, CurrentPlayerWeaponData, PlayerFrameComboCount);
	CurrentEnemyFrameData = StoringEnemyFrameData;
}

ParkourPositionData::ParkourPositionData(const FTransform StoringTransform,
	ParkourStatus CurrentStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime, TArray< FProjectileFrameData> StoringProjectileListDataSet, EnemyFrameData StoringEnemyFrameData, float CurrentPlayerHealth, float CurrentPlayerStamina, float CurrentPlayerRevengeMeter, float CurrentRageGage, WeaponRewindData CurrentPlayerWeaponData, int32 PlayerFrameComboCount)
{
	NextFrameData = nullptr;
	PreviousFrameData = nullptr;
	CurrentPlayerFrameData = PlayerFrameData(StoringTransform, CurrentStatus, CombatStatus::Idle, PlayingMontage, StoringAnimationTime, CurrentPlayerHealth, CurrentPlayerStamina, CurrentPlayerRevengeMeter, CurrentRageGage, CurrentPlayerWeaponData, PlayerFrameComboCount);
	ProjectileListDataSet = StoringProjectileListDataSet;
	CurrentEnemyFrameData = StoringEnemyFrameData;
}

ParkourPositionData::ParkourPositionData(FTransform StoringTransform, ParkourStatus CurrentStatus,
	CombatStatus CurrentCombatStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime, TArray< FProjectileFrameData> StoringProjectileListDataSet, EnemyFrameData StoringEnemyFrameData, float CurrentPlayerHealth, float CurrentPlayerStamina, float CurrentPlayerRevengeMeter, float CurrentRageGage, WeaponRewindData CurrentPlayerWeaponData, int32 PlayerFrameComboCount)
{
	NextFrameData = nullptr;
	PreviousFrameData = nullptr;
	CurrentPlayerFrameData = PlayerFrameData(StoringTransform, CurrentStatus, CurrentCombatStatus, PlayingMontage, StoringAnimationTime, CurrentPlayerHealth, CurrentPlayerStamina, CurrentPlayerRevengeMeter, CurrentRageGage, CurrentPlayerWeaponData, PlayerFrameComboCount);
	ProjectileListDataSet = StoringProjectileListDataSet;
	CurrentEnemyFrameData = StoringEnemyFrameData;
}


ParkourMovementLinkedList::ParkourMovementLinkedList()
{
	HeadData = nullptr;
	TailData = nullptr;

	ListSize = 0;
}

ParkourMovementLinkedList::~ParkourMovementLinkedList()
{
	ClearList();
}

void ParkourMovementLinkedList::AddAtTail(const FTransform DataToAdd)
{
	//If Head pointer is nullptr
	if (!HeadData)
	{
		// Create new data to add at both head and tail
		ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
		HeadData = NewData;
		TailData = NewData;
		ListSize++;
		return;
	}

	//If Linklist already has Head Data
	ParkourPositionData* IteratingData = HeadData;

	// While loop until CurrentNode is Tail
	while(IteratingData->NextFrameData)
	{	
		//Setting up CurrentNode as NewData for next while loop check
		IteratingData = IteratingData->NextFrameData;
	}

	// Create new data to add at tail
	ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);

	// Current Tail's NextTransform become NewData
	IteratingData->NextFrameData = NewData;
	NewData->PreviousFrameData = IteratingData;
	
	// NewData become Tail
	TailData = NewData;

	ListSize++;
	
}

void ParkourMovementLinkedList::AddAtHead(const FTransform DataToAdd)
{
	//If Head pointer is nullptr
	if (!HeadData)
	{
		// Create new data to add at both head and tail
		ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
		HeadData = NewData;
		TailData = NewData;
		ListSize++;
		return;
	}

	// If list only has head
	if(ListSize == 1)
	{
		ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
		HeadData = NewData;
		HeadData->NextFrameData = TailData;
		TailData->PreviousFrameData = HeadData;
		ListSize++;
		return;
	}
	
	//If Linklist already has Head Data and list is bigger than 1
	ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
	NewData->NextFrameData = HeadData;
	HeadData->PreviousFrameData = NewData;
	HeadData = NewData;

	ListSize++;
}

ParkourPositionData* ParkourMovementLinkedList::GetAt(const int32 IndexToGet)
{
	// If head data is null pointer
	if (!HeadData)
		// It means no data in list, just return
		return nullptr;
	
	
	// Head is not null pointer
	ParkourPositionData* CurrentNode = HeadData;

	//If The Index is 0
	if (IndexToGet == 0)
	{
		// 0 is the first one in LinkedList
		return HeadData;
	}

	//If the user inputs nIndex is less than 0 or greater than LinkedList Size
	if (IndexToGet < 0 || IndexToGet > ListSize)
	{
		return nullptr;
	}

	//If the user inputs nIndex is between 0 and Linkedlist size
	int CurrentIndexCounter = 0;
	// While loop until reaching Index number
	while (CurrentIndexCounter != IndexToGet)
	{
		//setting up CurrentNode next node in the list
		CurrentNode = CurrentNode->NextFrameData;
		CurrentIndexCounter++;
	}
	return CurrentNode;
}

void ParkourMovementLinkedList::RemoveTailData()
{
	// insure tail data is valid
	if(!TailData) return;

	// set removing tail data's previous data as tail data if previous data is valid
	ParkourPositionData* CurrentTailData = GetTail();
	ParkourPositionData* TailPreviousData = CurrentTailData->PreviousFrameData;

	if(TailPreviousData)
		TailData = TailPreviousData;
	else
		HeadData = nullptr;
		
	// remove tail data
	CurrentTailData = nullptr;
}

void ParkourMovementLinkedList::ClearList()
{
	// If head is null pointer
	if (!HeadData)
	{
		//no need to clear
		return;
	}

	// while loop until Head become null pointer
	while (HeadData)
	{
		// delete tail if Head data is still valid
		RemoveTailData();
	}

	// Set LinkedListSize as 0
	ListSize = 0;
}

// ==================================================== Stack ==========================================================



BackTrackData_Stack::BackTrackData_Stack()
{
	TopData = nullptr;
	TailData = nullptr;
	ListSize = 0;
}

BackTrackData_Stack::~BackTrackData_Stack()
{
	ClearList();
}

void BackTrackData_Stack::Push(const FTransform DataToAdd, const ParkourStatus StoringParkourStatus, const CombatStatus StoringCombatStatus, UAnimMontage* StoringAnimation, const float StoringAnimationTime, float CurrentPlayerHealth, float CurrentPlayerStamina, float CurrentPlayerRevengeMeter, float CurrentRageGage, TArray< FProjectileFrameData> StoringProjectileListDataSet, EnemyFrameData StoringEnemyFrameData, WeaponRewindData StoringPlayerWeaponData, int32 StoringPlayerFrameComboCount)
{
	// if is empty, create new node and assign it as Top
	if(IsEmpty())
	{
		TopData = new ParkourPositionData(DataToAdd, StoringParkourStatus, StoringAnimation, StoringAnimationTime, StoringProjectileListDataSet, StoringEnemyFrameData, CurrentPlayerHealth, CurrentPlayerStamina, CurrentPlayerRevengeMeter, CurrentRageGage, StoringPlayerWeaponData, StoringPlayerFrameComboCount);
		TailData = TopData;
		
		return;
	}

	//if not empty, create new Top
	ParkourPositionData* NewData = new ParkourPositionData(DataToAdd, StoringParkourStatus, StoringCombatStatus, StoringAnimation, StoringAnimationTime, StoringProjectileListDataSet, StoringEnemyFrameData, CurrentPlayerHealth, CurrentPlayerStamina, CurrentPlayerRevengeMeter, CurrentRageGage, StoringPlayerWeaponData, StoringPlayerFrameComboCount);
	
	NewData->NextFrameData = TopData;
	TopData->PreviousFrameData = NewData;
	TopData = NewData;

	ListSize++;
	if(ListSize > MaxListSize)
	{
		RemoveTailData();
	}
}

void BackTrackData_Stack::Pop()
{
	// if is empty, return
	if(IsEmpty())
	{
		return;
	}

	ParkourPositionData* RemovingData = TopData;
	TopData = RemovingData->NextFrameData;
	RemovingData = nullptr;
	
	ListSize--;
}

void BackTrackData_Stack::RemoveTailData()
{
	// insure tail data is valid
	if(!TailData) return;

	
	// set removing tail data's previous data as tail data if previous data is valid
	ParkourPositionData* CurrentTailData = TailData;
	ParkourPositionData* TailPreviousData = CurrentTailData->PreviousFrameData;

	if(TailPreviousData)
		TailData = TailPreviousData;
	else
		TopData = nullptr;
		
	// remove tail data
	CurrentTailData = nullptr;
}

void BackTrackData_Stack::ClearListToIndex(int32 ClearToIndex)
{
	// If head is null pointer
	if (!TopData)
	{
		//no need to clear
		return;
	}

	// while loop until Head become null pointer
	while (ListSize > ClearToIndex)
	{
		// delete tail if Head data is still valid
		Pop();
		ListSize--;
	}
}


void BackTrackData_Stack::ClearList()
{
	// If head is null pointer
	if (!TopData)
	{
		//no need to clear
		return;
	}

	// while loop until Head become null pointer
	while (TopData)
	{
		// delete tail if Head data is still valid
		Pop();
	}

	// Set LinkedListSize as 0
	ListSize = 0;
	
}