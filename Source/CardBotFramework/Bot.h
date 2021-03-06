#pragma once

#include "GameFramework/Actor.h"
#include "BotPart.h"
#include "Constants.h"
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
    
private:
    /** Dynamic multicast delegate used to send events to parts */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPerformActionSignature, FName, actionName, UObject*, actionData);
    /** Delegates are indexed by types */
    TMap<EActionType, TSharedPtr<FPerformActionSignature>> PerformActionMulticastDelegates;

    /** Recursive function to parse and assemble parts */
    bool AssemblePart(ABotPart &part, TArray<ABotPart*> *parts = nullptr);
    
    /** Recursive function to parse and reset parts */
    void ResetPart(ABotPart &part);
    
    /** Connect a part properly */
    void ConnectPart(ABotPart &part, USocketComponent *socket = nullptr, UPlugComponent *plug = nullptr);
    
    /** Disconnect a part properly */
    void DisconnectPart(ABotPart &part, USocketComponent *socket = nullptr, UPlugComponent *plug = nullptr, bool isDead = false, bool logicalOnly = true);
    
protected:
    
    /** The root part of the Bot */
    ABotPart* RootPart;
    
    /** Override to support ConstructionScript Bot */
    virtual void PreInitializeComponents() override;
    
public:
    
    /** Recursive function to break sockets constraint */
    void BreakSocket(USocketComponent& socket, bool destroyChild, bool recursive);
    
    /** Try to parse all parts and build Bot instance (assemble/connect) */
    bool Rebuild();
    
    /** Destroy a part properly */
    void DestroyPart(ABotPart &part);
    
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
    ABot* BreakSocket(FName name, bool recursive = false);
    
    /** Helper to get a component from its name */
    UFUNCTION(BlueprintCallable)
    UActorComponent* GetComponentByName(FName name) const;
    
    /** Send an Action to bot which acts as a hub for parts on action handling */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void PerformAction(EActionType actionType, FName actionName, UObject* actionData);
    
    /** Called after a BotPart has been added */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnPartAdded(ABotPart* part);
    
    /** Called after a BotPart has been removed */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnPartRemoved(ABotPart* part);
    
    /** Called after a BotPart has been removed */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnPartBroken(ABotPart* part);
    
    /** Handles sensor events sent from parts */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnSensorEvent(ESensorType sensorType, FName eventName, ABotPart* part, UObject* eventData);
    
    /** Reroute PartAdded event on Bot */
    UFUNCTION() virtual void NotifyOnPartAdded(ABotPart* part);
    
    /** Reroute PartRemoved event on Bot */
    UFUNCTION() virtual void NotifyOnPartRemoved(ABotPart* part);
    
    /** Reroute SensorEvent event on Bot */
    UFUNCTION() virtual void NotifyOnSensorEvent(ESensorType sensorType, FName eventName, ABotPart* part, UObject* eventData);
};
