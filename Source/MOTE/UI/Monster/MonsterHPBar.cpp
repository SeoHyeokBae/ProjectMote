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
		GetWorld()->GetTimerManager().SetTimer(TimerHandleHP, this, &UMonsterHPBar::DelayReductionEffect, GetWorld()->GetDeltaSeconds(), true);
	}
	else
	{
		// 0.3s �� �� ������ ������ ����
		GetWorld()->GetTimerManager().SetTimer(TimerHandleHP, this, &UMonsterHPBar::DelayReductionEffect, GetWorld()->GetDeltaSeconds(), true, 0.3);
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

void UMonsterHPBar::DelayReductionEffect()
{
	if (!mDelayHP) return;
	// ���� Percent ����
	float DestAmount = mMonsterHP->GetPercent();
	// ����(Start) Percent
	float CurrentAmount = mDelayHP->GetPercent();
	// ��ƽ ���ҽ�Ű�� �� (Duration 0.5s)
	float DecreaseAmount = (CurrentAmount - DestAmount) / 0.5 * GetWorld()->GetDeltaSeconds();
	CurrentAmount -= DecreaseAmount;
	if (CurrentAmount <= DestAmount)
	{
		CurrentAmount = DestAmount;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleHP);
	}
	mDelayHP->SetPercent(CurrentAmount);
}


