

#include "CardBotFramework.h"
#include "SocketComponent.h"


USocketComponent::USocketComponent()
{
    bConnected = false;
	PrimaryComponentTick.bCanEverTick = false;
}
