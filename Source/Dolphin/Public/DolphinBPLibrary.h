// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Bridge/DolphinManager.h"

#include "DolphinProtocol.h"
#include "DolphinListener.h"

#include "google/protobuf/message.h"

#include "Engine.h"
#include "Runtime/Core/Public/Containers/Queue.h"

#include <ctime>
#include <ratio>
#include <chrono>

#include "DolphinBPLibrary.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDolphinResponseDelegate, UDolphinResponse *, DolphinResponse);

using namespace std::chrono;
UCLASS()
class DOLPHIN_API UDolphinBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static char *mData;

	static TMap<uint16, FDolphinResponseDelegate> mDolphinResponseDelegates;
	static DolphinListener mDolphinListener;
	static TQueue<OriginalMessage *, EQueueMode::Spsc> mOriginalMessageQueue;

	//************************************
	// Method:    Dequeue
	// FullName:  UDolphinBPLibrary::Dequeue
	// Access:    public static 
	// Returns:   void
	// Qualifier: Dequeue OriginalMesage when Tick
	//************************************
	static void Dequeue();

public:
	static FString ClientID;
	static uint8 XKey[16];

	static high_resolution_clock::time_point t1;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Connect", Keywords = "Dolphin sample test testing"), Category = "Dolphin")
		static void Connect(const FString &IP, const FString &_ClientID);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disconnect", Keywords = "Dolphin sample test testing"), Category = "Dolphin")
		static void Disconnect();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RegisterResponseCallback", Keywords = "Dolphin sample test testing"), Category = "Dolphin")
		static void RegisterResponseCallback(uint8 ProtocolModule, uint8 SpecificProtocol, UObject * object, const FName& FunctionName);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UnregisterResponseCallback", Keywords = "Dolphin sample test testing"), Category = "Dolphin")
		static void UnregisterResponseCallback(uint8 ProtocolModule, uint8 SpecificProtocol, UObject * object, const FName& FunctionName);

	//************************************
	// Method:    Tick
	// FullName:  UDolphinBPLibrary::Tick
	// Access:    public static 
	// Returns:   void
	// Qualifier: Tick
	//************************************
	static void Tick();

	//************************************
	// Method:    Dispatch
	// FullName:  UDolphinBPLibrary::Dispatch
	// Access:    public static 
	// Returns:   void
	// Qualifier: Dispatch Specific DolphinResponse to Listener
	// Parameter: int protocolNo
	// Parameter: UDolphinResponse * DolphinResponse
	//************************************
	static void Dispatch(int protocolNo, UDolphinResponse *DolphinResponse);

	//************************************
	// Method:    Enqueue
	// FullName:  UDolphinBPLibrary::Enqueue
	// Access:    public static 
	// Returns:   void
	// Qualifier: Enqueue OriginalMesage when message received
	// Parameter: OriginalMessage * msg
	//************************************
	static void Enqueue(OriginalMessage * msg);
};