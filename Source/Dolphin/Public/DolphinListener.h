// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bridge/IDophinListener.h"

class DolphinListener : public Dolphin::IDophinListener
{
	void ConnectFailure(const mqtt::token& tok);

	void ConnectSuccess(const mqtt::token& tok);

	void SubscribeFailure(const mqtt::token& tok);

	void SubscribeSuccess(const mqtt::token& tok);

	void OnConnected(const std::string& cause);

	void ConnectionLost(const std::string& cause);

	void MessageReceived(mqtt::const_message_ptr msg);

	void MessageDelivered(mqtt::delivery_token_ptr token);
};
