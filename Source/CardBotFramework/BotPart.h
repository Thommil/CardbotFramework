#pragma once

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GameFramework/Actor.h"
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
    
    /** Reset the socket (disconnect) */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void Reset() { bConnected = false; Plug = NULL;}
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void SetPlug(UPlugComponent* plug) { Plug = plug;}
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UPlugComponent* GetPlug() { return Plug;}
    
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
    
    /** Indicates if the current socket/plug pair is connected (physics)*/
    bool bConnected;
    
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
    void Reset() { bConnected = false; Socket = NULL;}
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void SetSocket(USocketComponent* socket) { Socket = socket;}
    
    UFUNCTION(BlueprintCallable, Category="CardBot")
    USocketComponent* GetSocket() { return Socket;}
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
    
    /** Helper to get a component from its name */
    UFUNCTION(BlueprintCallable)
    UActorComponent* GetComponentByName(FName name) const;
	
    /** Indicates if Part has sockets */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    inline bool HasSockets() const { return this->FindComponentByClass(USocketComponent::StaticClass()) != NULL;}
    
    /** Get all sockets in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetSockets(TArray<USocketComponent*>& sockets) const;
    
    /** Indicates if Part has plugs */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    bool HasPlugs() const { return this->FindComponentByClass(UPlugComponent::StaticClass()) != NULL;}
    
    /** Get a socket by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    USocketComponent* GetSocket(FName name) const;
    
    /** Get all plugs in a TArray */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void GetPlugs(TArray<UPlugComponent*>& plugs) const;
    
    /** Get a plug by name */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UPlugComponent* GetPlug(FName name) const;
};
