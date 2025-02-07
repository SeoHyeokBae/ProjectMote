// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "../../../AI/AIInfo.h"
#include "../../../AI/AIMonsterBase.h"
#include "LightElemental.generated.h"

/**
 * 
 */
#define CREATE_PORTAL_DIST 1500.f

UCLASS()
class MOTE_API ALightElemental : public AAIMonsterBase, public ICombatInterface
{
	GENERATED_BODY()
	

public:
	ALightElemental();

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* mShellMesh;

	UNiagaraSystem* mHitEffect;

	UPROPERTY(EditAnywhere)
	class UMonsterAnimTemplate* mLightElementalAnim;

public:
	virtual void SetAIType(EAIType Type);


protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	void UpdateTargetDist();
	void CreatePortal();
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser);

public:
	UFUNCTION()
	virtual void HitInteraction() override;
	UFUNCTION()
	virtual void HitStopStart() override;
	UFUNCTION()
	virtual void HitStopEnd() override;

	virtual void Attack() override;

private:
	virtual void Create() override;
	virtual void Load() override;
	virtual void Init() override;
};
