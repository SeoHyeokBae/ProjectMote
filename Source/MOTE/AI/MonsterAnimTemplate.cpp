// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MonsterAnimTemplate.h"
#include "AIController.h"
#include "AIMonsterBase.h"
#include "MonsterDataManager.h"

UMonsterAnimTemplate::UMonsterAnimTemplate()
{
}

void UMonsterAnimTemplate::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UMonsterAnimTemplate::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}

void UMonsterAnimTemplate::SetAnimData(const FName& Key)
{
	const FMonsterAnimData* AnimData = CMonsterDataManager::GetInst()->FindMonsterAnim(Key);

	if (AnimData)
	{
		mSequenceMap = AnimData->mSequenceMap;
		mBlendSpaceMap = AnimData->mBlendSpaceMap;
		mMontageMap = AnimData->mMontageMap;
	}
}

void UMonsterAnimTemplate::PlayMontage(const FString& Name)
{
	UAnimMontage** Montage = mMontageMap.Find(Name);

	if (Montage)
	{
		if (!Montage_IsPlaying(*Montage))
		{
			if (Name == TEXT("Hit"))
				mHitAlpha = 1.f;

			Montage_SetPosition(*Montage, 0.f);
			Montage_Play(*Montage);
		}
	}

}

void UMonsterAnimTemplate::AnimNotify_Attack()
{
	AAIMonsterBase* Monster = Cast<AAIMonsterBase>(TryGetPawnOwner());

	if (IsValid(Monster))
		Monster->Attack();
}

void UMonsterAnimTemplate::AnimNotify_AttackEnd()
{
	AAIController* Control = TryGetPawnOwner()->GetController<AAIController>();
	Control->GetBlackboardComponent()->SetValueAsBool(
		TEXT("AttackEnd"), true);
}

void UMonsterAnimTemplate::AnimNotify_HitEnd()
{
	mHitAlpha = 0.f;
}

void UMonsterAnimTemplate::AnimNotify_DeathEnd()
{
	mIsDeathEnd = true;
}



