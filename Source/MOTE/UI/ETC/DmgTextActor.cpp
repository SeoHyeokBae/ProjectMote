// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ETC/DmgTextActor.h"
#include "UI/ETC/DmageAmountWidget.h"

ADmgTextActor::ADmgTextActor()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	mTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));
	mWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));

	SetRootComponent(mRoot);
	mWidgetComponent->SetupAttachment(mRoot);

	static ConstructorHelpers::FClassFinder<UDmageAmountWidget>
		DmgWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/ETC/UI_DmageAmountWidget.UI_DmageAmountWidget_C'"));

	if (DmgWidgetClass.Succeeded())
		mWidgetComponent->SetWidgetClass(DmgWidgetClass.Class);

	static ConstructorHelpers::FObjectFinder<UCurveFloat>
		Curve(TEXT("/Script/Engine.CurveFloat'/Game/Player/Data/DmgTimelineCurve.DmgTimelineCurve'"));

	if (Curve.Succeeded())
		mTimeCurve = Curve.Object;

	mWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

void ADmgTextActor::BeginPlay()
{
	Super::BeginPlay();

	//EndLocation = GetActorLocation() + FVector(-30.f, FMath::RandRange(-300, 300), -70.f);
	
	StartLocation = GetActorLocation();
	EndLocation = GetActorLocation() + FVector(0.f, 0.f, -70.f);
	IncreaseEndLocation = GetActorLocation()+FVector(30.0,30.0, 70.0);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager)
	{
		EndLocation += CameraManager->GetActorRightVector() * FMath::RandRange(-250, 250);
	}


	mWidget = Cast<UDmageAmountWidget>(mWidgetComponent->GetWidget());
	if (mWidget)
	{
		mWidget->FadeText();

		FOnTimelineFloat TimelineHandler;
		TimelineHandler.BindDynamic(this, &ThisClass::TimeLineUpdate);
		mTimelineComp->AddInterpFloat(mTimeCurve, TimelineHandler);

		FOnTimelineEvent TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("TimeLineFinish"));
		mTimelineComp->SetTimelineFinishedFunc(TimelineFinishedCallback);

		mTimelineComp->SetPlayRate(1);
		mTimelineComp->Play();
	}
}

void ADmgTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADmgTextActor::InputDamage(float Damage, bool IsHeadShot)
{
	mDamageAmount = Damage;
	mWidget->DmgAmt = mDamageAmount;
	mWidget->ChangeScale(Damage);
	mWidget->FadeText();
	if (IsHeadShot)
	{
		mWidget->FadeHeadShot();
		mWidget->ChangeColor(FLinearColor::Yellow);
	}
}

void ADmgTextActor::ChangeColor(FLinearColor NewColor)
{
	mWidget->ChangeColor(NewColor);
}

void ADmgTextActor::TimeLineUpdate(float Alpha)
{
	//FVector NewLocation;
	//NewLocation.X = FMath::Lerp(StartLocation.X, EndLocation.X, Alpha);
	//NewLocation.Y = FMath::Lerp(StartLocation.Y, EndLocation.Y, Alpha);
	//NewLocation.Z = StartLocation.Z + FMath::Sin(Alpha * PI * FREQUENCY) * AMPLITUDE;

	TimeAcc += GetWorld()->GetDeltaSeconds();
	FVector NewLocation;
	if(IsIncrease)
	{
		NewLocation = FMath::Lerp(StartLocation, IncreaseEndLocation, TimeAcc);
	}
	else
	{
		NewLocation.X = FMath::Lerp(StartLocation.X, EndLocation.X, TimeAcc);
		NewLocation.Y = FMath::Lerp(StartLocation.Y, EndLocation.Y, TimeAcc);
		NewLocation.Z = FMath::Lerp(StartLocation.Z, EndLocation.Z, Alpha);
	}

	SetActorLocation(NewLocation);
}

void ADmgTextActor::TimeLineFinish()
{
	Destroy();
}
// 처음에 커브로했다가 x,y값도 커브에 영향을받아 원하는느낌이안남 (x,y는 1:1커브로 하고싶음)
// 그래서 커브는 1:1 선형그래프로하고 z값만 변경
//NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);