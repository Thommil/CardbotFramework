#pragma once

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
