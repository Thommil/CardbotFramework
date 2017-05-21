#pragma once

#include "Components/SceneComponent.h"
#include "SocketComponent.generated.h"

/**
 *  Socket (physics) based on its Name (function)
 *      - set ComponentName to a StaticMesh to connect
 */
UCLASS( ClassGroup=(CardBot), meta=(BlueprintSpawnableComponent) )
class CARDBOTFRAMEWORK_API USocketComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	
	USocketComponent();

public:	

    /** Name/Function of this socket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="connector")
    FName Name;
    
    /** StaticMesh to connect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="connector")
    FName ComponentName;
	
};
