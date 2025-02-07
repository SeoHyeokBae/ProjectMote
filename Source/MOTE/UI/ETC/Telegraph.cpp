#include "Telegraph.h"
#include "UI/ETC/Telegraph.h"

// Sets default values
ATelegraph::ATelegraph()
{
	PrimaryActorTick.bCanEverTick = true;

	mAimMTI_Square = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
		, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/UI/Common/MTI_Telegraph_Decal_Square.MTI_Telegraph_Decal_Square'")));

	mAimMTI_Circle = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
		, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/UI/Common/MTI_Telegraph_Decal_Circle.MTI_Telegraph_Decal_Circle'")));

	if(mAimMTI_Square)
		mAimMTI_Dynamic_Square = UMaterialInstanceDynamic::Create(mAimMTI_Square, this);
	if(mAimMTI_Circle)
		mAimMTI_Dynamic_Circle = UMaterialInstanceDynamic::Create(mAimMTI_Circle, this);

	GetDecal()->SetMaterial(0, mAimMTI_Dynamic_Square);
	
}

// Called when the game starts or when spawned
void ATelegraph::BeginPlay()
{
	Super::BeginPlay();

	// �ʱⰪ
	UpdatePercent();
}

// Called every frame
void ATelegraph::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	mPercent += DeltaTime * 1 / mLifeTime;
	if (mPercent >= 1.f)
	{
		Destroy();
	}
	else
	{
		UpdatePercent();
	}
}

void ATelegraph::UpdatePercent()
{
	switch ((uint8)mShape)
	{
	case (uint8)ETelegraphShape::Square:
		mAimMTI_Dynamic_Square->SetScalarParameterValue(TEXT("Percent"), mPercent);
		break;

	case (uint8)ETelegraphShape::Circle:
		mAimMTI_Dynamic_Circle->SetScalarParameterValue(TEXT("Percent"), mPercent);
		break;
	}
}

void ATelegraph::SetShape(ETelegraphShape Shape)
{
	mShape = Shape;

	switch ((uint8)mShape)
	{
		case (uint8)ETelegraphShape::Square :
			GetDecal()->SetMaterial(0, mAimMTI_Dynamic_Square);
		break;

		case (uint8)ETelegraphShape::Circle :
			GetDecal()->SetMaterial(0, mAimMTI_Dynamic_Circle);
		break;
	}
}


