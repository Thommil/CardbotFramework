#include "CardBotFramework.h"
#include "Wireless.h"
#include "DamageEventData.h"
#include "Bot.h"

ABot::ABot()
{
    RootPart = nullptr;
    PrimaryActorTick.bCanEverTick = true;

    UEnum* EActionTypeEnum = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("EActionType"), true);
    for(int i=0; i < (EActionTypeEnum->NumEnums() - 1); i++)
    {
        TSharedPtr<FPerformActionSignature> performActionDelegate(new FPerformActionSignature());
        PerformActionMulticastDelegates.Add(static_cast<EActionType>(EActionTypeEnum->GetValueByIndex(i)), performActionDelegate);
    }
}

void ABot::PreInitializeComponents()
{
    //For Editor mode
    if(RootPart == nullptr)
    {
        Rebuild();
    }
}

void ABot::Reset()
{
    Super::Reset();
    if(RootPart != nullptr)
    {
        ResetPart(*RootPart);
    }
}

UActorComponent* ABot::GetComponentByName(FName name) const
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

void ABot::GetParts(TArray<ABotPart*>& parts) const
{
    parts.Empty();
    TArray<UActorComponent*> childActorComponents = GetComponentsByClass(UChildActorComponent::StaticClass());
    for(UActorComponent* childActorComponent : childActorComponents)
    {
        if(static_cast<UChildActorComponent *>(childActorComponent)->GetChildActor()->IsA(ABotPart::StaticClass()))
        {
            parts.Add(static_cast<ABotPart *>(static_cast<UChildActorComponent *>(childActorComponent)->GetChildActor()));
        }
    }
}

ABotPart* ABot::GetPart(FName name) const
{
    TArray<UActorComponent*> childActorComponents = GetComponentsByClass(UChildActorComponent::StaticClass());
    for(UActorComponent* childActorComponent : childActorComponents)
    {
        if(static_cast<UChildActorComponent *>(childActorComponent)->GetChildActor()->IsA(ABotPart::StaticClass()) && childActorComponent->GetFName() == name)
        {
            return static_cast<ABotPart *>(static_cast<UChildActorComponent *>(childActorComponent)->GetChildActor());
        }
    }
    return nullptr;
}

bool ABot::Rebuild()
{
    //Optim
    TArray<ABotPart*> parts;
    GetParts(parts);
    
    if(parts.Num() > 0)
    {
        //First if RootPart is not set, seek one
        if(RootPart == nullptr)
        {
            for(ABotPart* part : parts)
            {
                if(RootPart == nullptr && part->HasSockets() && !part->HasPlugs())
                {
                    RootPart = part;
                    ConnectPart(*RootPart);
                    NotifyOnPartAdded(RootPart);
                }
            }
        }
    
        if(RootPart != nullptr)
        {
            return AssemblePart(*RootPart, &parts);
        }
        else
        {
            ERROR(TEXT("No root part found (must have sockets and no plugs)"));
            return false;
        }
    }
    
    return true;
}

bool ABot::AssemblePart(ABotPart& part, TArray<ABotPart*> *parts)
{
    //Optim
    if(parts == nullptr)
    {
        TArray<ABotPart*> _parts;
        GetParts(_parts);
        parts = &_parts;
    }
    
    TArray<USocketComponent*> sockets;
    part.GetSockets(sockets);
    
    //Parse all sockets to find not assembled ones
    for(USocketComponent* socket : sockets)
    {
        if(socket->GetPlug() == nullptr)
        {
            for(ABotPart* plugPart : *parts)
            {
                UPlugComponent* plug = plugPart->GetPlug(socket->Name);
                if(plug != nullptr && plug->GetSocket() == nullptr)
                {
                    //Runtime, place -> component and actor directly
                    if(!plugPart->GetParentComponent()->IsCreatedByConstructionScript())
                    {
                        plugPart->SetActorRotation(socket->GetComponentRotation() - plug->GetComponentRotation());
                        plugPart->SetActorLocation(GetActorLocation() + socket->GetComponentLocation() - plug->GetComponentLocation());
                    }
                    
                    //Connect
                    ConnectPart(*plugPart, socket, plug);
                    NotifyOnPartAdded(plugPart);
                    break;
                }
            }
        }
        
        //Recursive
        if(socket->GetPlug() != nullptr && !AssemblePart(*static_cast<ABotPart *>(socket->GetPlug()->GetOwner()), parts))
        {
            return false;
        }
    }
 
    return true;
}

void ABot::ConnectPart(ABotPart &part, USocketComponent *socket, UPlugComponent *plug)
{
    //Physics
    if(socket != nullptr && plug != nullptr)
    {
        socket->SetPlug(plug);
        plug->SetSocket(socket);
    }
    
    //Logic
    UEnum* EActionTypeEnum = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("EActionType"), true);
    for(int i=0; i < (EActionTypeEnum->NumEnums() - 1); i++)
    {
        EActionType ActionType = static_cast<EActionType>(EActionTypeEnum->GetValueByIndex(i));
        if (((1 << static_cast<int32>(ActionType)) & part.ActionCapabilites) != 0)
        {
            TSharedPtr<FPerformActionSignature> *performActionDelegate = PerformActionMulticastDelegates.Find(ActionType);
            if(performActionDelegate != nullptr && performActionDelegate->IsValid())
            {
                performActionDelegate->Get()->AddUniqueDynamic(&part, &ABotPart::PerformAction);
            }
        }
    }
    part.OnSensorEventDelegate.AddUniqueDynamic(this, &ABot::NotifyOnSensorEvent);
}


ABot* ABot::AddPart(TSubclassOf<ABotPart> partClass, FName name)
{
    //Create component and attach it (physics can be moved after creation)
    UChildActorComponent* childActorComponent = NewObject<UChildActorComponent>(this, name);
    childActorComponent->SetChildActorClass(partClass);
    
    if(childActorComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale))
    {
        //Create Actor
        childActorComponent->CreateChildActor();
        ABotPart* part = static_cast<ABotPart *>(childActorComponent->GetChildActor());

        //Call Rebuild to connect plugs and sockets
        if(!Rebuild())
        {
            ERROR(FString::Printf(TEXT("Failed to Rebuild Bot %s when adding part %s"), *(this->GetFName().ToString()), *(name.ToString())));
            DestroyPart(*part);
        }
    }
    else
    {
        ERROR(FString::Printf(TEXT("Failed to attach part %s to Bot %s"), *(name.ToString()), *(this->GetFName().ToString())));
        childActorComponent = nullptr;

    }

    return this;
}

void ABot::ResetPart(ABotPart& part)
{
    TArray<USocketComponent*> sockets;
    part.GetSockets(sockets);
    
    for(USocketComponent* socket : sockets)
    {
        if(socket->GetPlug() != nullptr)
        {
            ResetPart(*static_cast<ABotPart*>(socket->GetPlug()->GetOwner()));
        }
    }
    
    part.Reset();
}

ABot* ABot::BreakSocket(FName name, bool all, bool recursive)
{
    if(RootPart != nullptr)
    {
        TArray<ABotPart*> parts;
        GetParts(parts);
        
        for(ABotPart *part : parts)
        {
            if(all)
            {
                TArray<USocketComponent*> sockets;
                part->GetSockets(sockets);
                
                for(USocketComponent* socket : sockets)
                {
                    if(socket->Name == name)
                    {
                        BreakSocket(*socket, false, recursive);
                    }
                }
            }
            else
            {
                USocketComponent *socket = part->GetSocket(name);
                if(socket != nullptr)
                {
                    BreakSocket(*socket, false, recursive);
                    return this;
                }
            }
        }
    }
    return this;
}

void ABot::BreakSocket(USocketComponent& socket, bool destroyChild, bool recursive)
{
    UPlugComponent *plug = socket.GetPlug();
    if(plug != nullptr)
    {

        ABotPart* plugPart = static_cast<ABotPart *>(plug->GetOwner());
        if(recursive)
        {
            TArray<USocketComponent*> childrenSockets;
            plugPart->GetSockets(childrenSockets);
            
            for(USocketComponent* childrenSocket : childrenSockets)
            {
                BreakSocket(*childrenSocket, destroyChild, recursive);
            }
        }
        
        //Disconnect
        DisconnectPart(*plugPart, &socket, plug);
        
        if(destroyChild)
        {
            DestroyPart(*plugPart);
        }
    }
}

void ABot::DisconnectPart(ABotPart &part, USocketComponent *socket, UPlugComponent *plug, bool isDead)
{
    if(socket != nullptr && plug != nullptr)
    {
        plug->Reset();
        socket->Reset();
    }
    
    if(isDead || !part.GetClass()->ImplementsInterface((UWireless::StaticClass())))
    {
        //Recursive
        TArray<USocketComponent*> sockets;
        part.GetSockets(sockets);
        
        for(USocketComponent* childSocket : sockets)
        {
            UPlugComponent *childPlug = childSocket->GetPlug();
            if(childPlug != nullptr)
            {
                DisconnectPart(*(static_cast<ABotPart *>(plug->GetOwner())), childSocket, childPlug, false);
            }
        }
        
        for (TPair<EActionType, TSharedPtr<FPerformActionSignature>> &performActionDelegatePair : PerformActionMulticastDelegates)
        {
            performActionDelegatePair.Value->RemoveAll(&part);
        }
        part.OnSensorEventDelegate.RemoveAll(this);
    }
}

ABot* ABot::RemovePart(FName name)
{
    ABotPart* part = GetPart(name);
    if(part != nullptr)
    {
        TArray<USocketComponent*> sockets;
        part->GetSockets(sockets);
        
        for(USocketComponent* socket : sockets)
        {
            BreakSocket(*socket, true, true);
        }
        
        TArray<UPlugComponent*> plugs;
        part->GetPlugs(plugs);
        
        for(UPlugComponent* plug : plugs)
        {
            if(plug->GetSocket() != nullptr)
            {
                BreakSocket(*(plug->GetSocket()), false, false);
            }
        }
        
        if(RootPart == part)
        {
            DisconnectPart(*RootPart, nullptr, nullptr, true);
            RootPart = nullptr;
        }
        DestroyPart(*part);
    }
    return this;
}

void ABot::DestroyPart(ABotPart &part)
{
    for (TPair<EActionType, TSharedPtr<FPerformActionSignature>> &performActionDelegatePair : PerformActionMulticastDelegates)
    {
        performActionDelegatePair.Value->RemoveAll(&part);
    }
    
    OnPartRemoved(&part);
    UChildActorComponent* component = part.GetParentComponent();
    component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    component->DestroyChildActor();
    component->DestroyComponent();
}

void ABot::PerformAction(EActionType actionType, FName actionName, UObject* actionData)
{
    TSharedPtr<FPerformActionSignature> *performActionDelegate = PerformActionMulticastDelegates.Find(actionType);
    if(performActionDelegate != nullptr && performActionDelegate->IsValid())
    {
        performActionDelegate->Get()->Broadcast(actionName, actionData);
    }
}

void ABot::NotifyOnPartAdded(ABotPart* part)
{
    OnPartAdded(part);
}


void ABot::NotifyOnPartRemoved(ABotPart* part)
{
    OnPartRemoved(part);
}

void ABot::NotifyOnSensorEvent(ESensorType sensorType, FName eventName, ABotPart* part, UObject* eventData)
{
    if(eventName == FName(TEXT("OnTakeDamage")))
    {
        UDamageEventData* DamageEventData = static_cast<UDamageEventData*>(eventData);
        TakeDamage(DamageEventData->DamageAmount,DamageEventData->DamageEvent,DamageEventData->EventInstigator,DamageEventData->DamageCauser);
    }
    else if(eventName == FName(TEXT("OnDie")))
    {
        DisconnectPart(*part, nullptr, nullptr, true);
        
        if(part->bBreakOnDeath)
        {
            TArray<USocketComponent*> sockets;
            part->GetSockets(sockets);
            
            for(USocketComponent* socket : sockets)
            {
                BreakSocket(*socket, false, false);
            }
            
            TArray<UPlugComponent*> plugs;
            part->GetPlugs(plugs);
            
            for(UPlugComponent* plug : plugs)
            {
                if(plug->GetSocket() != nullptr)
                {
                    BreakSocket(*(plug->GetSocket()), false, false);
                }
            }
        }
    }
    OnSensorEvent(sensorType, eventName, part, eventData);
}
