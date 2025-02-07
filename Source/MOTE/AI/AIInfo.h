#pragma once

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "AIInfo.generated.h"

enum class EMonsterShakeType : uint8
{
	Starting,
	Impact,
	BasicAttack,
	StrongAttack,
	MAX,
};

enum class EAIType : uint8
{
	Idle,
	Patrol,
	Trace,
	Attack,
	Death,
	Boss,
};

class CMonsterDefaultKey
{
public:
	static FName mTarget;
	static FName mAttackEnd;
	static FName mPatrolEnable;
	static FName mPatrolPoint;
	static FName mTakeDamage;
	static FName mAttackNumber;
};

class CBossGolemKey
{
public:
	static FName mStarting;
	static FName mCanAttack;
	static FName mStagger;
};



USTRUCT(BlueprintType)
struct FMonsterAnimData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UAnimSequence*>	mSequenceMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UBlendSpace*>		mBlendSpaceMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UAnimMontage*>	mMontageMap;
};

USTRUCT(BlueprintType)
struct FMonsterInfoTable : public FTableRowBase
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
	float		mMP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mStagger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float		mAttackDistance;
};
