// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "PlayerTrailFX.generated.h"

UCLASS()
class MOTE_API APlayerTrailFX : public AActor
{
	GENERATED_BODY()

public:
	APlayerTrailFX();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* mTrailComponent;

	UPROPERTY()
	UNiagaraComponent* mDashFXComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mDashFXSystem;

	UPROPERTY()
	UNiagaraComponent* mFloatFXComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mFloatFXSystem;

	UPROPERTY()
	UNiagaraComponent* mShieldFXComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mShieldFXSystem;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mAfterExplosionEffect;

protected:
	void DestroyComponent(UNiagaraComponent* Component);

protected:
	float mDashLifeTime = 0.f;
	float mFloatLifeTime = 0.f;
	float mShieldLifeTime = 0.f;

public:
	void OnDashActive();
	void OnFloatActive();
	void OnShieldActive();
	void OffShieldActive();
};

