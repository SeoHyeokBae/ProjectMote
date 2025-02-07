// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AICharacterBase.h"
#include "../UI/Monster/MonsterHPBar.h"
#include "AIMonsterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterHPDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterStaggerDelegate, float);

UCLASS()
class MOTE_API AAIMonsterBase : public AAICharacterBase
{
	GENERATED_BODY()

public:
	AAIMonsterBase();
	
public:
	FName mMonsterInfoKey;
	float mAttack;
	float mDefense;
	float mHP;
	float mHPMax;
	float mMP;
	float mMPMax;
	float mStagger;
	float mStaggerMax;
	float mMoveSpeed;
	float mAttackDistance;

	bool IsRage = false;

	FMonsterHPDelegate mHPDelegate;
	FMonsterStaggerDelegate mStaggerDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UTargetAimSystemComponent* mTargetAimSystem;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* mHPBarComponent;

	UPROPERTY()
	class UMonsterHPBar* mHPBarHUD;

private:
	FTimerHandle mHitTimer;
	float mHitTime = 0.2f;

public:
	virtual void SetAIType(EAIType Type);

	void SelectedTarget(bool bFlag);
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);


	FVector GetTargetAimLocation() const;

public:
	virtual void PossessedBy(AController* NewController);

	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser);

	virtual float TakeDamage(float DamageAmount);

	FORCEINLINE class UTargetAimSystemComponent* GetTargetAimSystem() const
	{
		return mTargetAimSystem;
	}

	virtual void Attack();
	void SetStagger(float Amount);

public:
	template <typename T>
	void AddMonsterHPDelegate(T* Obj, void (T::* Func)(float))
	{
		mHPDelegate.AddUObject(Obj, Func);
	}

	template <typename T>
	void AddMonsterStaggerDelegate(T* Obj, void (T::* Func)(float))
	{
		mStaggerDelegate.AddUObject(Obj, Func);
	}

	template <typename T>
	void RemoveMonsterHPDelegate(T* Obj)
	{
		mHPDelegate.RemoveAll(Obj);
	}	
	template <typename T>
	void RemoveMonsterStaggerDelegate(T* Obj)
	{
		mStaggerDelegate.RemoveAll(Obj);
	}

};
