// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GolemAnimNotifyCrashTelegraph.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UGolemAnimNotifyCrashTelegraph : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
