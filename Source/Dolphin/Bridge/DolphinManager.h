#pragma once

#include "IDophinListener.h"
#include "Engine.h"

#include <string>

#include "mqtt/async_client.h"

#include "Singleton.h"

using namespace std;

const int	QOS = 1;

DECLARE_LOG_CATEGORY_EXTERN(LogDolphinManager, Log, All);

namespace Dolphin
{
	namespace Mqtt {
#if DISABLE_MQTT==0
		class SubscribeListener : public virtual mqtt::iaction_listener
		{
			std::vector<IDophinListener*>* mDolphinListeners = nullptr;

			void on_failure(const mqtt::token& tok) override;

			void on_success(const mqtt::token& tok) override;

		public:
			SubscribeListener(std::vector<IDophinListener*>* dolphinListeners);
		};

		class ConnectListener : public virtual mqtt::iaction_listener
		{
			std::vector<IDophinListener*>* mDolphinListeners = nullptr;

			void on_failure(const mqtt::token& tok) override;

			void on_success(const mqtt::token& tok) override;

		public:
			ConnectListener(std::vector<IDophinListener*>* dolphinListeners);
		};

		class AsyncCallback : public virtual mqtt::callback
		{
			std::vector<IDophinListener*>* mDolphinListeners = nullptr;

			void connected(const string& cause);

			void connection_lost(const std::string& cause) override;

			void message_arrived(mqtt::const_message_ptr msg) override;

			void delivery_complete(mqtt::delivery_token_ptr token) override;

		public:
			AsyncCallback(std::vector<IDophinListener*>* dolphinListeners);
		};
#endif
	}

	class DolphinManager : public Singleton<DolphinManager>
	{
		friend class Singleton<DolphinManager>;

	private:
		DolphinManager();
		~DolphinManager();
#if DISABLE_MQTT==0
		Mqtt::ConnectListener* mConnectListener = nullptr;
		Mqtt::SubscribeListener* mSubscribeListener = nullptr;
		Mqtt::AsyncCallback* mAsyncCallback = nullptr;

		mqtt::async_client* mAsyncClient = nullptr;
		mqtt::connect_options* mConnectOptions = nullptr;

		std::vector<IDophinListener*> mDolphinListeners;
#endif
		std::string mIPAddress;
		std::string mClientID;

	public:
		void Connect(const char* IP, const char* ClientID);
		void Reconnect();

		void Disconnect();

		void Subscribe(const string& topic);
		void Unsubscribe(const string& topic);

		int Send(const string& topic, char* payload, size_t len);

		void AddDolphinListener(IDophinListener*);
		void RemoveDolphinListener(IDophinListener*);
	};
}