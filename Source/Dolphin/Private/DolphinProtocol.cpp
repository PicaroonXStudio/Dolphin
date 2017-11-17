#include "DolphinProtocol.h"
#include "DolphinBPLibrary.h"
#include "Bridge/DolphinManager.h"

#include "AES.h"

void  UDolphinRequest::Pack()
{
	UDolphinBPLibrary::t1 = high_resolution_clock::now();

	mSize = mMessage->ByteSize() + 2;
	mData = std::shared_ptr<char>(new char[mSize], std::default_delete<char[]>());

	char * data = mData.get();

	memset(data, 0, mSize);

	data[0] = mProtocolModule;
	data[1] = mSpecificProtocol;

	mMessage->SerializeToArray(data + 2, mSize);

	duration<double, std::milli> time_span = high_resolution_clock::now() - UDolphinBPLibrary::t1;

	UE_LOG(LogTemp, Warning, TEXT("UserLoginRequest_Pack_Elapse %f milliseconds"), time_span.count());

	UDolphinBPLibrary::t1 = high_resolution_clock::now();

	uint32 key_schedule[60];
	aes_key_setup(UDolphinBPLibrary::XKey, key_schedule, 128);
	aes_encrypt_ctr((const uint8 *)data, mSize, (uint8 *)data, key_schedule, 128, UDolphinBPLibrary::XKey);

	time_span = high_resolution_clock::now() - UDolphinBPLibrary::t1;
	UE_LOG(LogTemp, Warning, TEXT("UserLoginRequest_Encrypt_Elapse %f milliseconds"), time_span.count());
}

void UDolphinRequest::Send()
{
	Pack();

	UDolphinBPLibrary::t1 = high_resolution_clock::now();
	Dolphin::DolphinManager::GetInstance()->Send("f", mData.get(), mSize);
}

void UDolphinResponse::Dispatch(OriginalMessage *message)
{
	Unpack(message);
	UDolphinBPLibrary::Dispatch(protocolNo, this);
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