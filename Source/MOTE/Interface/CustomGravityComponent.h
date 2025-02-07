#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomGravityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOTE_API UCustomGravityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCustomGravityComponent();
	friend class APlayerCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

private:
	class APlayerCharacter* mPlayer;
	class AStaticMeshActor* mPlanet;
		
public:
	FORCEINLINE AStaticMeshActor* GetPlanet() { return mPlanet; }
};
