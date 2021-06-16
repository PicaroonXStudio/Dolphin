// Fill out your copyright notice in the Description page of Project Settings.

#include "DolphinListener.h"
#include "DolphinBPLibrary.h"
#include "Dolphin/Crypto/AES.h"


DEFINE_LOG_CATEGORY(LogDolphinListener);

void DolphinListener::ConnectFailure(const mqtt::token & tok)
{
	UDolphinBPLibrary::OnConnectFailure();
	UE_LOG(LogDolphinListener, Warning, TEXT("OnConnectFailure"));

	FPlatformProcess::Sleep(5.0f);

	Dolphin::DolphinManager::GetInstance()->Reconnect();
}

void DolphinListener::ConnectSuccess(const mqtt::token & tok)
{
}

void DolphinListener::SubscribeFailure(const mqtt::token & tok)
{
	UE_LOG(LogDolphinListener, Warning, TEXT("SubscribeFailure"));
}

void DolphinListener::SubscribeSuccess(const mqtt::token & tok)
{
	UE_LOG(LogDolphinListener, Warning, TEXT("SubscribeSuccess"));
	for (int i = 0; i < tok.get_topics()->size(); ++i) {
		if (strcmp(tok.get_topics()->c_arr()[i], "f")) {
			UDolphinBPLibrary::OnConnectionReady();
		}
	}
}

void DolphinListener::OnConnected(const std::string & cause)
{
	UE_LOG(LogDolphinListener, Warning, TEXT("OnConnectSuccess"));

	//UDolphinBPLibrary::OnConnectSuccess();
	Dolphin::DolphinManager::GetInstance()->Subscribe("s");
}

void DolphinListener::ConnectionLost(const std::string & cause)
{
	UE_LOG(LogDolphinListener, Warning, TEXT("OnConnectionLost"));
	UDolphinBPLibrary::OnConnectionLost();

	Dolphin::DolphinManager::GetInstance()->Reconnect();
}

void DolphinListener::MessageReceived(mqtt::const_message_ptr msg)
{
	UE_LOG(LogDolphinListener, Warning, TEXT("MessageReceived"));

	mqtt::binary_ref payload = msg->get_payload_ref();
	const char * topic = msg->get_topic_ref().c_str();
	size_t payloadSize = msg->get_payload_ref().length();
	
	if (strcmp(topic, "s") == 0)
	{
		const char * clientid = TCHAR_TO_ANSI(*UDolphinBPLibrary::ClientID);

		size_t clientidSize = UDolphinBPLibrary::ClientID.Len();
		size_t size = payloadSize + clientidSize;

		uint8 * content = (uint8 *)malloc(size);
		memset(content, 0, size);
		memcpy(content, clientid, clientidSize);
		memcpy(content + clientidSize, payload.c_str(), payloadSize);

		uint8 a = content[32];
		content[32] = content[6];
		content[6] = a;

		uint8 b = content[48];
		content[48] = content[31];
		content[31] = b;

		FMD5 md5;
		md5.Update(content, size);
		md5.Final(UDolphinBPLibrary::XKey);
		//memcpy(UDolphinBPLibrary::XKey + 16, UDolphinBPLibrary::XKey, 16);

		free(content);
		Dolphin::DolphinManager::GetInstance()->Unsubscribe("s");
		Dolphin::DolphinManager::GetInstance()->Subscribe("f");
		UDolphinBPLibrary::OnConnectSuccess();
	}
	else if (strcmp(topic, "f") == 0)
	{
		OriginalMessage * message = new OriginalMessage(payloadSize - 2);

		uint8* buffer = (uint8*)malloc(payloadSize);

		uint32 key_schedule[60];
		aes_key_setup(UDolphinBPLibrary::XKey, key_schedule, 128);
		aes_decrypt_ctr((const uint8 *)payload.data(), payloadSize, (uint8 *)buffer, key_schedule, 128, UDolphinBPLibrary::XKey);

		message->MsgId = msg->get_msgid();
		message->ProtocolModule = buffer[0];
		message->SpecificProtocol = buffer[1];
		memcpy(message->Buffer, buffer + 2, message->BufferSize);

		free(buffer);

		FString ProtocolModuleStr("EProtocolModule");
		UEnum* ProtocolModuleEnum = FindObject<UEnum>(ANY_PACKAGE, *ProtocolModuleStr);
		FString ProtocolModuleName = ProtocolModuleEnum->GetNameStringByValue((uint8)message->ProtocolModule);

		FString SpecificProtocolStr("E");
		SpecificProtocolStr.Append(ProtocolModuleName);
		SpecificProtocolStr.Append("Protocol");
		UEnum* SpecificProtocolEnum = FindObject<UEnum>(ANY_PACKAGE, *SpecificProtocolStr);
		if (SpecificProtocolEnum != nullptr) {
			FString SpecificProtocolName = SpecificProtocolEnum->GetNameStringByValue(message->SpecificProtocol);

			FString DolphinResponseStr(ProtocolModuleName);
			DolphinResponseStr.Append("_");
			DolphinResponseStr.Append(SpecificProtocolName);
			DolphinResponseStr.Append("_");
			DolphinResponseStr.Append("Response");
			message->ResponseClass = DolphinResponseStr;
			UDolphinBPLibrary::Enqueue(message);
		}
		else
		{
			UE_LOG(LogDolphinListener, Warning, TEXT("ProtocalModule not found"));
		}

	}
}

void DolphinListener::MessageDelivered(mqtt::delivery_token_ptr token)
{
	UE_LOG(LogDolphinListener, Log, TEXT("MessageDelivered"));
}