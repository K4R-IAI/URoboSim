// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InitialisationHandler.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FRequestInitialisation, FString, UObject* /*ObjectToInitialis*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOfferInitialisation, FString, UObject* /*ObjectThateInitialises*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOfferInitialisation, FString, UObject* /*ObjectThateInitialises*/);

UCLASS( ClassGroup=(Custom))
class UROBOSIM_API URInitialisationHandler : public UObject
{

}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UROBOSIM_API URGraspComponent : public UActorComponent
{

}
