#include "Engine/Core/EventSystem.hpp"
#include "ErrorWarningAssert.hpp"
#include "Engine.hpp"


//-----------------------------------------------------------------------------------------------
EventSystem::EventSystem( EventSystemConfig const& config )
	: m_config( config )
{

}

void EventSystem::Startup()
{

}

void EventSystem::Shutdown()
{

}

void EventSystem::BeginFrame()
{

}

void EventSystem::EndFrame()
{

}

//-----------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr )
{
	for ( int functionIndex = 0; functionIndex < static_cast< int >( m_subscriptionListsByEventName [ eventName ].size() ); ++ functionIndex )
	{
		if ( m_subscriptionListsByEventName [ eventName ][ functionIndex ] == nullptr )
		{
			m_subscriptionListsByEventName [ eventName ][ functionIndex ] = functionPtr;
			return;
		}
	}

	m_subscriptionListsByEventName [ eventName ].push_back( functionPtr );
}

//-----------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr )
{
	std::map<std::string, SubscriptionList>::iterator  iterator = m_subscriptionListsByEventName.find( eventName );
	GUARANTEE_OR_DIE( iterator != m_subscriptionListsByEventName.end(), Stringf( "Event: %s Not Found!!!", eventName.c_str() ) );
	SubscriptionList& functionList = iterator->second;

	for ( int funIndex = 0; funIndex < static_cast< int >( functionList.size() ); ++ funIndex )
	{
		if ( functionList[ funIndex ] == functionPtr )
		{
			functionList [ funIndex ] = nullptr;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName, EventArgs& args )
{
	std::map<std::string, SubscriptionList>::const_iterator  iterator = m_subscriptionListsByEventName.find( eventName );
	GUARANTEE_OR_DIE( iterator != m_subscriptionListsByEventName.end(), Stringf( "Event: %s Not Found!!!", eventName.c_str() ) );
	SubscriptionList subscribers = iterator->second;

	for ( int subscribersIndex = 0; subscribersIndex < static_cast< int >( subscribers.size() ); ++ subscribersIndex )
	{
		if ( subscribers [ subscribersIndex ] != nullptr )
		{
			bool wasConsumed = subscribers [ subscribersIndex ]( args );
			if ( wasConsumed )
			{
				break;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName )
{
	EventArgs args;
	std::map<std::string, SubscriptionList>::const_iterator  iterator = m_subscriptionListsByEventName.find( eventName );
	GUARANTEE_OR_DIE( iterator != m_subscriptionListsByEventName.end(), Stringf( "Event: %s Not Found!!!", eventName.c_str() ) );
	SubscriptionList subscribers = iterator->second;
	for ( int subscriberIndex = 0; subscriberIndex < static_cast< int >( subscribers.size() ); ++ subscriberIndex )
	{
		if ( subscribers [ subscriberIndex ] != nullptr )
		{
			bool wasConsumed = subscribers [ subscriberIndex ]( args );
			if ( wasConsumed )
			{
				break;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
std::vector< std::string > EventSystem::GetSubscribedEvents()
{
	std::vector<std::string> eventNames;
	for ( std::map<std::string, SubscriptionList>::iterator it = m_subscriptionListsByEventName.begin(); it != m_subscriptionListsByEventName.end(); ++ it )
	{
		eventNames.push_back( it->first );
	}
	return eventNames;
}


//-----------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr )
{
	if ( g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->SubscribeEventCallbackFunction( eventName, functionPtr );
	}
}


//-----------------------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunctionPtr functionPtr )
{
	if ( g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->UnsubscribeEventCallbackFunction( eventName, functionPtr );
	}
}


//-----------------------------------------------------------------------------------------------
void FireEvent( std::string const& eventName, EventArgs& args )
{
	if ( g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->FireEvent( eventName, args );
	}
}


//-----------------------------------------------------------------------------------------------
void FireEvent( std::string const& eventName )
{
	EventArgs args;
	if ( g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->FireEvent( eventName, args );
	}
}