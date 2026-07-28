#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/EventSystem.hpp"

//-----------------------------------------------------------------------------------------------
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <deque>
#include <vector>

//-----------------------------------------------------------------------------------------------
struct PacketHeader
{
    uint16_t sequence = 0;  // 2 bytes  - packetID
    uint16_t ack      = 0;  // 2 bytes  - remoteSequence
    uint32_t ackBits  = 0;  // 4 bytes
};

//-----------------------------------------------------------------------------------------------
struct Packet
{
    PacketHeader header;
    uint8_t      message[ 1192 ] = {};  // 1200 - 8
};

//-----------------------------------------------------------------------------------------------
enum NetworkState
{
    NET_STATE_INACTIVE,
    NET_STATE_IDLE,
    NET_STATE_SERVER_LISTENING,
    NET_STATE_CLIENT_CONNECTING,
    NET_STATE_CLIENT_CONNECTED
};

//-----------------------------------------------------------------------------------------------
struct NetworkConfig
{
    bool        m_isEnabled  = true;
    std::string m_port       = "3100";
    std::string m_remoteIP   = "127.0.0.1";
    std::string m_remotePort = "3200";  //UDP only
    std::string m_serverName = "Edwin's Nest";
};

//-----------------------------------------------------------------------------------------------
class NetworkSystem
{
public:
    NetworkSystem( NetworkConfig const& config );
    ~NetworkSystem() = default;

    void Startup();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void Server_BeginFrame();
    void Client_BeginFrame();

    void Server_EndFrame();
    void Client_EndFrame();

    bool StartClient();
    bool StartServer();

    void Disconnect();
    int  ErrorHandling();

    bool StartUDP();

    void UDP_BeginFrame();
    void UDP_EndFrame();

    char const* GetNetworkState() const;
    std::string GetErrorMessage( int errorCode );
    void        SendStringToAll( std::string commandString );

    static bool Event_StartServer( EventArgs& args );
    static bool Event_StartClient( EventArgs& args );
    static bool Event_Disconnect( EventArgs& args );
    static bool Event_StartUDPServer( EventArgs& args );

    // test
    static bool Event_SendTime( EventArgs& args );

public:
    NetworkConfig m_config;
    NetworkState  m_state = NET_STATE_INACTIVE;

    SOCKET m_udpSocket = INVALID_SOCKET;

    // server
    SOCKET m_listenSocket = INVALID_SOCKET;
    SOCKET m_clientSocket = INVALID_SOCKET;

    // client
    SOCKET m_connectionToServer = INVALID_SOCKET;

    std::deque< char > m_outgoingDataQueuedToSend;
    std::deque< char > m_incomingDataQueuedToReceive;

    // test
    std::string m_testMessage;
    uint16_t    m_localSequence           = 0;
    uint16_t    m_remoteSequence          = 0;
    uint32_t    m_ackBits                 = 0;
    bool        m_hasReceivedRemotePacket = false;
};