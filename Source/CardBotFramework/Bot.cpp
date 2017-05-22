#include "CardBotFramework.h"
#include "PlugComponent.h"
#include "SocketComponent.h"
#include "Bot.h"

ABot::ABot()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABot::PreInitializeComponents()
{
    Super::PreInitializeComponents();
    
    //If edited in BluePrint, parse Parts
    if(this->Parts.Num() == 0)
    {
        TArray <AActor*> childActors;
        this->GetAllChildActors(childActors, false);
        
        //Search root
        for(AActor* childActor : childActors)
        {
            if(!((ABotPart*)childActor)->HasPlugs()){
                if(this->Parts.Num() == 0)
                {
                    this->Parts.Add((ABotPart*)childActor);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Bot %s has several root Parts"), *(this->GetFName().ToString()));
                    return;
                }
            }
        }
        
        //Add other parts
        if(this->Parts.Num() > 0)
        {
            for(AActor* childActor : childActors)
            {
                if(((ABotPart*)childActor)->HasPlugs()){
                   this->Parts.Add((ABotPart*)childActor);
                }
            }
        }
    }
        
    //Assemble parts
    if(this->Parts.Num() > 0)
    {
        this->AssemblePart(*(this->Parts[0]));
    }
}

UActorComponent* ABot::GetComponentByName(FName name)
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

ABot* ABot::AddPart(TSubclassOf<ABotPart> partClass, FName name)
{
    bool bSuccess = false;
    
    //Create component
    UChildActorComponent* childActorComponent = NewObject<UChildActorComponent>(this, name);
    childActorComponent->SetChildActorClass(partClass);
    childActorComponent->CreateChildActor();
    ABotPart* part = (ABotPart*)childActorComponent->GetChildActor();
    
    //Root part
    if(this->Parts.Num() == 0)
    {
        if(part->HasSockets() && !part->HasPlugs())
        {
            bSuccess = childActorComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Bot %s failed to add root part %s which is not adapted (must have sockets and no plugs)"), *(this->GetFName().ToString()), *(name.ToString()));
        }
    }
    //Child parts
    else
    {
        
    }
    
    if(bSuccess)
    {
        this->Parts.Add(part);
        childActorComponent->RegisterComponent();
        childActorComponent->InitializeComponent();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to attach part %s to Bot %s"), *(name.ToString()), *(this->GetFName().ToString()));
        childActorComponent->DestroyChildActor();
        this->RemoveOwnedComponent(childActorComponent);
        childActorComponent = NULL;
    }
        
    return this;
}

void ABot::AssemblePart(ABotPart& part)
{
    //Only go from sockets to plugs
    if(part.HasSockets())
    {
        TArray<USocketComponent*> sockets;
        part.GetSockets(sockets);

        //Parse sockets
        for(USocketComponent* socket : sockets)
        {
            if(!socket->bConnected)
            {
                //Search among Parts the corresponding Plug
                for(ABotPart* plugPart : this->Parts)
                {
                    UPlugComponent* plug = plugPart->GetPlug(socket->Name);
                    if(plug != NULL && !plug->bConnected)
                    {
                        //Connect
                        UActorComponent* socketBody = part.GetComponentByName(socket->ComponentName);
                        UActorComponent* plugBody = plugPart->GetComponentByName(plug->ComponentName);
                        if(socketBody != NULL && plugBody != NULL
                           && socketBody->IsA(UPrimitiveComponent::StaticClass())
                           && plugBody->IsA(UPrimitiveComponent::StaticClass()))
                        {
                            plug->SetConstrainedComponents((UPrimitiveComponent*)plugBody, NAME_None, (UPrimitiveComponent*)socketBody, NAME_None);
                            plug->bConnected = true;
                            socket->bConnected = true;
                            this->AssemblePart(*plugPart);
                        }
                        else{
                            UE_LOG(LogTemp, Error, TEXT("Misconfigured Bot %s for socket %s"), *(this->GetFName().ToString()), *(socket->Name.ToString()));
                        }
                    }
                }
            }
        }
    }
}

ABot* ABot::RemovePart(ABotPart* part, bool all)
{
    return this;
}


void ABot::DisassemblePart(ABotPart& part, bool recursive)
{
    //Only go from sockets to plugs
    if(part.HasSockets())
    {
        TArray<USocketComponent*> sockets;
        part.GetSockets(sockets);
        
        //Parse sockets
        for(USocketComponent* socket : sockets)
        {
            if(socket->bConnected)
            {
                //Search among Parts the corresponding Plug
                for(ABotPart* plugPart : this->Parts)
                {
                    UPlugComponent* plug = plugPart->GetPlug(socket->Name);
                    if(plug != NULL && !plug->bConnected)
                    {
                        //Disconnect
                        plug->BreakConstraint();
                        plug->bConnected = false;
                        socket->bConnected = false;
                        if(recursive)
                        {
                            this->DisassemblePart(*plugPart);
                        }
                    }
                }
            }
        }

    }
}

void ABot::BreakSocket(FName name, bool all, bool recursive)
{
   for(ABotPart* part : this->Parts)
   {
       if(part->HasSockets())
       {
           TArray<USocketComponent*> sockets;
           part->GetSockets(sockets);
           
           //Parse sockets
           for(USocketComponent* socket : sockets)
           {
               if(socket->Name == name && socket->bConnected)
               {
                   //Search among Parts the corresponding Plug
                   for(ABotPart* plugPart : this->Parts)
                   {
                       UPlugComponent* plug = plugPart->GetPlug(socket->Name);
                       if(plug != NULL && plug->bConnected)
                       {
                           plug->BreakConstraint();
                           plug->bConnected = false;
                           socket->bConnected = false;
                           if(recursive)
                           {
                               this->DisassemblePart(*plugPart);
                           }
                           if(!all){
                               return;
                           }
                       }
                   }
               }
               
           }
           
       }
   }
}
