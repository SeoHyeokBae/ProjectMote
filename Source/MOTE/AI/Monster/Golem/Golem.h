// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "AI/AIInfo.h"
#include "AI/AIMonsterBase.h"
#include "Golem.generated.h"

UENUM()
enum class EGolemState : uint8
{
	Starting,
	Idle,
	Attack,
	Skill_Crash,
	Skill_Crarge,
	Skill_Lazer,
	Skill_Lazer_Charge,
	Skill_Meteor,
	Casting,
	Stagger,
	Death
};

UCLASS()
class MOTE_API AGolem : public AAIMonsterBase, public ICombatInterface
{
	GENERATED_BODY()

public:
	AGolem();

protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* mArmorMesh;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mVFXMesh;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* mFlameFXComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mFlameFXSystem;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* mSmokeFXComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mSmokeFXSystem;

	UPROPERTY(VisibleAnywhere)
	class UGolemAnimInstance* mGolemAnim;

	EGolemState mState;

	UPROPERTY(VisibleAnywhere)
	FRotator mLookTargetDir = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere)
	FRotator mLaserAimRot;

	UPROPERTY()
	TSet<UActorComponent*> mTSetEffects;

public:
	EGolemState GetGolemState() const { return mState; }
	void SetGolemState(EGolemState State) { mState = State; }

	void SetGolemAnim(EGolemState Type);

	FRotator GetLookTargetDir() const { return mLookTargetDir; }
	FRotator GetLaserAimRot() const { return mLaserAimRot; }
	void SetLaserAimRot(FRotator Rot) { mLaserAimRot = Rot; }
	
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

public:
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,
								class AController* EventInstigator, AActor* DamageCauser);

	void OffFX()
	{
		mFlameFXComponent->Deactivate();
		mSmokeFXComponent->Deactivate();
	}

protected:
	UFUNCTION()
	void MeshBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void MeshOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:
	virtual void Attack();

	bool GetCanSpawnMeteor();
	void SetSpawnMeteor();

private:
	void UpdateTargetDir();
	void DeathDelegate();

public:
	UFUNCTION()
	virtual void HitInteraction() override;
	UFUNCTION()
	virtual void HitStopStart() override;
	UFUNCTION()
	virtual void HitStopEnd() override;

private:
	virtual void Create() override;
	virtual void Load() override;
	virtual void Init() override;
};
