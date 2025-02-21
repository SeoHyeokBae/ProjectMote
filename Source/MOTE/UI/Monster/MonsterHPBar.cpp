// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterHPBar.h"
#include "AI/AIMonsterBase.h"


void UMonsterHPBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UMonsterHPBar::NativeConstruct()
{
	Super::NativeConstruct();

	mMonsterHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("MonsterHP")));
	mDelayHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("DelayHP")));
}

void UMonsterHPBar::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);

}

void UMonsterHPBar::SetMonsterHP(float Percent)
{
	if (!mDelayHP) return;
	mMonsterHP->SetPercent(Percent);

	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandleHP))
	{	
		// 0.3s 뒤 매프레임 서서히 감소
		GetWorld()->GetTimerManager().SetTimer(TimerHandleHP, [this, Percent]()
			{
				if (!mDelayHP) return;
				// 최종 Percent 지점
				float DestAmount = Percent;
				// 현재(Start) Percent
				float CurrentAmount = mDelayHP->GetPercent();
				// 매틱 감소시키는 양 (Duration 0.5s)
				float DecreaseAmount = (CurrentAmount - DestAmount) / 0.5 * GetWorld()->GetDeltaSeconds();
				CurrentAmount -= DecreaseAmount;
				if (CurrentAmount <= DestAmount)
				{
					CurrentAmount = DestAmount;
					GetWorld()->GetTimerManager().ClearTimer(TimerHandleHP);
				}
				mDelayHP->SetPercent(CurrentAmount);

			}, GetWorld()->GetDeltaSeconds(), true);
	}
	else
	{
		// 0.3s 뒤 매프레임 서서히 감소
		GetWorld()->GetTimerManager().SetTimer(TimerHandleHP, [this, Percent]()
			{
				// 최종 Percent 지점
				float DestAmount = Percent;
				// 현재(Start) Percent
				float CurrentAmount = mDelayHP->GetPercent();
				// 매틱 감소시키는 양 (Duration 0.5s)
				float DecreaseAmount = (CurrentAmount - DestAmount) / 0.5 * GetWorld()->GetDeltaSeconds();
				CurrentAmount -= DecreaseAmount;
				if (CurrentAmount <= DestAmount)
				{
					CurrentAmount = DestAmount;
					GetWorld()->GetTimerManager().ClearTimer(TimerHandleHP);
				}
				mDelayHP->SetPercent(CurrentAmount);

			}, GetWorld()->GetDeltaSeconds(), true, 0.3f);
	}

}

void UMonsterHPBar::SetOwner(AAIMonsterBase* Owner)
{
	mOwner = Owner;

	if (mOwner)
		mOwner->AddMonsterHPDelegate(this, &UMonsterHPBar::SetMonsterHP);
}

void UMonsterHPBar::ClearHandle()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleHP);
}
