// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GolemAnimNotifyAttackCollision.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UGolemAnimNotifyAttackCollision : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class AGolem* mGolem;

	UPROPERTY()
	UNiagaraSystem* mHitEffect;

	//UPROPERTY()
	//TSet<class APlayerCharacter*> DamagedActors;


	bool IsAlreadyHit = false;
	float DamageResetTime = 1.0f; // 일정 시간 후 데미지 재적용 가능
	FTimerHandle DamageResetTimerHandle;

private:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
