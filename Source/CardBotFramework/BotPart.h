#pragma once

#include "GameFramework/Actor.h"
#include "PlugComponent.h"
#include "SocketComponent.h"
#include "BotPart.generated.h"

/**
 *  Functional Part of a Bot
 *      - owns StaticMeshes and Physiscs of the part
 *      - owns Sockets and Plugs for parts assembly by the Bot instance
 *      -
 */
UCLASS()
class CARDBOTFRAMEWORK_API ABotPart : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABotPart();
    
public:
    
    virtual void OnConstruction(const FTransform & Transform) override;
    
    /** Helper to get a component from its name */
    UFUNCTION(BlueprintCallable)
    UActorComponent* GetComponentByName(FName name);
	
    /** Indicates if Part has sockets */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    inline bool HasSockets() { return this->FindComponentByClass(USocketComponent::StaticClass()) != NULL;}
    
    /** Get all sockets in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetSockets(TArray<USocketComponent*>& sockets);
    
    /** Indicates if Part has plugs */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    bool HasPlugs() { return this->FindComponentByClass(UPlugComponent::StaticClass()) != NULL;}
    
    /** Get a socket by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    USocketComponent* GetSocket(FName name);
    
    /** Get all plugs in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetPlugs(TArray<UPlugComponent*>& plugs);
    
    /** Get a plug by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UPlugComponent* GetPlug(FName name);
};
