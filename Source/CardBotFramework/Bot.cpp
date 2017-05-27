#include "CardBotFramework.h"
#include "Bot.h"

ABot::ABot()
{
    RootPart = nullptr;
    PrimaryActorTick.bCanEverTick = true;
}

void ABot::Reset()
{
    Super::Reset();
    if(RootPart != nullptr)
    {
        DisassemblePart(*RootPart);
    }
    RootPart = nullptr;
    Rebuild();
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
        if(((UChildActorComponent*)childActorComponent)->GetChildActor()->IsA(ABotPart::StaticClass()))
        {
            parts.Add((ABotPart*)((UChildActorComponent*)childActorComponent)->GetChildActor());
        }
    }
}

ABotPart* ABot::GetPart(FName name) const
{
    TArray<UActorComponent*> childActorComponents = GetComponentsByClass(UChildActorComponent::StaticClass());
    for(UActorComponent* childActorComponent : childActorComponents)
    {
        if(((UChildActorComponent*)childActorComponent)->GetChildActor()->IsA(ABotPart::StaticClass())
           &&childActorComponent->GetFName() == name)
        {
            return (ABotPart*)((UChildActorComponent*)childActorComponent)->GetChildActor();
        }
    }
    return nullptr;
}

bool ABot::Rebuild()
{
    //Optim
    TArray<ABotPart*> parts;
    GetParts(parts);
    
    //First if RootPart is not set, seek one
    if(RootPart == nullptr)
    {
        for(ABotPart* part : parts)
        {
            if(RootPart == nullptr && part->HasSockets() && !part->HasPlugs())
            {
                RootPart = part;
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

void ABot::DestroyPart(ABotPart &part)
{
    UChildActorComponent* component = part.GetParentComponent();
    component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    component->DestroyChildActor();
    component->DestroyComponent();
    OnPartRemoved(&part);
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
                    
                    //Connect Joint
                    UActorComponent* socketBody = part.GetComponentByName(socket->ComponentName);
                    UActorComponent* plugBody = plugPart->GetComponentByName(plug->ComponentName);
                    if(socketBody != nullptr && plugBody != nullptr
                       && socketBody->IsA(UPrimitiveComponent::StaticClass())
                       && plugBody->IsA(UPrimitiveComponent::StaticClass()))
                    {
                        plug->SetConstrainedComponents((UPrimitiveComponent*)socketBody, NAME_None, (UPrimitiveComponent*)plugBody, NAME_None);
                        socket->SetPlug(plug);
                        plug->SetSocket(socket);
                        part.OnSocketConnected(socket);
                        plugPart->OnPlugConnected(plug);
                        break;
                    }
                    else{
                        ERROR(FString::Printf(TEXT("Bot %s failed to connect socket %s"), *(this->GetFName().ToString()), *(socket->Name.ToString())));
                        return false;
                    }
                }
            }
        }
        if(socket->GetPlug() != nullptr && !AssemblePart(*(ABotPart*)(socket->GetPlug()->GetOwner()), parts))
        {
            return false;
        }
    }
    return true;
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
        ABotPart* part = (ABotPart*)childActorComponent->GetChildActor();
        
        //No RootPart --> Added part must be of RootPart type (no plugs)
        if(RootPart == nullptr)
        {
            if(part->HasSockets() && !part->HasPlugs())
            {
                RootPart = part;
            }
            else
            {
                ERROR(FString::Printf(TEXT("Bot %s failed to add root part %s which is not adapted (must have sockets and no plugs)"), *(this->GetFName().ToString()), *(name.ToString())));
                DestroyPart(*part);
                return this;
            }
        }
        
        //Call Rebuild to connect plugs and sockets
        if(Rebuild())
        {
            childActorComponent->RegisterComponent();
            childActorComponent->InitializeComponent();
            OnPartAdded(part);
        }
        else
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

void ABot::DisassemblePart(ABotPart& part)
{
    TArray<USocketComponent*> sockets;
    part.GetSockets(sockets);
    
    for(USocketComponent* socket : sockets)
    {
        BreakSocket(*socket, false, true);
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
            RootPart = nullptr;
        }
        DestroyPart(*part);
    }
    return this;
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

        ABotPart* plugPart = (ABotPart*)plug->GetOwner();
        ABotPart* socketPart = (ABotPart*)socket.GetOwner();
        if(recursive)
        {
            TArray<USocketComponent*> childrenSockets;
            plugPart->GetSockets(childrenSockets);
            
            for(USocketComponent* childrenSocket : childrenSockets)
            {
                BreakSocket(*childrenSocket, destroyChild);
            }
        }
        plug->BreakConstraint();
        plug->TermComponentConstraint();
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
