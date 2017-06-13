#include "CardBotFramework.h"
#include "Bot.h"
#include "BotPart.h"


USocketComponent::USocketComponent()
{
    Plug = nullptr;
    PrimaryComponentTick.bCanEverTick = false;
}

void USocketComponent::Reset()
{
    Plug = nullptr;
}


void USocketComponent::SetPlug(UPlugComponent* plug)
{
    if(plug != nullptr)
    {
        Plug = plug;
    }
    else
    {
        Reset();
    }
}

UPlugComponent* USocketComponent::GetPlug()
{
    return Plug;
}

UPlugComponent::UPlugComponent()
{
    Socket = nullptr;
    PrimaryComponentTick.bCanEverTick = false;
}

void UPlugComponent::Reset() {
    OnPlugBrokenWrapper();
    TermComponentConstraint();
    Socket = nullptr;
}

void UPlugComponent::SetSocket(USocketComponent* socket) {
    if(socket != nullptr)
    {
        ABotPart *socketPart = static_cast<ABotPart *>(socket->GetOwner());
        ABotPart *plugPart = static_cast<ABotPart *>(GetOwner());
        UActorComponent* socketBody = socketPart->GetComponentByName(socket->ComponentName);
        UActorComponent* plugBody = plugPart->GetComponentByName(ComponentName);
        if(socketBody != nullptr && plugBody != nullptr && socketBody->IsA(UPrimitiveComponent::StaticClass()) && plugBody->IsA(UPrimitiveComponent::StaticClass()))
        {
            SetConstrainedComponents(static_cast<UPrimitiveComponent *>(socketBody), NAME_None, static_cast<UPrimitiveComponent *>(plugBody), NAME_None);
            socketPart->NotifyOnSocketConnected(socket);
            plugPart->NotifyOnPlugConnected(this);
            OnConstraintBroken.AddUniqueDynamic(this, &UPlugComponent::OnPlugBrokenWrapper);
        }
        Socket = socket;
    }
    else
    {
        Reset();
    }
    
}

void UPlugComponent::OnPlugBrokenWrapper(int32 ConstraintIndex)
{
    OnConstraintBroken.RemoveAll(this);
    ABotPart *plugPart = static_cast<ABotPart *>(GetOwner());
    plugPart->NotifyOnPlugBroken(this);
    if(Socket != nullptr)
    {
        ABotPart *socketPart = static_cast<ABotPart *>(Socket->GetOwner());
        socketPart->NotifyOnSocketBroken(Socket);
    }
}

USocketComponent* UPlugComponent::GetSocket() {
    return Socket;
}

ABotPart::ABotPart()
{
    LifeRate = 1.0f;
    bBreakOnDeath = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ABotPart::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    if(bCanBeDamaged)
    {
        DamageEventDataPool = MakeShareable(new TObjectPool<UDamageEventData>());
    }
    TArray<UActorComponent*> components = this->GetComponentsByClass(UPrimitiveComponent::StaticClass());
    for (UActorComponent* component : components)
    {
        UPrimitiveComponent* primitiveComponent = static_cast<UPrimitiveComponent*>(component);
        if(primitiveComponent->BodyInstance.bNotifyRigidBodyCollision)
        {
            CollisionEventDataPool = MakeShareable(new TObjectPool<UCollisionEventData>());
            break;
        }
    }
}

UActorComponent* ABotPart::GetComponentByName(FName name) const
{
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->GetFName() == name){
            return component;
        }
    }
    return nullptr;
}

ABot* ABotPart::GetBot() const
{
    return (GetParentComponent() != nullptr) ? static_cast<ABot*>(GetParentComponent()->GetOwner()) : nullptr;
}

void ABotPart::GetSockets(TArray<USocketComponent*>& sockets) const
{
    sockets.Empty();
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(USocketComponent::StaticClass()))
        {
            sockets.Add(static_cast<USocketComponent *>(component));
        }
    }
}

USocketComponent* ABotPart::GetSocket(FName name) const
{
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(USocketComponent::StaticClass()) && static_cast<USocketComponent *>(component)->Name == name)
        {
            return static_cast<USocketComponent *>(component);
        }
    }
    return nullptr;
}

void ABotPart::GetPlugs(TArray<UPlugComponent*>& plugs) const
{
    plugs.Empty();
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(UPlugComponent::StaticClass()))
        {
            plugs.Add(static_cast<UPlugComponent *>(component));
        }
    }
}


UPlugComponent* ABotPart::GetPlug(FName name) const
{
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(UPlugComponent::StaticClass()) && static_cast<UPlugComponent *>(component)->Name == name)
        {
            return static_cast<UPlugComponent *>(component);
        }
    }
    return nullptr;
}

void ABotPart::NotifyOnSocketConnected(USocketComponent *socket)
{
    OnSocketConnected(socket);
}

void ABotPart::NotifyOnSocketBroken(USocketComponent *socket)
{
    OnSocketBroken(socket);
}

void ABotPart::NotifyOnPlugConnected(UPlugComponent *plug)
{
    OnPlugConnected(plug);
}

void ABotPart::NotifyOnPlugBroken(UPlugComponent *plug)
{
    OnPlugBroken(plug);
}

void ABotPart::NotifyHit(class UPrimitiveComponent * MyComp, AActor * Other, class UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
    UCollisionEventData *CollisionEventData = CollisionEventDataPool->Pop();
    CollisionEventData->SetEventData(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
    GenerateSensorEvent(ESensorType::Collision, FName(TEXT("OnPartHit"), EFNameIndex::OnPartHit), CollisionEventData);
    CollisionEventDataPool->Push(CollisionEventData);
}

void ABotPart::NotifyActorBeginOverlap(AActor * OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    GenerateSensorEvent(ESensorType::Collision, FName(TEXT("OnPartBeginOverlap"), EFNameIndex::OnPartBeginOverlap), OtherActor);
}

void ABotPart::NotifyActorEndOverlap(AActor * OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);
    GenerateSensorEvent(ESensorType::Collision, FName(TEXT("OnPartEndOverlap"), EFNameIndex::OnPartEndOverlap), OtherActor);
}

float ABotPart::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float finalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if(bCanBeDamaged)
    {
        UDamageEventData *DamageEventData = DamageEventDataPool->Pop();
        DamageEventData->SetEventData(finalDamageAmount, DamageEvent, EventInstigator, DamageCauser);
        GenerateSensorEvent(ESensorType::Damage, FName(TEXT("OnPartDamage"), EFNameIndex::OnPartDamage), DamageEventData);
        DamageEventDataPool->Push(DamageEventData);
        
        LifeRate -= finalDamageAmount;
        
        if(LifeRate <= 0)
        {
            GenerateSensorEvent(ESensorType::Damage, FName(TEXT("OnPartBroken"), EFNameIndex::OnPartBroken), nullptr);
        }
    }
    return finalDamageAmount;
}


void ABotPart::GenerateSensorEvent(ESensorType sensorType, FName eventName, UObject* eventData) const
{
    OnSensorEventDelegate.Broadcast(sensorType, eventName, const_cast<ABotPart*>(this), eventData);
}
