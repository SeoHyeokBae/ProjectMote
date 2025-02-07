// Fill out your copyright notice in the Description page of Project Settings.


#include "InputData.h"

UDefaultInputData::UDefaultInputData()
{
	{
		static ConstructorHelpers::FObjectFinder<UInputMappingContext>
			Asset(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));

		if (Asset.Succeeded())
		{
			mDefaultContext = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Move.IA_Move'"));

		if (Asset.Succeeded())
		{
			mMove = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Rotation.IA_Rotation'"));

		if (Asset.Succeeded())
		{
			mRotation = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Attack.IA_Attack'"));

		if (Asset.Succeeded())
		{
			mAttack = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Jump.IA_Jump'"));

		if (Asset.Succeeded())
		{
			mJump = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Dash.IA_Dash'"));

		if (Asset.Succeeded())
		{
			mDash = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Float.IA_Float'"));

		if (Asset.Succeeded())
		{
			mFloat = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Aim.IA_Aim'"));

		if (Asset.Succeeded())
		{
			mAim = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Detected.IA_Detected'"));

		if (Asset.Succeeded())
		{
			mDetected = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_BossKill.IA_BossKill'"));

		if (Asset.Succeeded())
		{
			mBossKill = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Guide.IA_Guide'"));

		if (Asset.Succeeded())
		{
			mGuide = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Interaction.IA_Interaction'"));

		if (Asset.Succeeded())
		{
			mInteraction = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Pause.IA_Pause'"));

		if (Asset.Succeeded())
		{
			mPause = Asset.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UInputAction>
			Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Respawn.IA_Respawn'"));

		if (Asset.Succeeded())
		{
			mRespawn = Asset.Object;
		}
	}
}
