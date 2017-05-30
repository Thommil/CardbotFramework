#pragma once

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GameFramework/Actor.h"
#include "BaseEvent.h"
#include "BotPart.generated.h"

/**
 *  Socket (physics) based on its Name (function)
 *      - set ComponentName to a StaticMesh to connect
 */
UCLASS(ClassGroup=(CardBot), meta=(BlueprintSpawnableComponent) )
class CARDBOTFRAMEWORK_API USocketComponent : public USceneComponent
{
    GENERATED_BODY()
    
public:
    
    USocketComponent();
    
protected:
    
    /** The associated PlugComponent instance (indicates that assembly is done)*/
    UPlugComponent* Plug;
    
public:
    
    /** Name/Function of this socket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    FName Name;
    
    /** StaticMesh to connect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    FName ComponentName;
    
    /** Reset the socket (disconnect) */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void Reset();
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void SetPlug(UPlugComponent* plug);
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UPlugComponent* GetPlug();
    
};

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
    
protected:
    
    /** The associated SocketComponent instance (indicates that assembly is done)*/
    USocketComponent* Socket;
    
public:
    
    /** Name/Function of this plug */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    FName Name;
    
    /** StaticMesh to connect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    FName ComponentName;
    
    /** Reset the plug (disconnect) */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void Reset();
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void SetSocket(USocketComponent* socket);
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    USocketComponent* GetSocket();
};

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
    
    /** Indicates the event types routed to this part */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot", meta=(Bitmask, BitmaskEnum=EEventType))
    int32 EventTypeFlags;
    
    /** Helper to get a component from its name */
    UFUNCTION(BlueprintCallable)
    UActorComponent* GetComponentByName(FName name) const;
	
    /** Indicates if Part has sockets */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    inline bool HasSockets() const { return this->FindComponentByClass(USocketComponent::StaticClass()) != nullptr;}
    
    /** Get all sockets in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetSockets(TArray<USocketComponent*>& sockets) const;
    
    /** Indicates if Part has plugs */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    bool HasPlugs() const { return this->FindComponentByClass(UPlugComponent::StaticClass()) != nullptr;}
    
    /** Get a socket by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    USocketComponent* GetSocket(FName name) const;
    
    /** Get all plugs in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetPlugs(TArray<UPlugComponent*>& plugs) const;
    
    /** Get a plug by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UPlugComponent* GetPlug(FName name) const;
    
    /** Called after a connection to a owned socket has been made */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void OnSocketConnected(USocketComponent *socket);
    virtual void OnSocketConnected_Implementation(USocketComponent *socket){}
    
    /** Called after a connection to a owned socket has been borken */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void OnSocketBroken(USocketComponent *socket);
    virtual void OnSocketBroken_Implementation(USocketComponent *socket){}
    
    /** Called after a connection to a owned plug has been made */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void OnPlugConnected(UPlugComponent *plug);
    virtual void OnPlugConnected_Implementation(UPlugComponent *plug){}
    
    /** Called after a connection to a owned plug has been borken */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void OnPlugBroken(UPlugComponent *plug);
    virtual void OnPlugBroken_Implementation(UPlugComponent *plug){}
    
    /** Called when Bot reroute a received event compliant with current part */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void OnBotEvent(UBaseEvent* event);
    virtual void OnBotEvent_Implementation(UBaseEvent* event){WARNING(TEXT("HIT"));}
    
    /** Called on received event from Bot (route event to OnEvent)*/
    UFUNCTION(Category="CardBot")
    virtual void HandleBotEvent(TSubclassOf<UBaseEvent> event);
};
