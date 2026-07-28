#pragma once


//-----------------------------------------------------------------------------------------------
#include "NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <string>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef NamedStrings EventArgs;
typedef bool ( *EventCallbackFunctionPtr )( EventArgs& args );
typedef std::vector < EventCallbackFunctionPtr > SubscriptionList;

//-----------------------------------------------------------------------------------------------
struct EventSystemConfig
{
	bool m_isEnabled = true;
};


//-----------------------------------------------------------------------------------------------
class EventSystem 
{
public:
	EventSystem( EventSystemConfig const& config );
	~EventSystem() = default;

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr );
	void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr );
	void FireEvent( std::string const& eventName, EventArgs& args );
	void FireEvent( std::string const& eventName );

	std::vector< std::string > GetSubscribedEvents();

protected:
	EventSystemConfig m_config;
	std::map < std::string, SubscriptionList > m_subscriptionListsByEventName;
};

void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr );
void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr );
void FireEvent( std::string const& eventName, EventArgs& args );
void FireEvent( std::string const& eventName );