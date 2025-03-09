// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIInfo.h"
#include "GameFramework/Character.h"
#include "AICharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FAIDeathDelegate);


UCLASS()
class MOTE_API AAICharacterBase : public ACharacter,
	public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AAICharacterBase();

protected:
	TArray<UMaterialInstanceDynamic*> mMaterialDynamicArray;

	FString mName;

	FAIDeathDelegate mDeathDelegate;

	EAIType	mAIType;

	uint8 mTeamID = 10;

	TArray<AActor*> mPatrolArray;
	int32			mPatrolIndex = 1;

	TArray<FVector> mPatrolPoints;

public:
	const FString& GetAIName() const
	{
		return mName;
	}

	EAIType GetAIType() const
	{
		return mAIType;
	}

	void SetAIName(const FString& Name)
	{
		mName = Name;
	}

	void SetTeamID(uint8 TeamID)
	{
		mTeamID = TeamID;
	}

	virtual void SetAIType(EAIType Type);

	virtual FGenericTeamId GetGenericTeamId() const;

	void AddPatrolStart(FVector Point);
	void RegisterPatrolVectorPoint();
	void NextPatrolVectorPoint();
	void RandomPatrolPoint();

public:
	virtual void PossessedBy(AController* NewController);
	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser);


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void Create();
	virtual void Load();
	virtual void Init();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	void DeathEnd();

public:
	template <typename T>
	void AddDeathDelegate(T* Object, void(T::* Func)())
	{
		mDeathDelegate.AddUObject(Object, Func);
	}
};
