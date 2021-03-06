#pragma once

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GameFramework/Actor.h"
#include "Constants.h"
#include "ObjectPool.h"
#include "CollisionEventData.h"
#include "DamageEventData.h"
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

private:
    /** Wrapper to route broken events to owners */
    UFUNCTION()
    void OnPlugBrokenWrapper(int32 ConstraintIndex = 0);
    
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

private:
    TSharedPtr<TObjectPool<UCollisionEventData>> CollisionEventDataPool;
    TSharedPtr<TObjectPool<UDamageEventData>> DamageEventDataPool;
    
public:
    
    /** Indicates the event types routed to this part */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot", meta=(Bitmask, BitmaskEnum=EActionType))
    int32 ActionCapabilites;
    
    /** Life of the part (max = 1.0) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    float LifeRate;
    
    /** Indicates that part is disconnected on death */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    bool bBreakOnDeath;
    
    /** Dynamic multicast delegate used to send sensor events to bot */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSensorEventSignature, ESensorType, sensorType, FName, eventName, ABotPart*, part, UObject*, eventData);
    FOnSensorEventSignature OnSensorEventDelegate;
    
    /** Helper to get a component from its name */
    UFUNCTION(BlueprintCallable)
    UActorComponent* GetComponentByName(FName name) const;
	
    /** Gets the Bot owning this part */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    ABot* GetBot() const;
    
    /** Indicates if Part has sockets */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    inline bool HasSockets() const { return FindComponentByClass(USocketComponent::StaticClass()) != nullptr;}
    
    /** Get all sockets in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetSockets(TArray<USocketComponent*>& sockets) const;
    
    /** Indicates if Part has plugs */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    bool HasPlugs() const { return FindComponentByClass(UPlugComponent::StaticClass()) != nullptr;}
    
    /** Get a socket by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    USocketComponent* GetSocket(FName name) const;
    
    /** Get all plugs in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetPlugs(TArray<UPlugComponent*>& plugs) const;
    
    /** Get a plug by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UPlugComponent* GetPlug(FName name) const;
    
    /** Generate a sensor event sent to owning Bot */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GenerateSensorEvent(ESensorType sensorType, FName eventName, UObject* eventData) const;
    
    /** Called after a connection to a owned socket has been made */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnSocketConnected(USocketComponent *socket);
    
    /** Called after a connection to a owned socket has been borken */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnSocketBroken(USocketComponent *socket);
    
    /** Called after a connection to a owned plug has been made */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnPlugConnected(UPlugComponent *plug);
    
    /** Called after a connection to a owned plug has been borken */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void OnPlugBroken(UPlugComponent *plug);
    
    /** Called when Bot reroute a received event compliant with current part */
    UFUNCTION(BlueprintImplementableEvent, Category="CardBot")
    void PerformAction(FName actionName, UObject* actionData);

    /** Reroute SocketConnected event on Bot */
    UFUNCTION() virtual void NotifyOnSocketConnected(USocketComponent *socket);
    
    /** Reroute SocketBroken event on Bot */
    UFUNCTION() virtual void NotifyOnSocketBroken(USocketComponent *socket);
    
    /** Reroute PlugConnected event on Bot */
    UFUNCTION() virtual void NotifyOnPlugConnected(UPlugComponent *plug);
    
    /** Reroute PlugBroken event on Bot */
    UFUNCTION() virtual void NotifyOnPlugBroken(UPlugComponent *plug);
    
    /** Override to set up pools */
    virtual void PostInitializeComponents() override;
    
    /** Override Hit mechanism to reroute on Bot */
    virtual void NotifyHit(class UPrimitiveComponent * MyComp, AActor * Other, class UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) override;
    
    /** Override BeginOverlap mechanism to reroute on Bot */
    virtual void NotifyActorBeginOverlap(AActor * OtherActor) override;
    
    /** Override EndOverlap mechanism to reroute on Bot */
    virtual void NotifyActorEndOverlap(AActor * OtherActor) override;
    
    /** Override Damage mechanism to reroute on Bot */
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
