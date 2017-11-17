// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "google/protobuf/message.h"
#include "mqtt/async_client.h"
#include "Engine.h"
#include "DolphinProtocol.generated.h"

/**
*  OriginalMesage use for DophinResponse to Dispatch
*/
class OriginalMessage
{
public:
	uint8 ProtocolModule;
	uint8 SpecificProtocol;
	FString ResponseClass;
	char *Buffer;
	size_t BufferSize;
	OriginalMessage(size_t bufferLength);
	~OriginalMessage();
};

USTRUCT(BlueprintType)
struct FResponseData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Example)
		FString msg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Example)
		uint8 state;
};

UCLASS(Blueprintable)
class DOLPHIN_API UDolphinRequest : public UObject
{
	GENERATED_BODY()
private:
	std::shared_ptr<char> mData;
	int mSize;

protected:
	google::protobuf::Message* mMessage = nullptr;
	uint8 mProtocolModule = 0;
	uint8 mSpecificProtocol = 0;
	virtual void Pack();

public:
	UFUNCTION(BlueprintCallable, Category = "Dolphin")
		void Send();
};

UCLASS(Blueprintable)
class DOLPHIN_API UDolphinResponse : public UObject
{
	GENERATED_BODY()

protected:
	uint16 protocolNo;

	virtual void Unpack(OriginalMessage *message);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Example, Meta = (ExposeOnSpawn = true))
		FResponseData responseData;

	void Dispatch(OriginalMessage *message);
};
