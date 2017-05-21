#pragma once

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PlugComponent.generated.h"

/**
 *  Plugs (physics) based on its corresponding socket Name (function) 
 *  Socket data will be used a second component for hinge joint
 */
UCLASS( ClassGroup=(CardBot), meta=(BlueprintSpawnableComponent) )
class CARDBOTFRAMEWORK_API UPlugComponent : public UPhysicsConstraintComponent
{
	GENERATED_BODY()
	
public:
    
    UPlugComponent();
    
public:
    
    /** Name/Function of this plug */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="connector")
    FName Name;
};  
