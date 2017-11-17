// Fill out your copyright notice in the Description page of Project Settings.

#include "DolphinManager.h"
#include <algorithm>

using namespace Dolphin;
using namespace Dolphin::Mqtt;

DolphinManager::DolphinManager()
{
}

DolphinManager::~DolphinManager()
{
}

void Dolphin::DolphinManager::Connect(const char *IP, const char *ClientID)
{
	const string SERVER_ADDRESS{ IP };
	if (mAsyncClient == nullptr)
	{
		mConnectListener = new ConnectListener(&mDolphinListeners);
		mSubscribeListener = new SubscribeListener(&mDolphinListeners);

		mConnectOptions = new mqtt::connect_options;
		mAsyncClient = new mqtt::async_client(SERVER_ADDRESS, ClientID);
		mAsyncCallback = new AsyncCallback(&mDolphinListeners);
	}

	if (mAsyncClient->is_connected())
	{
		return;
	}

	mConnectOptions->set_connection_timeout(3);

	mConnectOptions->set_keep_alive_interval(20);
	mConnectOptions->set_clean_session(true);

	mAsyncClient->set_callback(*mAsyncCallback);

	mAsyncClient->connect(*mConnectOptions, nullptr, *mConnectListener);
}

void Dolphin::DolphinManager::Reconnect()
{
	if (mAsyncClient != nullptr && !mAsyncClient->is_connected())
	{
		mAsyncClient->reconnect();
	}
}

void DolphinManager::Disconnect()
{
	if (mAsyncClient != nullptr)
	{
		if (mAsyncClient->is_connected()) {
			mAsyncClient->disconnect()->wait();
		}

		delete mConnectOptions;
		delete mAsyncClient;
		delete mConnectListener;
		delete mSubscribeListener;
		delete mAsyncCallback;

		mSubscribeListener = nullptr;
		mConnectOptions = nullptr;
		mAsyncClient = nullptr;
		mConnectListener = nullptr;
		mAsyncCallback = nullptr;
	}
}

void Dolphin::DolphinManager::Subscribe(const string & topic)
{
	if (mAsyncClient != nullptr)
	{
		mAsyncClient->subscribe(topic, QOS, nullptr, *mSubscribeListener);
	}
}

void Dolphin::DolphinManager::Unsubscribe(const string & topic)
{
	if (mAsyncClient != nullptr)
	{
		mAsyncClient->unsubscribe(topic);
	}
}

void Dolphin::DolphinManager::Send(const string & topic, char * payload, size_t len)
{
	if (mAsyncClient)
	{
		mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload, len);
		pubmsg->set_qos(QOS);
		try {
			mAsyncClient->publish(pubmsg);
		}
		catch (const std::exception& e)
		{
			std::cout << e.what();
			//UE_LOG(LogTemp, Warning, TEXT("Exception %s"),*FString(e.what()));
		}
	}
}

void DolphinManager::AddDolphinListener(IDophinListener *l)
{
	mDolphinListeners.push_back(l);
}

void DolphinManager::RemoveDolphinListener(IDophinListener *l)
{
	mDolphinListeners.erase(std::remove(mDolphinListeners.begin(), mDolphinListeners.end(), l), mDolphinListeners.end());
}