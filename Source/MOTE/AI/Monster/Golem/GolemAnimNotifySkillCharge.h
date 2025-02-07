// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GolemAnimNotifySkillCharge.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UGolemAnimNotifySkillCharge : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UParticleSystem* mExplosionEffect;

	UPROPERTY()
	UNiagaraSystem* mHitEffect;

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
