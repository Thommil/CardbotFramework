#include "CardBotFramework.h"
#include "Bot.h"

ABot::ABot()
{
    RootPart = NULL;
    PrimaryActorTick.bCanEverTick = true;
}

void ABot::Reset()
{
    Super::Reset();
    if(RootPart != NULL)
    {
        DisassemblePart(*RootPart);
    }
    RootPart = NULL;
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
    return NULL;
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
    return NULL;
}

bool ABot::Rebuild()
{
    //Optim
    TArray<ABotPart*> parts;
    GetParts(parts);
    
    //First if RootPart is not set, seek one
    if(RootPart == NULL)
    {
        for(ABotPart* part : parts)
        {
            if(RootPart == NULL && part->HasSockets() && !part->HasPlugs())
            {
                RootPart = part;
            }
        }
    }
    
    if(RootPart != NULL)
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
}

bool ABot::AssemblePart(ABotPart& part, TArray<ABotPart*> *parts)
{
    //Optim
    if(parts == NULL)
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
        if(socket->GetPlug() == NULL)
        {
            for(ABotPart* plugPart : *parts)
            {
                UPlugComponent* plug = plugPart->GetPlug(socket->Name);
                if(plug != NULL && plug->GetSocket() == NULL)
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
                    if(socketBody != NULL && plugBody != NULL
                       && socketBody->IsA(UPrimitiveComponent::StaticClass())
                       && plugBody->IsA(UPrimitiveComponent::StaticClass()))
                    {
                        plug->SetConstrainedComponents((UPrimitiveComponent*)socketBody, NAME_None, (UPrimitiveComponent*)plugBody, NAME_None);
                        socket->SetPlug(plug);
                        plug->SetSocket(socket);
                        break;
                    }
                    else{
                        ERROR(FString::Printf(TEXT("Bot %s failed to connect socket %s"), *(this->GetFName().ToString()), *(socket->Name.ToString())));
                        return false;
                    }
                }
            }
        }
        if(socket->GetPlug() != NULL && !AssemblePart(*(ABotPart*)(socket->GetPlug()->GetOwner()), parts))
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
        if(RootPart == NULL)
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
        childActorComponent = NULL;

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
    if(part != NULL)
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
            if(plug->GetSocket() != NULL)
            {
                BreakSocket(*(plug->GetSocket()), false, false);
            }
        }
        
        if(RootPart == part)
        {
            RootPart = NULL;
        }
        DestroyPart(*part);
    }
    return this;
}

ABot* ABot::BreakSocket(FName name, bool all, bool recursive)
{
    if(RootPart != NULL)
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
                if(socket != NULL)
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
    if(plug != NULL)
    {

        ABotPart* plugPart = (ABotPart*)plug->GetOwner();
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
        
        if(destroyChild)
        {
            DestroyPart(*plugPart);
        }
    }
}
