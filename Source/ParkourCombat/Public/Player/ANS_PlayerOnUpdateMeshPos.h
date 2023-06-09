// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PlayerOnUpdateMeshPos.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UANS_PlayerOnUpdateMeshPos : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	
};
