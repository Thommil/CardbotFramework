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
    void AssemblePart(ABotPart &part);
    
    /** Recursive function to parse and disassemble parts */
    void DisassemblePart(ABotPart &part, bool recursive = false);
    
public:	
	
    /** Add a Part to the Bot (Not assembled now but transformed if needed) */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    ABot* AddPart(TSubclassOf<ABotPart> partClass, FName name);
    
    /** Remove a Part, remove all children too, if assembled, disassemble safely first
     */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    ABot* RemovePart(FName name, bool all = true);
    
    /** Break a socket or all sockets based on name.
     */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void BreakSocket(FName name, bool all = true, bool recursive = false);
    
    /** Helper to get a component from its name */
    UFUNCTION(BlueprintCallable)
    UActorComponent* GetComponentByName(FName name);
	
};
