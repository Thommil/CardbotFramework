#include "CardBotFramework.h"
#include "BotPart.h"


ABotPart::ABotPart()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABotPart::OnConstruction(const FTransform & Transform)
{
    INFO(TEXT("Touched"));
}

UActorComponent* ABotPart::GetComponentByName(FName name)
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


void ABotPart::GetSockets(TArray<USocketComponent*>& sockets)
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

USocketComponent* ABotPart::GetSocket(FName name)
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

void ABotPart::GetPlugs(TArray<UPlugComponent*>& plugs)
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


UPlugComponent* ABotPart::GetPlug(FName name)
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
