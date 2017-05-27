#include "CardBotFramework.h"
#include "BotPart.h"

USocketComponent::USocketComponent()
{
    Plug = NULL;
    PrimaryComponentTick.bCanEverTick = false;
}


UPlugComponent::UPlugComponent()
{

    Socket = NULL;
    PrimaryComponentTick.bCanEverTick = false;
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
    return NULL;
}


void ABotPart::GetSockets(TArray<USocketComponent*>& sockets) const
{
    sockets.Empty();
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(USocketComponent::StaticClass()))
        {
            sockets.Add((USocketComponent*)component);
        }
    }
}

USocketComponent* ABotPart::GetSocket(FName name) const
{
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(USocketComponent::StaticClass()) && ((USocketComponent*)component)->Name == name)
        {
            return (USocketComponent*)component;
        }
    }
    return NULL;
}

void ABotPart::GetPlugs(TArray<UPlugComponent*>& plugs) const
{
    plugs.Empty();
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(UPlugComponent::StaticClass()))
        {
            plugs.Add((UPlugComponent*)component);
        }
    }
}


UPlugComponent* ABotPart::GetPlug(FName name) const
{
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->IsA(UPlugComponent::StaticClass()) && ((UPlugComponent*)component)->Name == name)
        {
            return (UPlugComponent*)component;
        }
    }
    return NULL;
}

