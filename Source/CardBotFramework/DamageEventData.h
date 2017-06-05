#pragma once

#include "UObject/NoExportTypes.h"
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

class CARDBOTFRAMEWORK_API FDamageEventDataPool
{
private:
    TSet<UDamageEventData*> InnerPool;
    
    inline FDamageEventDataPool(uint16 initialCapacity = DEFAULT_DAMAGE_EVENTDATA_POOL_SIZE)
    {
        InnerPool.Reserve(initialCapacity);
        for(uint16 i=0; i < initialCapacity; i++)
        {
            InnerPool.Add(NewObject<UDamageEventData>());
        }
    }
    
public:
    inline static FDamageEventDataPool& Instance()
    {
        static FDamageEventDataPool Instance;
        return Instance;
    }
    
    inline virtual ~FDamageEventDataPool()
    {
        InnerPool.Empty();
    }
    
public:
    inline UDamageEventData* Pull()
    {
        static FSetElementId index = FSetElementId::FromInteger(0);
        return InnerPool[index];
    }
    
    inline void PushBack(UDamageEventData* DamageEventData)
    {
        InnerPool.Add(DamageEventData);
    }
};
