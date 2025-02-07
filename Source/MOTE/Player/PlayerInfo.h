#pragma once

#include "PlayerInfo.generated.h"

#define DashValue				15.f
#define FloatValue				15.f
#define FloatLoopValue			4.f
#define RecoveryStaminaValue	35.f
#define DetectionRange			10000.f

enum class EShakeType : uint8
{
	FloatDash,
	BaseAttack,
	StrongAttack,
	StrongAttack_4th,
	DashAttack,
	MAX,
};

enum class EScreenEffct : uint8
{
	Damage,
	Fire,
	DashAttack,
};

USTRUCT(BlueprintType)
struct FPlayerInfoTable : public FTableRowBase
{
	GENERATED_BODY()

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
	float		mStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mAttackDistance;
};
