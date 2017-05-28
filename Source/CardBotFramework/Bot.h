#pragma once

#include "GameFramework/Actor.h"
#include "BotPart.h"
#include "Moveable.h"
#include "Bot.generated.h"

/**
 *  Bot Actor
 *      - made of BotPart Actors -> assemble/connect based on plugs and sockets
 */
UCLASS()
class CARDBOTFRAMEWORK_API ABot : public AActor, public IMoveable
{
	GENERATED_BODY()
	
public:	

	ABot();
    
protected:
    
    /** The root part of the Bot */
    ABotPart* RootPart;
    
    /** Override to support ConstructionScript Bot */
    virtual void PreInitializeComponents() override;
    
    /** Recursive function to parse and assemble parts */
    bool AssemblePart(ABotPart &part, TArray<ABotPart*> *parts = nullptr);

    /** Recursive function to parse and reset parts */
    void ResetPart(ABotPart &part);
    
    /** Destroy a part properly */
    void DestroyPart(ABotPart &part);
    
    /** Recursive function to break sockets constraint */
    void BreakSocket(USocketComponent& socket, bool destroyChild = false, bool recursive = false);
    
    /** Try to parse all parts and build Bot instance (assemble/connect) */
    bool Rebuild();
    
public:
    
    /** Reset bot and parts */
    UFUNCTION(BlueprintCallable)
    virtual void Reset() override;
	
    /** Get all parts contained in the Actor, assembled or not */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetParts(TArray<ABotPart*>& parts) const;
    
    /** Get all part from its name, assembled or not  */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    ABotPart* GetPart(FName name) const;
    
    /** Add a Part to the Bot (Not assembled now but transformed if needed) */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    ABot* AddPart(TSubclassOf<ABotPart> partClass, FName name);
    
    /** Remove a Part, remove all children too, if assembled, disassemble safely first
     */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    ABot* RemovePart(FName name);
    
    /** Break a socket or all sockets based on name.
     */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    ABot* BreakSocket(FName name, bool all = true, bool recursive = false);
    
    /** Helper to get a component from its name */
    UFUNCTION(BlueprintCallable)
    UActorComponent* GetComponentByName(FName name) const;
    
    /** Called after a BotPart has been added */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void OnPartAdded(ABotPart* part);
    virtual void OnPartAdded_Implementation(ABotPart* part){}
    
    /** Called after a BotPart has been removed */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void OnPartRemoved(ABotPart* part);
    virtual void OnPartRemoved_Implementation(ABotPart* part){}
    
    
    virtual float SetSpeed_Implementation(float speedRate) override  {return 0;};
    
    virtual float GetSpeed_Implementation() override {return 0;};

};
