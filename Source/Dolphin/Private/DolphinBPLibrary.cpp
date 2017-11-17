// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DolphinBPLibrary.h"
#include "Dolphin.h"

DolphinListener UDolphinBPLibrary::mDolphinListener;
TMap<uint16, FDolphinResponseDelegate> UDolphinBPLibrary::mDolphinResponseDelegates;
TQueue<OriginalMessage *, EQueueMode::Spsc> UDolphinBPLibrary::mOriginalMessageQueue;
char * UDolphinBPLibrary::mData = nullptr;

FString UDolphinBPLibrary::ClientID;

uint8 UDolphinBPLibrary::XKey[];

std::chrono::high_resolution_clock::time_point UDolphinBPLibrary::t1;

void UDolphinBPLibrary::Connect(const FString &IP, const FString &_ClientID)
{
	ClientID = _ClientID;

	std::string ip = TCHAR_TO_UTF8(*IP);
	std::string clientid = TCHAR_TO_UTF8(*_ClientID);

	Dolphin::DolphinManager::GetInstance()->AddDolphinListener(&mDolphinListener);
	Dolphin::DolphinManager::GetInstance()->Connect(ip.c_str(), clientid.c_str());
}

void UDolphinBPLibrary::Disconnect()
{
	Dolphin::DolphinManager::GetInstance()->Disconnect();
	Dolphin::DolphinManager::GetInstance()->RemoveDolphinListener(&mDolphinListener);
}

void UDolphinBPLibrary::RegisterResponseCallback(uint8 ProtocolModule, uint8 SpecificProtocol, UObject * object, const FName& FunctionName)
{
	uint16 protocolNo = (uint8)ProtocolModule << 8 | SpecificProtocol;

	if (!mDolphinResponseDelegates.Contains(protocolNo))
	{
		mDolphinResponseDelegates.Add(protocolNo);
	}

	if (mDolphinResponseDelegates.Contains(protocolNo))
	{
		FScriptDelegate Delegate;
		Delegate.BindUFunction(object, FunctionName);

		if (!mDolphinResponseDelegates[protocolNo].Contains(Delegate)) {
			mDolphinResponseDelegates[protocolNo].Add(Delegate);
		}
	}
}

void UDolphinBPLibrary::UnregisterResponseCallback(uint8 ProtocolModule, uint8 SpecificProtocol, UObject * object, const FName& FunctionName)
{
	uint16 protocolNo = (uint8)ProtocolModule << 8 | SpecificProtocol;
	if (mDolphinResponseDelegates.Contains(protocolNo))
	{
		FScriptDelegate Delegate;
		Delegate.BindUFunction(object, FunctionName);
		if (mDolphinResponseDelegates[protocolNo].Contains(Delegate)) {
			mDolphinResponseDelegates[protocolNo].Remove(Delegate);
		}
	}
}

void UDolphinBPLibrary::Tick()
{
	Dequeue();
}

void UDolphinBPLibrary::Dispatch(int protocolNo, UDolphinResponse *DolphinResponse)
{
	if (mDolphinResponseDelegates.Contains(protocolNo))
	{
		mDolphinResponseDelegates[protocolNo].Broadcast(DolphinResponse);
	}
}

void UDolphinBPLibrary::Enqueue(OriginalMessage * msg)
{
	mOriginalMessageQueue.Enqueue(msg);
}

void UDolphinBPLibrary::Dequeue()
{
	OriginalMessage * msg = nullptr;
	if (mOriginalMessageQueue.Dequeue(msg))
	{
		UClass* DolphinResponseClass = FindObject<UClass>(ANY_PACKAGE, *msg->ResponseClass);
		UDolphinResponse *DolphinResponse = NewObject<UDolphinResponse>(GetTransientPackage(), DolphinResponseClass);
		DolphinResponse->Dispatch(msg);
		delete msg;
		duration<double, std::milli> time_span = high_resolution_clock::now() - UDolphinBPLibrary::t1;
		UE_LOG(LogTemp, Warning, TEXT("UserLoginResponse_Dispatch_Elapse %f milliseconds"), time_span.count());
	}
}
