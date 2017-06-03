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
    
    inline void SetEventData(UPrimitiveComponent *PartComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent, bool bSelfMoved, FVector HitLocation, FVector HitNormal ,FVector NormalImpulse, const FHitResult *Hit)
    {
        this->PartComponent = PartComponent;
        this->OtherActor = OtherActor;
        this->OtherComponent = OtherComponent;
        this->bSelfMoved = bSelfMoved;
        this->HitLocation = HitLocation;
        this->HitNormal = HitNormal;
        this->NormalImpulse = NormalImpulse;
        this->Hit = *Hit;
    }
    
};
