#include "CardBotFramework.h"
#include "PlugComponent.h"

UPlugComponent::UPlugComponent()
{
    bConnected = false;
    PrimaryComponentTick.bCanEverTick = false;
}
