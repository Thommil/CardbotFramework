#pragma once

#include "Components/SceneComponent.h"
#include "PlugComponent.h"
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

protected:
    
    /** Indicates if the current socket/plug pair is connected (physics)*/
    bool bConnected;
    
    /** The associated PlugComponent instance (indicates that assembly is done)*/
    UPlugComponent* Plug;
    
public:	

    /** Name/Function of this socket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    FName Name;
    
    /** StaticMesh to connect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    FName ComponentName;
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void SetPlug(UPlugComponent* plug) { Plug = plug;}
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UPlugComponent* GetPlug() { return Plug;}
	
};
