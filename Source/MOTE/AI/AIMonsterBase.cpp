// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIMonsterBase.h"
#include "Controller/MonsterController.h"
#include "MonsterDataManager.h"
#include "Controller/MotePlayerController.h"
#include "../UI/Main/MainWidget.h"
#include "../UI/Main/MonsterInfoWidget.h"
#include "../UI/ETC/DmgTextActor.h"

#include "AI/Monster/Golem/Golem.h"
#include "Controller/GolemController.h"

#include "AI/Monster/TargetAimSystemComponent.h"

AAIMonsterBase::AAIMonsterBase()
{
    AIControllerClass = AMonsterController::StaticClass();
    
    GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);

	// TargetAimSystem
	mTargetAimSystem = CreateDefaultSubobject<UTargetAimSystemComponent>(TEXT("TargetAimSystem"));
	mTargetAimSystem->SetupAttachment(RootComponent);
	mTargetAimSystem->SetCanEverAffectNavigation(false);
	mTargetAimSystem->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// HP BAR
	mHPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarHUDWidget"));

	static ConstructorHelpers::FClassFinder<UMonsterHPBar>
		HPBarWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Monster/UI_MonsterHPBar.UI_MonsterHPBar_C'"));

	if (HPBarWidgetClass.Succeeded())
		mHPBarComponent->SetWidgetClass(HPBarWidgetClass.Class);

	mHPBarComponent->SetupAttachment(RootComponent);
	mHPBarComponent->SetPivot(FVector2D(0.5, 0.5));
	mHPBarComponent->SetDrawSize(FVector2D(150.0, 15.0));
	mHPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	mHPBarComponent->SetRelativeLocation(FVector(0.0, 0.0, 120.f));
	mHPBarComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mHPBarComponent->bVisibleInReflectionCaptures = false;
	mHPBarComponent->bVisibleInRealTimeSkyCaptures = false;
	mHPBarComponent->bReceivesDecals = false;

}

void AAIMonsterBase::SetAIType(EAIType Type)
{
    Super::SetAIType(Type);

}

void AAIMonsterBase::BeginPlay()
{
    Super::BeginPlay();

	const FMonsterInfoTable* Info = CMonsterDataManager::GetInst()->FindMonsterInfo(mMonsterInfoKey);
	if (mTargetAimSystem)
	{
		//mTargetAimSystem >> RootComponent;
		mTargetAimSystem->GetLockOnWidget()->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (Info)
	{
		mAttack = Info->mAttack;
		mDefense = Info->mDefense;
		mHP = Info->mHP;
		mHPMax = Info->mHP;
		mMP = Info->mMP;
		mMPMax = Info->mMP;
		mStagger = Info->mStagger;
		mStaggerMax = Info->mStagger;
		mMoveSpeed = Info->mMoveSpeed;
		mAttackDistance = Info->mAttackDistance;
	}

	mHPBarHUD = Cast<UMonsterHPBar>(mHPBarComponent->GetWidget());
	mHPBarHUD->SetOwner(this);

	if (mAIType == EAIType::Boss)
	{
		mHPBarHUD->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		AddPatrolStart(GetActorLocation());
	}
}

void AAIMonsterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void AAIMonsterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	AGolemController* GolemController = Cast<AGolemController>(NewController);
	if (IsValid(GolemController))
	{
		GolemController->SetTeamID(mTeamID);
		return;
	}


	AMonsterController* MonsterController = Cast<AMonsterController>(NewController);

	if (IsValid(MonsterController))
		MonsterController->SetTeamID(mTeamID);

}

float AAIMonsterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (EventInstigator && DamageCauser)
	{
		UAISense_Damage::ReportDamageEvent(
			GetWorld(),
			this,                          // 데미지를 받은 액터
			DamageCauser,                  // 데미지를 유발한 액터
			DamageAmount,                  // 데미지 양
			DamageCauser->GetActorLocation(),  // 데미지 발생 위치
			GetActorLocation()             // 타겟(자신)의 위치
		);
	}


	mHP -= DamageAmount;
	if (mHP >= mHPMax)
		mHP = mHPMax;

	{ // Visible Damage
		if (DamageAmount < 0)
		{
			FActorSpawnParameters SpawnParams;
			ADmgTextActor* DmgText = GetWorld()->SpawnActor<ADmgTextActor>(ADmgTextActor::StaticClass(), GetActorTransform(), SpawnParams);
			DmgText->InputDamage(-DamageAmount);
			DmgText->ChangeColor(FLinearColor::Green);
			DmgText->SetIsIncrease();
		}

	}
	
	// 일정 피 이하 분노상태
	if (mHP / mHPMax <= 0.2f && !IsRage)
	{
		IsRage = true;
		//CustomTimeDilation = 2.f;
	}
	else if ((mHP <= 0.f && IsRage) || (mHP / mHPMax > mHPMax / 0.2f && IsRage))
	{
		IsRage = false;
		//CustomTimeDilation = 1.f;
	}

	if (mHP <= 0.f)
	{
		if (mAIType == EAIType::Boss)
		{
			AMotePlayerController* PlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			UUserWidget* MainWidget = PlayerController->GetMainWidget();
			UUserWidget* MonsterInfoWidget = Cast<UUserWidget>(MainWidget->GetWidgetFromName(TEXT("UI_MonsterInfo")));
			UMonsterInfoWidget* MonsterInfo = Cast<UMonsterInfoWidget>(MonsterInfoWidget);
			MonsterInfo->SetVisibility(ESlateVisibility::Collapsed);
			MonsterInfo->ClearHandle();
		}
		SetAIType(EAIType::Death);

		mHPBarHUD->SetVisibility(ESlateVisibility::Collapsed);
		mHPBarHUD->ClearHandle();

		AAIController* AIControl = GetController<AAIController>();

		if (IsValid(AIControl))
		{
			AIControl->GetBrainComponent()->StopLogic(TEXT("Death")); 
		}

		if (mDeathDelegate.IsBound())
			mDeathDelegate.Broadcast();
	}
	else
	{
		if (mHPDelegate.IsBound())
		{
			mHPDelegate.Broadcast(mHP / mHPMax);
		}

		if (mStaggerDelegate.IsBound() && DamageAmount >= 0.f)
		{
			mStagger -= (DamageAmount * 2.1f) * (mStaggerMax / mHPMax);
			mStaggerDelegate.Broadcast(mStagger / mStaggerMax);
		}
	}

	return DamageAmount;
}

float AAIMonsterBase::TakeDamage(float DamageAmount)
{
	mHP -= DamageAmount;
	if (mHPDelegate.IsBound())
	{
		mHPDelegate.Broadcast(mHP / mHPMax);
	}

	if (mHP <= 0.f)
	{
		SetAIType(EAIType::Death);

		mHPBarHUD->SetVisibility(ESlateVisibility::Collapsed);

		// ������ Ʈ�� ����
		AAIController* AIControl = GetController<AAIController>();

		if (IsValid(AIControl))
		{
			AIControl->GetBrainComponent()->StopLogic(TEXT("Death")); //text = ����
		}

		if (mDeathDelegate.IsBound())
			mDeathDelegate.Broadcast();
	}
	else
	{
		// ���� �ʾ��� ���
	}


	return DamageAmount;
}

void AAIMonsterBase::Attack()
{
}

void AAIMonsterBase::SetStagger(float Amount)
{
	if (mStaggerDelegate.IsBound())
	{
		mStagger = Amount;
		mStaggerDelegate.Broadcast(mStagger / mStaggerMax);
	}

}

void AAIMonsterBase::SelectedTarget(bool bFlag)
{
	if (!mTargetAimSystem) return;
	mTargetAimSystem->GetLockOnWidget()->SetHiddenInGame(!bFlag);

	if (mAIType == EAIType::Boss)
	{
		if(mHPBarHUD)
			mHPBarHUD->SetVisibility(ESlateVisibility::Collapsed);

		AMotePlayerController* PlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
		if (PlayerController)
		{
			UUserWidget* MainWidget = PlayerController->GetMainWidget();
			UUserWidget* MonsterInfoWidget = Cast<UUserWidget>(MainWidget->GetWidgetFromName(TEXT("UI_MonsterInfo")));
			UMonsterInfoWidget* MonsterInfo = Cast<UMonsterInfoWidget>(MonsterInfoWidget);
			if (bFlag)
				MonsterInfo->SetTarget(this);
			else
				MonsterInfo->ClearTarget();
		}
	}
}

FVector AAIMonsterBase::GetTargetAimLocation() const
{
	return GetTargetAimSystem()->GetComponentLocation();
}
