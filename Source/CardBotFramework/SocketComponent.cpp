

#include "CardBotFramework.h"
#include "SocketComponent.h"


USocketComponent::USocketComponent()
{
    bConnected = false;
    Plug = NULL;
	PrimaryComponentTick.bCanEverTick = false;
}
