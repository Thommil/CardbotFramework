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
        RootPart = NULL;
        Rebuild();
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
                childActorComponent->DestroyChildActor();
                RemoveOwnedComponent(childActorComponent);
                childActorComponent = NULL;
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
            childActorComponent->DestroyChildActor();
            RemoveOwnedComponent(childActorComponent);
            childActorComponent = NULL;
        }
    }
    else
    {
        ERROR(FString::Printf(TEXT("Failed to attach part %s to Bot %s"), *(name.ToString()), *(this->GetFName().ToString())));
        childActorComponent = NULL;

    }

    return this;
}

void ABot::DisassemblePart(ABotPart& part, TArray<ABotPart*> *parts)
{
    
}


ABot* ABot::RemovePart(FName name)
{
    return this;
}

void ABot::BreakSocket(FName name, bool all, bool recursive)
{
    
}




//ABot* ABot::AddPart(TSubclassOf<ABotPart> partClass, FName name)
//{

//    //Create component and attach it (physics can be moved after creation)
//    UChildActorComponent* childActorComponent = NewObject<UChildActorComponent>(this, name);
//    childActorComponent->SetChildActorClass(partClass);
//    
//    if(childActorComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale))
//    {
//        //Create Actor
//        childActorComponent->CreateChildActor();
//        ABotPart* part = (ABotPart*)childActorComponent->GetChildActor();
//        
//        //Root part
//        if(this->Parts.Num() == 0)
//        {
//            if(part->HasSockets() && !part->HasPlugs())
//            {
//                part->bAssembled = true;
//                this->Parts.Add(part);
//                childActorComponent->RegisterComponent();
//                childActorComponent->InitializeComponent();
//            }
//            else
//            {
//                UE_LOG(LogTemp, Error, TEXT("Bot %s failed to add root part %s which is not adapted (must have sockets and no plugs)"), *(this->GetFName().ToString()), *(name.ToString()));
//                part->bAssembled = false;
//                childActorComponent->DestroyChildActor();
//                this->RemoveOwnedComponent(childActorComponent);
//                childActorComponent = NULL;
//            }
//        }
//        //Other parts
//        else
//        {
//            if(part->HasPlugs())
//            {
//                TArray<UPlugComponent*> plugs;
//                part.GetPlugs(plugs);
//                
//                for(UPlugComponent* plug : plugs)
//                    //Search among Parts the corresponding Socket for location
//                    for(ABotPart* socketPart : this->Parts)
//                    {
//                        if(socketPart->bAssembled)
//                        {
//                            USocketComponent* socket = socketPart->GetSocket(plug.Name);
//                            if(socket != NULL)
//                            {
//                                UActorComponent* plugBody = part.GetComponentByName(plug->ComponentName);
//                                UActorComponent* socketBody = socketPart->GetComponentByName(socket->ComponentName);
//                                if(socketBody != NULL && plugBody != NULL
//                                   && socketBody->IsA(UPrimitiveComponent::StaticClass())
//                                   && plugBody->IsA(UPrimitiveComponent::StaticClass()))
//                                {
//                            
//                                    part->bAssembled = true;
//                                    socketPart->bAssembled = true;
//                                }
//                                else{
//                                    UE_LOG(LogTemp, Error, TEXT("Misconfigured Bot %s for socket %s"), *(this->GetFName().ToString()), *(socket->Name.ToString()));
//                                }
//                            //Connect
//                            /*UActorComponent* socketBody = part.GetComponentByName(socket->ComponentName);
//                             UActorComponent* plugBody = plugPart->GetComponentByName(plug->ComponentName);
//                             if(socketBody != NULL && plugBody != NULL
//                             && socketBody->IsA(UPrimitiveComponent::StaticClass())
//                             && plugBody->IsA(UPrimitiveComponent::StaticClass()))
//                             {
//                             plug->SetConstrainedComponents((UPrimitiveComponent*)plugBody, NAME_None, (UPrimitiveComponent*)socketBody, NAME_None);
//                             plug->bConnected = true;
//                             socket->bConnected = true;
//                             this->AssemblePart(*plugPart);
//                             }
//                             else{
//                             UE_LOG(LogTemp, Error, TEXT("Misconfigured Bot %s for socket %s"), *(this->GetFName().ToString()), *(socket->Name.ToString()));
//                             }*/
//                            }
//                        }
//                    }
//                }
//            
//                if(!part->bAssembled)
//                {
//                    UE_LOG(LogTemp, Warning, TEXT("Bot %s can't find plug/socket assembly for part %"), *(this->GetFName().ToString()), *(name.ToString()));
//                }
//            }
//            else
//            {
//                UE_LOG(LogTemp, Error, TEXT("Bot %s failed to add part %s which is not adapted (must have plugs)"), *(this->GetFName().ToString()), *(name.ToString()));
//                childActorComponent->DestroyChildActor();
//                this->RemoveOwnedComponent(childActorComponent);
//                childActorComponent = NULL;
//            }
//        }
//        
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT("Failed to attach part %s to Bot %s"), *(name.ToString()), *(this->GetFName().ToString()));
//        childActorComponent = NULL;
//    }
//    
//    return this;
//    
//    //Create component
//    //UChildActorComponent* childActorComponent = NewObject<UChildActorComponent>(this, name);
//    //childActorComponent->SetChildActorClass(partClass);
//    //childActorComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
//    //childActorComponent->CreateChildActor();
//    
//    
//    /*//KO -> Physics too soon
//    
//    bool bSuccess = false;
//    
//    //Create component
//    UChildActorComponent* childActorComponent = NewObject<UChildActorComponent>(this, name);
//    childActorComponent->SetChildActorClass(partClass);
//    childActorComponent->CreateChildActor();
//    ABotPart* part = (ABotPart*)childActorComponent->GetChildActor();
//    
//    //Root part
//    if(this->Parts.Num() == 0)
//    {
//        if(part->HasSockets() && !part->HasPlugs())
//        {
//            bSuccess = childActorComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
//        }
//        else
//        {
//            UE_LOG(LogTemp, Error, TEXT("Bot %s failed to add root part %s which is not adapted (must have sockets and no plugs)"), *(this->GetFName().ToString()), *(name.ToString()));
//        }
//    }
//    //Child parts
//    else
//    {
//        
//    }
//    
//    if(bSuccess)
//    {
//        this->Parts.Add(part);
//        childActorComponent->RegisterComponent();
//        childActorComponent->InitializeComponent();
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT("Failed to attach part %s to Bot %s"), *(name.ToString()), *(this->GetFName().ToString()));
//        childActorComponent->DestroyChildActor();
//        this->RemoveOwnedComponent(childActorComponent);
//        childActorComponent = NULL;
//    }
//    */
    
//    return this;
//}

//void ABot::PreInitializeComponents()
//{
//    Super::PreInitializeComponents();

//    //If edited in BluePrint, parse Parts
//    if(this->Parts.Num() == 0)
//    {
//        TArray <AActor*> childActors;
//        this->GetAllChildActors(childActors, false);
//
//        //Search root
//        for(AActor* childActor : childActors)
//        {
//            if(!((ABotPart*)childActor)->HasPlugs()){
//                if(this->Parts.Num() == 0)
//                {
//                    this->Parts.Add((ABotPart*)childActor);
//                }
//                else
//                {
//                    UE_LOG(LogTemp, Error, TEXT("Bot %s has several root Parts"), *(this->GetFName().ToString()));
//                    return;
//                }
//            }
//        }
//
//        //Add other parts
//        if(this->Parts.Num() > 0)
//        {
//            for(AActor* childActor : childActors)
//            {
//                if(((ABotPart*)childActor)->HasPlugs()){
//                   this->Parts.Add((ABotPart*)childActor);
//                }
//            }
//        }
//    }
//
//    //Assemble parts
//    if(this->Parts.Num() > 0)
//    {
//        this->AssemblePart(*(this->Parts[0]));
//    }
//}

//void ABot::AssemblePart(ABotPart& part)
//{
//    //Only go from sockets to plugs
//    if(part.HasSockets())
//    {
//        TArray<USocketComponent*> sockets;
//        part.GetSockets(sockets);
//
//        //Parse sockets
//        for(USocketComponent* socket : sockets)
//        {
//            if(!socket->bConnected)
//            {
//                //Search among Parts the corresponding Plug
//                for(ABotPart* plugPart : this->Parts)
//                {
//                    UPlugComponent* plug = plugPart->GetPlug(socket->Name);
//                    if(plug != NULL && !plug->bConnected)
//                    {
//                        //Connect
//                        UActorComponent* socketBody = part.GetComponentByName(socket->ComponentName);
//                        UActorComponent* plugBody = plugPart->GetComponentByName(plug->ComponentName);
//                        if(socketBody != NULL && plugBody != NULL
//                           && socketBody->IsA(UPrimitiveComponent::StaticClass())
//                           && plugBody->IsA(UPrimitiveComponent::StaticClass()))
//                        {
//                            plug->SetConstrainedComponents((UPrimitiveComponent*)plugBody, NAME_None, (UPrimitiveComponent*)socketBody, NAME_None);
//                            plug->bConnected = true;
//                            socket->bConnected = true;
//                            this->AssemblePart(*plugPart);
//                        }
//                        else{
//                            UE_LOG(LogTemp, Error, TEXT("Misconfigured Bot %s for socket %s"), *(this->GetFName().ToString()), *(socket->Name.ToString()));
//                        }
//                    }
//                }
//            }
//        }
//    }
//}



//void ABot::DisassemblePart(ABotPart& part, bool recursive)
//{
//    //Only go from sockets to plugs
//    if(part.HasSockets())
//    {
//        TArray<USocketComponent*> sockets;
//        part.GetSockets(sockets);
//        
//        //Parse sockets
//        for(USocketComponent* socket : sockets)
//        {
//            if(socket->bConnected)
//            {
//                //Search among Parts the corresponding Plug
//                for(ABotPart* plugPart : this->Parts)
//                {
//                    UPlugComponent* plug = plugPart->GetPlug(socket->Name);
//                    if(plug != NULL && !plug->bConnected)
//                    {
//                        //Disconnect
//                        plug->BreakConstraint();
//                        plug->bConnected = false;
//                        socket->bConnected = false;
//                        if(recursive)
//                        {
//                            this->DisassemblePart(*plugPart);
//                        }
//                    }
//                }
//            }
//        }
//
//    }
//}

//void ABot::BreakSocket(FName name, bool all, bool recursive)
//{
//   for(ABotPart* part : this->Parts)
//   {
//       if(part->HasSockets())
//       {
//           TArray<USocketComponent*> sockets;
//           part->GetSockets(sockets);
//           
//           //Parse sockets
//           for(USocketComponent* socket : sockets)
//           {
//               if(socket->Name == name && socket->bConnected)
//               {
//                   //Search among Parts the corresponding Plug
//                   for(ABotPart* plugPart : this->Parts)
//                   {
//                       UPlugComponent* plug = plugPart->GetPlug(socket->Name);
//                       if(plug != NULL && plug->bConnected)
//                       {
//                           plug->BreakConstraint();
//                           plug->bConnected = false;
//                           socket->bConnected = false;
//                           if(recursive)
//                           {
//                               this->DisassemblePart(*plugPart);
//                           }
//                           if(!all){
//                               return;
//                           }
//                       }
//                   }
//               }
//               
//           }
//           
//       }
//   }
//}
