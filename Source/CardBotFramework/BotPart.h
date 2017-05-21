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
    /** Helper to get a component from its name */
    UActorComponent* GetComponentByName(FName name);
	
    /** Get a socket by name */
    USocketComponent* GetSocket(FName name);
    
    /** Get a plug by name */
    UPlugComponent* GetPlug(FName name);
    
    /** Indicates if Part has sockets */
    inline bool HasSockets() { return this->FindComponentByClass(USocketComponent::StaticClass()) != NULL;}
    
    /** Indicates if Part has plugs */
    inline bool HasPlugs() { return this->FindComponentByClass(UPlugComponent::StaticClass()) != NULL;}
	
};
