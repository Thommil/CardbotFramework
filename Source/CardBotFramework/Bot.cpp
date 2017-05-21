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
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not root part found on Bot %s"), *(this->GetFName().ToString()));
        return;
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

void ABot::AddPart(ABotPart* part)
{
    //Set location & rotation
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

void ABot::RemovePart(FName name, bool all)
{
    
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
