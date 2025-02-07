// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInfoWidget.h"
#include "Player/MotePlayerState.h"

void UPlayerInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Player State Delegate
	AMotePlayerState* State = GetOwningPlayerState<AMotePlayerState>();

	if (IsValid(State))
		State->AddHPDelegate<UPlayerInfoWidget>(this, &UPlayerInfoWidget::SetHPPercent);
}

void UPlayerInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	mPlayerHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("PlayerHP")));
	mDelayHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("DelayHP")));
}

void UPlayerInfoWidget::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);
}

void UPlayerInfoWidget::SetHPPercent(float Percent)
{
	mPlayerHP->SetPercent(Percent);

	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandleHP))
	{
		if (!mDelayHP) return;
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

			}, GetWorld()->GetDeltaSeconds(), true);
	}
	else
	{
		if (!mDelayHP) return;
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

void UPlayerInfoWidget::ClearHandle()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleHP);
}
