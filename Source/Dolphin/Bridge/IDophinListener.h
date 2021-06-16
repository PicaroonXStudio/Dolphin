#pragma once
#include <string>
#if PLATFORM_SWITCH == 1
#define DISABLE_MQTT 1
#else
#define DISABLE_MQTT 0
#endif
#if DISABLE_MQTT==0
#include "mqtt/async_client.h"
#endif

namespace Dolphin
{
	class IDophinListener
	{
	public:
		//Connection iaction_listener
#if DISABLE_MQTT==0
		virtual	void ConnectFailure(const mqtt::token& tok) {}
		virtual	void ConnectSuccess(const mqtt::token& tok) {}

		//Connection Callback
		virtual void OnConnected(const std::string& cause) {}
		virtual void ConnectionLost(const std::string& cause) {}

		//Subscribe iaction_listener
		virtual	void SubscribeFailure(const mqtt::token& tok) {}
		virtual	void SubscribeSuccess(const mqtt::token& tok) {}

		//Message Callback
		virtual void MessageReceived(mqtt::const_message_ptr msg) {}
		virtual void MessageDelivered(mqtt::delivery_token_ptr token) {}
#endif
	};
}