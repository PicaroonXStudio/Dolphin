// Fill out your copyright notice in the Description page of Project Settings.

#include "DolphinManager.h"
#include <algorithm>

using namespace Dolphin;
using namespace Dolphin::Mqtt;

DEFINE_LOG_CATEGORY(LogDolphinManager);

DolphinManager::DolphinManager()
{
	UE_LOG(LogDolphinManager, Warning, TEXT("DolphinManager"));
#if DISABLE_MQTT==0
	mConnectOptions = new mqtt::connect_options;
	mConnectOptions->set_connection_timeout(3);
	mConnectOptions->set_keep_alive_interval(20);
	mConnectOptions->set_clean_session(true);

	mConnectListener = new ConnectListener(&mDolphinListeners);
	mSubscribeListener = new SubscribeListener(&mDolphinListeners);
	mAsyncCallback = new AsyncCallback(&mDolphinListeners);
#endif

}

DolphinManager::~DolphinManager()
{
	UE_LOG(LogDolphinManager, Warning, TEXT("~DolphinManager"));
#if DISABLE_MQTT==0
	delete mConnectOptions;
	delete mConnectListener;
	delete mSubscribeListener;
	delete mAsyncCallback;

	mConnectOptions = nullptr;
	mSubscribeListener = nullptr;
	mConnectListener = nullptr;
	mAsyncCallback = nullptr;
#endif
}

void Dolphin::DolphinManager::Connect(const char* IP, const char* ClientID)
{
	UE_LOG(LogDolphinManager, Warning, TEXT("Connect"));

	mIPAddress = IP;
	mClientID = ClientID;
#if DISABLE_MQTT==0
	if (mAsyncClient == nullptr)
	{
		mAsyncClient = new mqtt::async_client(mIPAddress, mClientID);
		mAsyncClient->set_callback(*mAsyncCallback);
	}

	mAsyncClient->connect(*mConnectOptions, nullptr, *mConnectListener);
#endif
}

void Dolphin::DolphinManager::Reconnect()
{
	UE_LOG(LogDolphinManager, Warning, TEXT("Reconnect"));
#if DISABLE_MQTT==0
	if (mAsyncClient == nullptr)
	{
		mAsyncClient = new mqtt::async_client(mIPAddress, mClientID);
		mAsyncClient->set_callback(*mAsyncCallback);
	}

	mAsyncClient->connect(*mConnectOptions, nullptr, *mConnectListener);
#endif
}

void DolphinManager::Disconnect()
{
	UE_LOG(LogDolphinManager, Warning, TEXT("Disconnect"));
#if DISABLE_MQTT==0
	if (mAsyncClient != nullptr)
	{
		if (mAsyncClient->is_connected()) {
			mAsyncClient->disconnect()->wait();
		}

		delete mAsyncClient;

		mAsyncClient = nullptr;
	}
#endif
}

void Dolphin::DolphinManager::Subscribe(const string& topic)
{
	UE_LOG(LogDolphinManager, Warning, TEXT("Subscribe"));
#if DISABLE_MQTT==0
	if (mAsyncClient != nullptr)
	{
		mAsyncClient->subscribe(topic, QOS, nullptr, *mSubscribeListener);
	}
#endif
}

void Dolphin::DolphinManager::Unsubscribe(const string& topic)
{
	UE_LOG(LogDolphinManager, Warning, TEXT("Unsubscribe"));
#if DISABLE_MQTT==0
	if (mAsyncClient != nullptr)
	{
		mAsyncClient->unsubscribe(topic);
	}
#endif
}

int Dolphin::DolphinManager::Send(const string& topic, char* payload, size_t len)
{
	UE_LOG(LogDolphinManager, Warning, TEXT("Send"));
	int msgId = 0;
#if DISABLE_MQTT==0
	if (mAsyncClient && mAsyncClient->is_connected())
	{
		mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload, len);
		pubmsg->set_qos(QOS);
		//try {
		msgId = mAsyncClient->publish(pubmsg)->get_message_id();
		UE_LOG(LogDolphinManager, Warning, TEXT("MessageID %d"), msgId);
		//		}
				//catch (const std::exception& e)
				//{
				//	FString ex(UTF8_TO_TCHAR(e.what()));
				//	UE_LOG(LogDolphinManager, Warning, TEXT("Exception %s"),*ex);
				//}
	}
	else
	{
		UE_LOG(LogDolphinManager, Warning, TEXT("No Connection"));
	}
#endif
	return msgId;
}

void DolphinManager::AddDolphinListener(IDophinListener* l)
{
#if DISABLE_MQTT==0
	if (std::find(mDolphinListeners.begin(), mDolphinListeners.end(), l) == mDolphinListeners.end()) {
		mDolphinListeners.push_back(l);
	}
#endif
}

void DolphinManager::RemoveDolphinListener(IDophinListener* l)
{
#if DISABLE_MQTT==0
	mDolphinListeners.erase(std::remove(mDolphinListeners.begin(), mDolphinListeners.end(), l), mDolphinListeners.end());
#endif
}

#if DISABLE_MQTT==0
void Dolphin::Mqtt::SubscribeListener::on_failure(const mqtt::token& tok)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->SubscribeFailure(tok);
		}
	}
}

void Dolphin::Mqtt::SubscribeListener::on_success(const mqtt::token& tok)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->SubscribeSuccess(tok);
		}
	}
}

Dolphin::Mqtt::SubscribeListener::SubscribeListener(std::vector<IDophinListener*>* dolphinListeners)
	:mDolphinListeners(dolphinListeners)
{
}

void Dolphin::Mqtt::ConnectListener::on_failure(const mqtt::token& tok)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->ConnectFailure(tok);
		}
	}
}

void Dolphin::Mqtt::ConnectListener::on_success(const mqtt::token& tok)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->ConnectSuccess(tok);
		}
	}
}

Dolphin::Mqtt::ConnectListener::ConnectListener(std::vector<IDophinListener*>* dolphinListeners)
	:mDolphinListeners(dolphinListeners)
{
}

void Dolphin::Mqtt::AsyncCallback::connected(const string& cause)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->OnConnected(cause);
		}
	}
}

void Dolphin::Mqtt::AsyncCallback::connection_lost(const std::string& cause)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->ConnectionLost(cause);
		}
	}
}

void Dolphin::Mqtt::AsyncCallback::message_arrived(mqtt::const_message_ptr msg)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->MessageReceived(msg);
		}
	}
}

void Dolphin::Mqtt::AsyncCallback::delivery_complete(mqtt::delivery_token_ptr token)
{
	if (mDolphinListeners != nullptr) {
		for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
		{
			(*dolphinListener)->MessageDelivered(token);
		}
	}
}

Dolphin::Mqtt::AsyncCallback::AsyncCallback(std::vector<IDophinListener*>* dolphinListeners)
	: mDolphinListeners(dolphinListeners)
{
}
#endif