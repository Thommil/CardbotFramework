#include "CardBotFramework.h"
#include "Wireless.h"
#include "Bot.h"

//Optim
UEnum* EActionTypeEnum = nullptr;

ABot::ABot()
{
    RootPart = nullptr;
    PrimaryActorTick.bCanEverTick = true;

    if(EActionTypeEnum == nullptr)
    {
       EActionTypeEnum = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("EActionType"), true);
    }
    
    for(int i=0; i < (EActionTypeEnum->NumEnums() - 1); i++)
    {
        TSharedPtr<FPerformActionSignature> performActionDelegate(new FPerformActionSignature());
        PerformActionMulticastDelegates.Add(static_cast<EActionType>(EActionTypeEnum->GetValueByIndex(i)), performActionDelegate);
    }
}

void ABot::PreInitializeComponents()
{
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
                    OnPartAdded(RootPart);
                }
            }
        }
    
        if(RootPart != nullptr)
        {
            return AssemblePart(*RootPart, &parts);
        }
        else
        {
            ERROR(FString::Printf(TEXT("Bot %s failed to Rebuild, no root part found (sockets and no plugs)"), *(this->GetFName().ToString())));
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
                    socket->SetPlug(plug);
                    plug->SetSocket(socket);
                    part.OnSocketConnected(socket);
                    plugPart->OnPlugConnected(plug);
                    
                    ConnectPart(*plugPart);
                    OnPartAdded(&part);
                    
                    break;
                }
            }
        }
        
        if(socket->GetPlug() != nullptr && !AssemblePart(*static_cast<ABotPart *>(socket->GetPlug()->GetOwner()), parts))
        {
            return false;
        }
    }
 
    return true;
}

void ABot::ConnectPart(ABotPart &part)
{
    for(int i=0; i < (EActionTypeEnum->NumEnums() - 1); i++)
    {
        EActionType ActionType = static_cast<EActionType>(EActionTypeEnum->GetValueByIndex(i));
        if (((1 << static_cast<int32>(ActionType)) & part.ActionCapabilites) != 0)
        {
            TSharedPtr<FPerformActionSignature> *performActionDelegate = PerformActionMulticastDelegates.Find(ActionType);
            if(performActionDelegate != nullptr && performActionDelegate->IsValid())
            {
                performActionDelegate->Get()->AddUniqueDynamic(&part, &ABotPart::OnPerformAction);
            }
        }
    }
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
        
        //No RootPart --> Added part must be of RootPart type (no plugs)
        if(RootPart == nullptr)
        {
            if(part->HasSockets() && !part->HasPlugs())
            {
                RootPart = part;
                ConnectPart(*RootPart);
            }
            else
            {
                ERROR(FString::Printf(TEXT("Bot %s failed to add root part %s which is not adapted (must have sockets and no plugs)"), *(this->GetFName().ToString()), *(name.ToString())));
                DestroyPart(*part);
                return this;
            }
        }
        
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
        ABotPart* socketPart = static_cast<ABotPart *>(socket.GetOwner());
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
        DisconnectPart(*plugPart);
        
        plug->Reset();
        socket.Reset();
        socketPart->OnSocketBroken(&socket);
        plugPart->OnPlugBroken(plug);
        
        if(destroyChild)
        {
            DestroyPart(*plugPart);
        }
    }
}

void ABot::DisconnectPart(ABotPart &part, bool force)
{
    if(force || !part.GetClass()->ImplementsInterface((UWireless::StaticClass())))
    {
        for (TPair<EActionType, TSharedPtr<FPerformActionSignature>> &performActionDelegatePair : PerformActionMulticastDelegates)
        {
            performActionDelegatePair.Value->RemoveAll(&part);
        }
        
        //Apply on children
        TArray<USocketComponent*> sockets;
        part.GetSockets(sockets);
        
        for(USocketComponent* socket : sockets)
        {
            UPlugComponent *plug = socket->GetPlug();
            if(plug != nullptr)
            {
                DisconnectPart(*(static_cast<ABotPart *>(plug->GetOwner())), true);
            }
        }
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
            DisconnectPart(*RootPart, true);
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

void ABot::PerformAction(EActionType actionType, int32 actionFlags, UObject* actionData)
{
    TSharedPtr<FPerformActionSignature> *performActionDelegate = PerformActionMulticastDelegates.Find(actionType);
    if(performActionDelegate != nullptr && performActionDelegate->IsValid())
    {
        performActionDelegate->Get()->Broadcast(actionFlags, actionData);
    }
}
