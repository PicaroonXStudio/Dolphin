#pragma once
#include <string>
#include "mqtt/async_client.h"

namespace Dolphin
{
	class IDophinListener
	{
	public:
		//Connection iaction_listener
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
	};
}