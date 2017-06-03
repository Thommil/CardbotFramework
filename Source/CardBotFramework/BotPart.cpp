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
    Plug = plug;
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
    OnPlugBrokenWrapper(0);
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
            socketPart->OnSocketConnected(socket);
            plugPart->OnPlugConnected(this);
            OnConstraintBroken.AddUniqueDynamic(this, &UPlugComponent::OnPlugBrokenWrapper);
        }
    }
    Socket = socket;
}

void UPlugComponent::OnPlugBrokenWrapper(int32 ConstraintIndex)
{
    OnConstraintBroken.RemoveAll(this);
    ABotPart *plugPart = static_cast<ABotPart *>(GetOwner());
    plugPart->OnPlugBroken(this);
    if(Socket != nullptr)
    {
        ABotPart *socketPart = static_cast<ABotPart *>(Socket->GetOwner());
        socketPart->OnSocketBroken(Socket);
    }
}

USocketComponent* UPlugComponent::GetSocket() {
    return Socket;
}


ABotPart::ABotPart()
{
	PrimaryActorTick.bCanEverTick = true;
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

ABot* ABotPart::GetBot() const
{
    return (GetParentComponent() != nullptr) ? static_cast<ABot*>(GetParentComponent()->GetOwner()) : nullptr;
}

void ABotPart::GenerateSensorEvent(ESensorType sensorType, UObject* sensorData) const
{
    OnSensorEventDelegate.Broadcast(sensorType, const_cast<ABotPart*>(this), sensorData);
}
