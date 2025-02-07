// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStaminaWidget.h"
#include "../../Player/PlayerCharacter.h"
#include "../../Player/MotePlayerState.h"

void UPlayerStaminaWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mStaminaMaterial = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
		, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/UI/Player/MTI_Stamina.MTI_Stamina'")));

	// Set Stamina
	mStaminaMaterialDynamic = UMaterialInstanceDynamic::Create(mStaminaMaterial, this);

	if (APlayerController* PlayerController = GetOwningPlayer())
		mPlayer = Cast<APlayerCharacter>(PlayerController->GetPawn());

	mPlayer->GetStaminaComponent()->SetMaterial(0, mStaminaMaterialDynamic);

	UI_Stamina->SetBrushFromMaterial(mStaminaMaterialDynamic);

	// Player State Delegate
	AMotePlayerState* State = GetOwningPlayerState<AMotePlayerState>();

	if (IsValid(State))
		State->AddStaminaDelegate<UPlayerStaminaWidget>(this, &UPlayerStaminaWidget::SetStaminaPercent);
}

void UPlayerStaminaWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerStaminaWidget::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);

	AMotePlayerState* State = GetOwningPlayerState<AMotePlayerState>();

	if (IsValid(State))
	{
		if (State->mStamina == State->mStaminaMax && UI_Stamina->GetVisibility() == ESlateVisibility::Visible)
		{
			if (!mIsFadeOut)
			{
				mIsFadeOut = true;
				PlayAnimation(FadeOut);
			}
		}
		else if(State->mStamina != State->mStaminaMax && UI_Stamina->GetVisibility() == ESlateVisibility::Collapsed)
		{
			mIsFadeOut = false;
			PlayAnimation(FadeIn);

		}
	}

	
}

void UPlayerStaminaWidget::SetStaminaPercent(float Percent)
{
	mStaminaMaterialDynamic->SetScalarParameterValue(FName(TEXT("Percent")), Percent);
}
