// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "PlayerInfo.h"
#include "GameFramework/PlayerState.h"
#include "MotePlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FHPDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FStaminaDelegate, float);

UCLASS()
class MOTE_API AMotePlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	static UDataTable* mPlayerInfoTable;

public:
	AMotePlayerState();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString		mName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mDefense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mHPMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mStaminaMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mAttackDistance;

	FHPDelegate			mHPDelegate;
	FStaminaDelegate	mStaminaDelegate;

//protected:
//	UPROPERTY()
//	class APlayerCharacter* mPlayer;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	bool	SetHP(float Amount, bool IsIncrease = false, bool IsVisible = true);
	bool	SetStamina(float Amount, bool IsIncrease = false); //Use, Expend, Consume

	float	GetStamina() { return mStamina; }
	bool	GetMaxStamina() { return mStamina >= mStaminaMax; }

public:
	template <typename T>
	void AddHPDelegate(T* Obj, void (T::* Func)(float))
	{
		mHPDelegate.AddUObject(Obj, Func);
	}

	template <typename T>
	void AddStaminaDelegate(T* Obj, void (T::* Func)(float))
	{
		mStaminaDelegate.AddUObject(Obj, Func);
	}
	
};
