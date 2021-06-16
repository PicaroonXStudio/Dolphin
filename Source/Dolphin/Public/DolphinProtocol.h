// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "google/protobuf/message.h"
#include "mqtt/async_client.h"
#include "DolphinProtocol.generated.h"

UENUM(BlueprintType)
enum class EConnectStatus : uint8
{
	initing = 0	UMETA(DisplayName = "0"),
	
	success = 1	UMETA(DisplayName = "连接成功"),

	failure = 2	UMETA(DisplayName = "连接失败"),

	lost = 3	UMETA(DisplayName = "连接丢失"),

	ready = 4	UMETA(DisplayName = "连接就绪"),

	disconnect = 5	UMETA(DisplayName = "连接断开"),

};


DECLARE_LOG_CATEGORY_EXTERN(LogDolphinProtocol, Log, All);

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
	int MsgId;
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
DECLARE_DYNAMIC_DELEGATE_OneParam(FDolphinConnectDelegate, EConnectStatus, connectStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDolphinResponseDelegate, UDolphinResponse *, DolphinResponse);

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

	UPROPERTY(BlueprintAssignable, Category = "Dolphin")
		FDolphinResponseDelegate OnResponse;
};


