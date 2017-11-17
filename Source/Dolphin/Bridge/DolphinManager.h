#pragma once
#include "IDophinListener.h"

#include <string>

#include "mqtt/async_client.h"

#include "Singleton.h"

using namespace std;

const int	QOS = 1;

namespace Dolphin
{
	namespace Mqtt {
		class SubscribeListener : public virtual mqtt::iaction_listener
		{
			std::vector<IDophinListener*> *mDolphinListeners = nullptr;

			void on_failure(const mqtt::token& tok) override {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->SubscribeFailure(tok);
					}
				}
			}

			void on_success(const mqtt::token& tok) override {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->SubscribeSuccess(tok);
					}
				}
			}

		public:
			SubscribeListener(std::vector<IDophinListener*> *dolphinListeners) {
				mDolphinListeners = dolphinListeners;
			}
		};

		class ConnectListener : public virtual mqtt::iaction_listener
		{
			std::vector<IDophinListener*> *mDolphinListeners = nullptr;

			void on_failure(const mqtt::token& tok) override {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->ConnectFailure(tok);
					}
				}
			}

			void on_success(const mqtt::token& tok) override {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->ConnectSuccess(tok);
					}
				}
			}

		public:
			ConnectListener(std::vector<IDophinListener*> *dolphinListeners) {
				mDolphinListeners = dolphinListeners;
			}
		};

		class AsyncCallback : public virtual mqtt::callback
		{
			std::vector<IDophinListener*> *mDolphinListeners = nullptr;

			void connected(const string& cause) {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->OnConnected(cause);
					}
				}
			}

			void connection_lost(const std::string& cause) override {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->ConnectionLost(cause);
					}
				}
			}

			void message_arrived(mqtt::const_message_ptr msg) override {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->MessageReceived(msg);
					}
				}
			}

			void delivery_complete(mqtt::delivery_token_ptr token) override {
				if (mDolphinListeners != nullptr) {
					for (std::vector<IDophinListener*>::iterator dolphinListener = mDolphinListeners->begin(); dolphinListener != mDolphinListeners->end(); ++dolphinListener)
					{
						(*dolphinListener)->MessageDelivered(token);
					}
				}
			}

		public:
			AsyncCallback(std::vector<IDophinListener*> *dolphinListeners) {
				mDolphinListeners = dolphinListeners;
			}
		};
	}
	class DolphinManager : public Singleton<DolphinManager>
	{
		friend class Singleton<DolphinManager>;

	private:
		DolphinManager();
		~DolphinManager();

		Mqtt::ConnectListener *mConnectListener = nullptr;
		Mqtt::SubscribeListener *mSubscribeListener = nullptr;
		Mqtt::AsyncCallback *mAsyncCallback = nullptr;

		mqtt::async_client *mAsyncClient = nullptr;
		mqtt::connect_options *mConnectOptions = nullptr;

		std::vector<IDophinListener*> mDolphinListeners;

	public:
		void Connect(const char *IP, const char *ClientID);
		void Reconnect();

		void Disconnect();

		void Subscribe(const string &topic);
		void Unsubscribe(const string &topic);

		void Send(const string &topic,char * payload, size_t len);

		void AddDolphinListener(IDophinListener*);
		void RemoveDolphinListener(IDophinListener*);
	};
}