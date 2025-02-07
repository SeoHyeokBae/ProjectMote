#pragma once

#include "SoundInfo.generated.h"

USTRUCT(BlueprintType)
struct FSoundData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, USoundBase*>	SoundDatas;
};