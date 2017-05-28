#pragma once

#include "Moveable.generated.h"

UINTERFACE(MinimalAPI)
class UMoveable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 *  Indicates an object which possesses movement capacities
 */
class CARDBOTFRAMEWORK_API IMoveable
{
	GENERATED_IINTERFACE_BODY()

public:
	
    /** Set current speed */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="CardBot")
    float SetSpeed(float speedRate);
    //virtual float SetSpeed_Implementation(float speedRate);
    
    /** Get current speed */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="CardBot")
    float GetSpeed();
    //virtual float GetSpeed_Implementation();
};
