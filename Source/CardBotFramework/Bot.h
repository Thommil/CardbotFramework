#pragma once

#include "GameFramework/Actor.h"
#include "BotPart.h"
#include "Bot.generated.h"

/**
 *  Bot Actor
 *      - made of BotPart Actors -> assemble/connect based on plugs and sockets
 */
UCLASS()
class CARDBOTFRAMEWORK_API ABot : public AActor
{
	GENERATED_BODY()
	
public:	

	ABot();

protected:
    
    /** All parts */
    TArray<ABotPart*> Parts;
    
    virtual void PreInitializeComponents() override;
    
    /** Recursive function to parse and assemble parts */
    void assemblePart(ABotPart &part);
    
    /** Recursive function to parse and disassemble parts */
    void disassemblePart(ABotPart &part);
    
public:	
	
    /** Add a Part to the Bot (Not assembled now but transformed if needed) */
    UFUNCTION(BlueprintCallable)
    void addPart(ABotPart* part);
    
    /** Remove a Part based on its connecting socket
     *      > Remove all children too
     *      > If assembled, disassemble safely first
     */
    UFUNCTION(BlueprintCallable)
    void removePart(FName socketName);
    
    /** Helper to get a component from its name */
    UActorComponent* GetComponentByName(FName name);
	
};
