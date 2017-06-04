#pragma once

#include "UObject/NoExportTypes.h"
#include "CollisionEventData.generated.h"

/**
 * Collision event data to be passed through sensors
 */
UCLASS(BlueprintType)
class CARDBOTFRAMEWORK_API UCollisionEventData : public UObject
{
	GENERATED_BODY()
	
public:
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    UPrimitiveComponent *PartComponent;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    AActor *OtherActor;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    UPrimitiveComponent *OtherComponent;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    bool bSelfMoved;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    FVector HitLocation;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    FVector HitNormal;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    FVector NormalImpulse;
    
    UPROPERTY(BlueprintReadOnly, Category="CardBot")
    FHitResult Hit;
    
    inline void SetEventData(UPrimitiveComponent *partComponent, AActor *otherActor, UPrimitiveComponent *otherComponent, bool bselfMoved, FVector &hitLocation, FVector &hitNormal ,FVector &normalImpulse, const FHitResult &hit)
    {
        PartComponent = partComponent;
        OtherActor = otherActor;
        OtherComponent = otherComponent;
        bSelfMoved = bselfMoved;
        HitLocation = hitLocation;
        HitNormal = hitNormal;
        NormalImpulse = normalImpulse;
        Hit = hit;
    }
    
};

class CARDBOTFRAMEWORK_API FCollisionEventDataPool
{
private:
    TSet<UCollisionEventData*> InnerPool;
    
    inline FCollisionEventDataPool(uint16 initialCapacity = DEFAULT_COLLISION_EVENTDATA_POOL_SIZE)
    {
        InnerPool.Reserve(initialCapacity);
        for(uint16 i=0; i < initialCapacity; i++)
        {
            InnerPool.Add(NewObject<UCollisionEventData>());
        }
    }
    
public:
    inline static FCollisionEventDataPool& Instance()
    {
        static FCollisionEventDataPool Instance;
        return Instance;
    }
    
    inline virtual ~FCollisionEventDataPool()
    {
        InnerPool.Empty();
    }
    
public:
    inline UCollisionEventData* Pull()
    {
        static FSetElementId index = FSetElementId::FromInteger(0);
        return InnerPool[index];
    }
    
    inline void PushBack(UCollisionEventData* CollisionEventData)
    {
        InnerPool.Add(CollisionEventData);
    }
};
