#pragma once

#include "DamageEventData.generated.h"

/**
 * Damages event data to be passed through sensors
 */
UCLASS(BlueprintType)
class CARDBOTFRAMEWORK_API UDamageEventData : public UObject
{
    GENERATED_BODY()
    
public:
        
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    float DamageAmount;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    FDamageEvent DamageEvent;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    AController* EventInstigator;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    AActor* DamageCauser;
    
    inline void SetEventData(float damageAmount, FDamageEvent const& damageEvent, AController* eventInstigator, AActor* damageCauser)
    {
        DamageAmount = damageAmount;
        DamageEvent = damageEvent;
        EventInstigator = eventInstigator;
        DamageCauser = damageCauser;
    }
    
};
