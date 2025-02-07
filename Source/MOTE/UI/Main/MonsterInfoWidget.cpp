// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterInfoWidget.h"
#include "AI/AIMonsterBase.h"

void UMonsterInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UMonsterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	mMonsterName = Cast<UTextBlock>(GetWidgetFromName(TEXT("MonsterName")));
	mMonsterHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("MonsterHP")));
	mDelayHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("DelayHP")));
	mMonsterStagger = Cast<UProgressBar>(GetWidgetFromName(TEXT("MonsterStagger")));
	mDelayStagger = Cast<UProgressBar>(GetWidgetFromName(TEXT("DelayStagger")));

	mMonsterName->SetVisibility(ESlateVisibility::Collapsed);
	mMonsterHP->SetVisibility(ESlateVisibility::Collapsed);
	mDelayHP->SetVisibility(ESlateVisibility::Collapsed);
	mMonsterStagger->SetVisibility(ESlateVisibility::Collapsed);
	mDelayStagger->SetVisibility(ESlateVisibility::Collapsed);
}

void UMonsterInfoWidget::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);
}

void UMonsterInfoWidget::SetMonsterName(const FString& Name)
{
	mMonsterName->SetText(FText::FromString(Name)) ;
}

void UMonsterInfoWidget::SetMonsterHP(float Percent)
{
	mMonsterHP->SetPercent(Percent);

	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandleHP))
	{	
		//매프레임 서서히 감소
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

void UMonsterInfoWidget::SetMonsterStagger(float Percent)
{
	mMonsterStagger->SetPercent(Percent);

	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandleStagger))
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandleStagger, [this, Percent]()
			{
				float DestAmount = Percent;
				float CurrentAmount = mDelayStagger->GetPercent();
				float DecreaseAmount = (CurrentAmount - DestAmount) / 0.5f * GetWorld()->GetDeltaSeconds();
				CurrentAmount -= DecreaseAmount;
				if (CurrentAmount <= DestAmount)
				{
					CurrentAmount = DestAmount;
					GetWorld()->GetTimerManager().ClearTimer(TimerHandleStagger);
				}
				mDelayStagger->SetPercent(CurrentAmount);

			}, GetWorld()->GetDeltaSeconds(), true);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandleStagger, [this, Percent]()
			{
				float DestAmount = Percent;
				float CurrentAmount = mDelayStagger->GetPercent();
				float DecreaseAmount = (CurrentAmount - DestAmount) / 0.5f * GetWorld()->GetDeltaSeconds();
				CurrentAmount -= DecreaseAmount;
				if (CurrentAmount <= DestAmount)
				{
					CurrentAmount = DestAmount;
					GetWorld()->GetTimerManager().ClearTimer(TimerHandleStagger);
				}
				mDelayStagger->SetPercent(CurrentAmount);

			}, GetWorld()->GetDeltaSeconds(), true, 0.3f);
	}
}

void UMonsterInfoWidget::SetTarget(AAIMonsterBase* NewMonster)
{
	if (IsValid(mCurrentTarget))
	{
		// 기존 델리게이트 해제
		mCurrentTarget->RemoveMonsterHPDelegate<UMonsterInfoWidget>(this);
		mCurrentTarget->RemoveMonsterStaggerDelegate<UMonsterInfoWidget>(this);
	}

	mCurrentTarget = NewMonster;

	mMonsterName->SetVisibility(ESlateVisibility::HitTestInvisible);
	mMonsterHP->SetVisibility(ESlateVisibility::HitTestInvisible);
	mDelayHP->SetVisibility(ESlateVisibility::HitTestInvisible);
	mMonsterStagger->SetVisibility(ESlateVisibility::HitTestInvisible);
	mDelayStagger->SetVisibility(ESlateVisibility::HitTestInvisible);
	SetMonsterName(mCurrentTarget->mMonsterInfoKey.ToString());
	SetMonsterHP(mCurrentTarget->mHP / mCurrentTarget->mHPMax);
	SetMonsterStagger(mCurrentTarget->mStagger / mCurrentTarget->mStaggerMax);

	// Monster HP Delegate
	mCurrentTarget->AddMonsterHPDelegate<UMonsterInfoWidget>(this, &UMonsterInfoWidget::SetMonsterHP);
	// Monster Stagger Delegate
	mCurrentTarget->AddMonsterStaggerDelegate<UMonsterInfoWidget>(this, &UMonsterInfoWidget::SetMonsterStagger);


}

void UMonsterInfoWidget::ClearTarget()
{
	if (IsValid(mCurrentTarget))
	{
		// 기존 델리게이트 해제
		mCurrentTarget->RemoveMonsterHPDelegate<UMonsterInfoWidget>(this);
		mCurrentTarget->RemoveMonsterStaggerDelegate<UMonsterInfoWidget>(this);
	}

	mCurrentTarget = nullptr;
	mMonsterName->SetVisibility(ESlateVisibility::Collapsed);
	mMonsterHP->SetVisibility(ESlateVisibility::Collapsed);
	mDelayHP->SetVisibility(ESlateVisibility::Collapsed);
	mMonsterStagger->SetVisibility(ESlateVisibility::Collapsed);
	mDelayStagger->SetVisibility(ESlateVisibility::Collapsed);
}

void UMonsterInfoWidget::ClearHandle()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleHP);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleStagger);
}
