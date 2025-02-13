// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NotifyPlayerAttackCollision.generated.h"

/**
 * 
 */
#define VERTEX_COUNT 13

UCLASS()
class MOTE_API UNotifyPlayerAttackCollision : public UAnimNotify
{
	GENERATED_BODY()

public:
	UNotifyPlayerAttackCollision();
private:
	UPROPERTY()
	UNiagaraSystem* mHitEffect;

	UPROPERTY()
	UNiagaraSystem* mSlashHitEffect;

	UPROPERTY()
	TSet<class AAIMonsterBase*> DamagedActors;

	UPROPERTY()
	TSet<class AActor*> DamagedEnvActors;

	UPROPERTY()
	TArray<FVector> mVertex;

	UPROPERTY()
	TArray<FVector> mBottomVertex;

private:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	void CreateCollision(class APlayerCharacter* Player, float Angle = 210.f);


	
};
