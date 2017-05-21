#include "CardBotFramework.h"
#include "BotPart.h"


ABotPart::ABotPart()
{
	PrimaryActorTick.bCanEverTick = true;
}


USocketComponent* ABotPart::GetSocket(FName name)
{
    return NULL;
}

UPlugComponent* ABotPart::GetPlug(FName name)
{
    return NULL;
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
