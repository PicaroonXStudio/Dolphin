#include "DolphinProtocol.h"
#include "DolphinBPLibrary.h"
#include "Dolphin/Crypto/AES.h"

DEFINE_LOG_CATEGORY(LogDolphinProtocol);

void  UDolphinRequest::Pack()
{
	UE_LOG(LogDolphinProtocol, Warning, TEXT("UDolphinRequest::Pack"));

	mSize = mMessage->ByteSize() + 2;
	mData = std::shared_ptr<char>(new char[mSize], std::default_delete<char[]>());

	char * data = mData.get();

	memset(data, 0, mSize);

	data[0] = mProtocolModule;
	data[1] = mSpecificProtocol;

	mMessage->SerializeToArray(data + 2, mSize);

	uint32 key_schedule[60];
	aes_key_setup(UDolphinBPLibrary::XKey, key_schedule, 128);
	aes_encrypt_ctr((const uint8 *)data, mSize, (uint8 *)data, key_schedule, 128, UDolphinBPLibrary::XKey);
}

void UDolphinRequest::Send()
{
	Pack();
	int msgId = Dolphin::DolphinManager::GetInstance()->Send("f", mData.get(), mSize);

	if (OnResponse.IsBound())
	{
		UDolphinBPLibrary::SpecificResponseDelegates.Add(msgId, OnResponse);
	}
}

void UDolphinResponse::Dispatch(OriginalMessage *message)
{
	Unpack(message);
	UDolphinBPLibrary::Dispatch(protocolNo, this, message->MsgId);
}

void UDolphinResponse::Unpack(OriginalMessage *message)
{

}

OriginalMessage::OriginalMessage(size_t bufferLength)
{
	Buffer = (char *)malloc(bufferLength);
	BufferSize = bufferLength;
}

OriginalMessage::~OriginalMessage()
{
	free(Buffer);
}