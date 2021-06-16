// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DolphinBPLibrary.h"
#include "Dolphin.h"

DolphinListener UDolphinBPLibrary::mDolphinListener;
TMap<uint16, FDolphinResponseDelegate> UDolphinBPLibrary::mDolphinResponseDelegates;
TMap<int32, FDolphinResponseDelegate> UDolphinBPLibrary::SpecificResponseDelegates;

TQueue<OriginalMessage *, EQueueMode::Spsc> UDolphinBPLibrary::mOriginalMessageQueue;
EConnectStatus UDolphinBPLibrary::CurrentConnectStatus;
EConnectStatus  UDolphinBPLibrary::ConnectStatus;

FDolphinConnectDelegate UDolphinBPLibrary::ConnectStatusCallback;

char * UDolphinBPLibrary::mData = nullptr;

FString UDolphinBPLibrary::ClientID;

uint8 UDolphinBPLibrary::XKey[];

DEFINE_LOG_CATEGORY(LogDolphinBPLibrary);

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
	CurrentConnectStatus = EConnectStatus::disconnect;
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

void UDolphinBPLibrary::RegisterOnConnectStatusCallback(const FDolphinConnectDelegate& Callback)
{
	ConnectStatusCallback = Callback;
	ConnectStatusCallback.ExecuteIfBound(CurrentConnectStatus);
}

EConnectStatus UDolphinBPLibrary::getConnectStatus()
{
	return CurrentConnectStatus;
}

void UDolphinBPLibrary::OnConnectSuccess()
{

	CurrentConnectStatus = EConnectStatus::success;
}

void UDolphinBPLibrary::OnConnectFailure()
{
	CurrentConnectStatus = EConnectStatus::failure;
}

void UDolphinBPLibrary::OnConnectionReady()
{
	CurrentConnectStatus = EConnectStatus::ready;
}

void UDolphinBPLibrary::OnConnectionLost()
{
	CurrentConnectStatus = EConnectStatus::lost;
}

void UDolphinBPLibrary::Tick()
{
	if (CurrentConnectStatus != ConnectStatus) {
		ConnectStatus = CurrentConnectStatus;
		ConnectStatusCallback.ExecuteIfBound(ConnectStatus);
	}

	Dequeue();
}

void UDolphinBPLibrary::Dispatch(int protocolNo, UDolphinResponse *DolphinResponse, int msgid)
{
	if (SpecificResponseDelegates.Contains(msgid))
	{
		UE_LOG(LogDolphinBPLibrary, Warning, TEXT("Response Message ID %d"), msgid);
		SpecificResponseDelegates[msgid].Broadcast(DolphinResponse);
		SpecificResponseDelegates.Remove(msgid);
	}
	else if (mDolphinResponseDelegates.Contains(protocolNo))
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
		if (DolphinResponseClass && DolphinResponseClass->IsChildOf(UDolphinResponse::StaticClass())) {
			UDolphinResponse *DolphinResponse = NewObject<UDolphinResponse>(GetTransientPackage(), DolphinResponseClass);
			if (DolphinResponse) {
				DolphinResponse->Dispatch(msg);
			}
		}
		delete msg;
	}
}
