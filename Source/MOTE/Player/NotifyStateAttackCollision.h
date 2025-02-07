// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NotifyStateAttackCollision.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UNotifyStateAttackCollision : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class APlayerCharacter* mPlayer;

	UPROPERTY()
	UNiagaraSystem* mHitEffect;

	UPROPERTY()
	TSet<class AAIMonsterBase*> DamagedActors;

private:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;	
};
