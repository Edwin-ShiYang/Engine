//-----------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <WS2TCPIP.h>
#include <WinSock2.h>
#include <Windows.h>
#pragma comment( lib, "Ws2_32.lib" )

#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "chrono"

//-----------------------------------------------------------------------------------------------
NetworkSystem::NetworkSystem( NetworkConfig const& config )
    : m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::Startup()
{
    /*
        MAKEWORD( 2, 2 ) - Winsock 2.2
        WSAStartup - initialize the sockets layer
    */
    WSADATA data;
    int     errorCode = WSAStartup( MAKEWORD( 2, 2 ), &data );
    GUARANTEE_OR_DIE( errorCode == 0, "[Network]    Failed to startup NetworkSystem" );
    m_state = NET_STATE_IDLE;
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_GREEN, "[Network]    Successfully started up the NetworkSystem" );

    // SubscribeEvents
    SubscribeEventCallbackFunction( "StartServer", NetworkSystem::Event_StartServer );
    SubscribeEventCallbackFunction( "StartClient", NetworkSystem::Event_StartClient );
    SubscribeEventCallbackFunction( "Disconnect", NetworkSystem::Event_Disconnect );
    SubscribeEventCallbackFunction( "StartUDPServer", NetworkSystem::Event_StartUDPServer );

    SubscribeEventCallbackFunction( "SendTime", NetworkSystem::Event_SendTime );
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::StartServer()
{
    if ( m_state == NET_STATE_INACTIVE )
    {
        return false;
    }

    if ( m_state == NET_STATE_CLIENT_CONNECTING || m_state == NET_STATE_CLIENT_CONNECTED )
    {
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, "[Error]   Cannot start server: client is already running." );
        return false;
    }

    int result;

    /*
        AF_INET     - IPv4
        SOCK_STREAM - Reliable, connection-based byte stream socket
        IPPROTO_TCP - TCP Protocol
    */
    m_listenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    GUARANTEE_OR_DIE( m_listenSocket != INVALID_SOCKET, Stringf( "[Network] Failed to create listen socket. Error: %d", ErrorHandling() ) );

    /*
        non-blocking - 1
        blocking     - 0
        FIONBIO      - Set socket is blocking or non-blocking
    */
    unsigned long blockingMode = 1;
    result                     = ioctlsocket( m_listenSocket, FIONBIO, &blockingMode );
    if ( result == SOCKET_ERROR )
    {
        closesocket( m_listenSocket );
        GUARANTEE_OR_DIE( result != SOCKET_ERROR, Stringf( "[Network] Failed to set server socket non-blocking. Error: %d", ErrorHandling() ) );
    }

    /*
        INADDR_ANY - Listen on all local IPv4 addresses
        TCP port   - 3100
    */
    uint32_t    myIPAddressU32  = INADDR_ANY;
    uint16_t    myListenPortU16 = static_cast< unsigned short >( atoi( m_config.m_port.c_str() ) );

    /*
        Build the local IPv4 address that the server will bind to
        AF_INET         - IPv4 address
        myIPAddressU32  - Local IP address
        myListenPortU16 - TCP port
    */
    sockaddr_in addr;
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl( myIPAddressU32 );
    addr.sin_port             = htons( myListenPortU16 );

    /*
        SERVER-SPECIFIC
        SOMAXCONN - maximum pending connection queue size
        Listen for clients on port 3100 from any IPv4 address assigned to this machine
    */
    result = bind( m_listenSocket, (sockaddr*)&addr, (int)sizeof( addr ) );
    if ( result == SOCKET_ERROR )
    {
        closesocket( m_listenSocket );
        ERROR_AND_DIE( Stringf( "[Network] Bind failed. Error: %d", ErrorHandling() ) );
    }

    if ( listen( m_listenSocket, SOMAXCONN ) == SOCKET_ERROR )
    {
        closesocket( m_listenSocket );
        ERROR_AND_DIE( Stringf( "[Network] Listen failed. Error: %d", ErrorHandling() ) );
    }

    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_GREEN, Stringf( "[Network] Server is listening at port %s ...", m_config.m_port.c_str() ) );
    m_state = NET_STATE_SERVER_LISTENING;
    return true;
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::StartUDP()
{
    int result;

    /*
        AF_INET     - IPv4
        SOCK_DGRAM  - Datagram-based socket
        IPPROTO_TCP - UDP protocol
    */
    m_udpSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    GUARANTEE_OR_DIE( m_udpSocket != INVALID_SOCKET, Stringf( "[Network] Failed to create udp socket. Error: %d", ErrorHandling() ) );

    sockaddr_in address;
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl( INADDR_ANY );  // host to network long
    address.sin_port        = htons( static_cast< unsigned short >( atoi( m_config.m_port.c_str() ) ) );

    result = bind( m_udpSocket, (sockaddr*)&address, (int)sizeof( address ) );
    if ( result == SOCKET_ERROR )
    {
        int errCode = ErrorHandling();
        closesocket( m_udpSocket );
        m_udpSocket = INVALID_SOCKET;

        ERROR_AND_DIE( Stringf( "[Network Error] Failed to bind UDP socket. - %s ", GetErrorMessage( errCode ).c_str() ) );
    }

    unsigned long blockingMode = 1;
    result                     = ioctlsocket( m_udpSocket, FIONBIO, &blockingMode );
    if ( result == SOCKET_ERROR )
    {
        closesocket( m_udpSocket );
        GUARANTEE_OR_DIE( result != SOCKET_ERROR, Stringf( "[Network] Failed to set UDP socket non-blocking. Error: %d", ErrorHandling() ) );
    }

    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_GREEN, Stringf( "[Network] UDP socket successfully bound to port %s...", m_config.m_port.c_str() ) );
    return true;
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::UDP_BeginFrame()
{
    int         result;

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port   = htons( static_cast< unsigned short >( atoi( m_config.m_remotePort.c_str() ) ) );
    result             = inet_pton( AF_INET, m_config.m_remoteIP.c_str(), &address.sin_addr );  //Convert the destination IPv4 address from string format to network binary format

    if ( !m_testMessage.empty() )
    {
        Packet packet          = {};
        packet.header.sequence = m_localSequence++;
        packet.header.ack      = m_remoteSequence;
        packet.header.ackBits  = m_ackBits;

        memcpy( packet.message, m_testMessage.data(), m_testMessage.size() );
        char const* data       = reinterpret_cast< char const* >( &packet );
        int         packetSize = static_cast< int >( sizeof( PacketHeader ) + m_testMessage.size() );
        int         sent_bytes = sendto( m_udpSocket, data, packetSize, 0, reinterpret_cast< sockaddr const* >( &address ), static_cast< int >( sizeof( address ) ) );
        if ( sent_bytes != packetSize )
        {
            // g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, Stringf( "failed to send packet" ) );
            return;
        }

        m_testMessage.clear();
    }

    // recv
    while ( true )
    {
        unsigned char packetData[ 1200 ] = {};

        sockaddr_in   senderAddress       = {};
        int           senderAddressLength = sizeof( senderAddress );

        int           bytesReceived = recvfrom( m_udpSocket, reinterpret_cast< char* >( packetData ), static_cast< int >( sizeof( packetData ) ), 0, reinterpret_cast< sockaddr* >( &senderAddress ), &senderAddressLength );
        if ( bytesReceived == SOCKET_ERROR )
        {
            int errorCode = ErrorHandling();

            if ( errorCode == WSAEWOULDBLOCK )
            {
                // No more UDP packets are currently waiting to be received.
                break;
            }
            //Failed to receive UDP packet
            break;
        }

        if ( bytesReceived < static_cast< int >( sizeof( PacketHeader ) ) )
        {
            continue;
        }

        PacketHeader header = {};
        memcpy( &header, packetData, sizeof( PacketHeader ) );

        uint16_t sequence = header.sequence;
        uint16_t ack      = header.ack;
        uint32_t ackBits  = header.ackBits;

        if ( !m_hasReceivedRemotePacket )
        {
            m_remoteSequence          = sequence;
            m_ackBits                 = 0;
            m_hasReceivedRemotePacket = true;
        }
        else if ( sequence > m_remoteSequence )
        {
            uint16_t difference =
                static_cast< uint16_t >( sequence - m_remoteSequence );

            if ( difference > 32 )
            {
                m_ackBits = 0;
            }
            else if ( difference == 32 )
            {
                m_ackBits = 1u << 31;
            }
            else
            {
                m_ackBits <<= difference;

                // The previous m_remoteSequence was received.
                m_ackBits |= 1u << ( difference - 1 );
            }

            m_remoteSequence = sequence;
        }
        else if ( sequence < m_remoteSequence )
        {
            uint16_t difference =
                static_cast< uint16_t >( m_remoteSequence - sequence );

            if ( difference <= 32 )
            {
                m_ackBits |= 1u << ( difference - 1 );
            }
        }

        int         messageSize = bytesReceived - static_cast< int >( sizeof( PacketHeader ) );
        std::string message( reinterpret_cast< char const* >( packetData + sizeof( PacketHeader ) ), static_cast< size_t >( messageSize ) );
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_BLUE, Stringf( "------------------------------" ) );
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_BLUE, Stringf( "Received:  %d bytes", bytesReceived ) );
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_BLUE, Stringf( "Packet ID: %u", static_cast< unsigned int >( sequence ) ) );
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_BLUE, Stringf( "ACK:       %u", static_cast< unsigned int >( ack ) ) );
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_BLUE, Stringf( "ACK Bits:  0x%08X", static_cast< unsigned int >( ackBits ) ) );
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_BLUE, Stringf( "Message:   %s", message.c_str() ) );

        // unsigned short senderPort = ntohs( senderAddress.sin_port );
    }
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::UDP_EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::StartClient()
{
    if ( m_state == NET_STATE_INACTIVE )
    {
        return false;
    }

    if ( m_state == NET_STATE_SERVER_LISTENING )
    {
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, "[Error]   Cannot start client: already running as server." );
        return false;
    }

    int result;

    /*
        AF_INET     - IPv4
        SOCK_STREAM - Reliable, connection-based byte stream socket
        IPPROTO_TCP - TCP Protocol
    */
    m_connectionToServer = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    GUARANTEE_OR_DIE( m_connectionToServer != INVALID_SOCKET, Stringf( "[Network] Failed to create connection socket. Error: %d", ErrorHandling() ) );

    /*
        non-blocking - 1
        blocking     - 0
        FIONBIO      - Set socket is blocking or non-blocking
    */
    unsigned long blockingMode = 1;
    result                     = ioctlsocket( m_connectionToServer, FIONBIO, &blockingMode );
    GUARANTEE_OR_DIE( result != SOCKET_ERROR, Stringf( "[Network] Failed to set client socket non-blocking. Error: %d", ErrorHandling() ) );

    /*
        CLIENT-SPECIFIC
        Convert the IP address from text to network format
        The remote server IP that the client will connect to is 10.9.176.156::3100
    */
    IN_ADDR ipAddr;
    result = inet_pton( AF_INET, m_config.m_remoteIP.c_str(), &ipAddr );
    GUARANTEE_OR_DIE( result == 1, "[Network] Invalid server IP address" );

    uint32_t    serverIPAddressU32 = ntohl( ipAddr.S_un.S_addr );
    uint16_t    serverPortU16      = static_cast< unsigned short >( atoi( m_config.m_port.c_str() ) );

    /*
       Build the remote server IPv4 address that the client will connect to.
       AF_INET             - IPv4 address
       serverIPAddressU32  - Remote IP address
       serverPortU16       - TCP port
    */
    sockaddr_in addr;
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl( serverIPAddressU32 );
    addr.sin_port             = htons( serverPortU16 );

    /*
        CLIENT-SPECIFIC
        Connect to the server at remote IP address on port 3100
    */
    result = connect( m_connectionToServer, (sockaddr*)&addr, (int)sizeof( addr ) );
    if ( result == SOCKET_ERROR )
    {
        int error = ErrorHandling();
        if ( error == WSAEWOULDBLOCK )
        {
            m_state = NET_STATE_CLIENT_CONNECTING;
            g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_GREEN, "[Network] Client is connecting..." );
            return true;
        }

        closesocket( m_connectionToServer );
        m_connectionToServer = INVALID_SOCKET;
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, Stringf( "[Error]   Unable to connect to server. Error: %d", error ) );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::Server_BeginFrame()
{
    if ( m_state != NET_STATE_SERVER_LISTENING )
    {
        return;
    }

    int result;

    /*
        SERVER-SPECIFIC
        newClientSocket will continue to be set to INVALID_SOCKET until connected
        This accepts only a single connection
    */
    if ( m_clientSocket == INVALID_SOCKET )
    {
        SOCKET newClientSocket = accept( m_listenSocket, NULL, NULL );
        if ( newClientSocket == INVALID_SOCKET )
        {
            closesocket( newClientSocket );
            return;
        }

        /*
            Add this new socket to the server’s list of connected clients
            non-blocking - 1
            blocking     - 0
            FIONBIO      - Set socket is blocking or non-blocking
        */

        unsigned long blockingMode = 1;
        ioctlsocket( newClientSocket, FIONBIO, &blockingMode );
        m_clientSocket = newClientSocket;
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_GREEN, Stringf( "[Network] A client connected successfully" ) );
    }

    if ( !m_outgoingDataQueuedToSend.empty() )
    {
        constexpr int SEND_BUFFER_SIZE               = 1200;
        char          sendBuffer[ SEND_BUFFER_SIZE ] = {};
        int           writeSlot                      = 0;
        while ( !m_outgoingDataQueuedToSend.empty() && writeSlot < SEND_BUFFER_SIZE )
        {
            sendBuffer[ writeSlot++ ] = m_outgoingDataQueuedToSend.back();
            m_outgoingDataQueuedToSend.pop_back();
        }
        int numBytesToSend = writeSlot;
        result             = send( m_clientSocket, sendBuffer, numBytesToSend, 0 );
        if ( result == SOCKET_ERROR )
        {
            int errorCode = ErrorHandling();
            // disconnected? - change state if server disconnected
            g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, Stringf( "[Error]   Failed to send data to client. Error: %d", errorCode ) );
            return;
        }
        else if ( result < numBytesToSend )
        {
            // partial send
        }
        else
        {
            // full send
        }
    }

    constexpr int RECV_BUFFER_SIZE               = 2048;
    char          recvBuffer[ RECV_BUFFER_SIZE ] = {};
    result                                       = recv( m_clientSocket, recvBuffer, sizeof( recvBuffer ), 0 );

    if ( result > 0 )
    {
        for ( int byteIndex = 0; byteIndex < result; ++byteIndex )
        {
            m_incomingDataQueuedToReceive.push_back( recvBuffer[ byteIndex ] );
        }
    }

    auto nullIter = std::find( m_incomingDataQueuedToReceive.begin(), m_incomingDataQueuedToReceive.end(), '\0' );
    if ( nullIter == m_incomingDataQueuedToReceive.end() )
    {
        return;
    }

    std::string command;
    while ( !m_incomingDataQueuedToReceive.empty() )
    {
        char nextByte = m_incomingDataQueuedToReceive.front();
        m_incomingDataQueuedToReceive.pop_front();

        if ( nextByte == '\0' )
        {
            break;
        }

        command.push_back( nextByte );
    }

    command += " remote=true";
    g_engine->m_devConsole->Execute( command );
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::Client_BeginFrame()
{
    if ( m_state != NET_STATE_CLIENT_CONNECTING && m_state != NET_STATE_CLIENT_CONNECTED )
    {
        return;
    }

    int    result;

    /*
        CLIENT-SPECIFIC
    */
    fd_set writeSockets;
    fd_set exceptSockets;

    FD_ZERO( &writeSockets );
    FD_ZERO( &exceptSockets );

    FD_SET( m_connectionToServer, &writeSockets );
    FD_SET( m_connectionToServer, &exceptSockets );

    timeval waitTime = {};
    result           = select( 0, NULL, &writeSockets, &exceptSockets, &waitTime );
    if ( result == SOCKET_ERROR )
    {
        int error = ErrorHandling();
        closesocket( m_connectionToServer );
        m_connectionToServer = INVALID_SOCKET;
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, Stringf( "[Error]   select failed. Error: %d", error ) );
        m_state = NET_STATE_IDLE;
        return;
    }
    else if ( result == 0 )
    {
        return;
    }

    if ( FD_ISSET( m_connectionToServer, &exceptSockets ) )
    {
        m_connectionToServer = INVALID_SOCKET;
        g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, "[Error]   Connection failed while connecting." );
        m_state = NET_STATE_IDLE;
        return;
    }

    if ( FD_ISSET( m_connectionToServer, &writeSockets ) )
    {
        if ( m_state == NET_STATE_CLIENT_CONNECTING )
        {
            g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_GREEN, "[Network] Successfully connected to server" );
            m_state = NET_STATE_CLIENT_CONNECTED;
        }
    }

    if ( !m_outgoingDataQueuedToSend.empty() )
    {
        constexpr int SEND_BUFFER_SIZE               = 1200;
        char          sendBuffer[ SEND_BUFFER_SIZE ] = {};
        int           writeSlot                      = 0;
        while ( !m_outgoingDataQueuedToSend.empty() && writeSlot < SEND_BUFFER_SIZE )
        {
            sendBuffer[ writeSlot++ ] = m_outgoingDataQueuedToSend.back();
            m_outgoingDataQueuedToSend.pop_back();
        }
        int numBytesToSend = writeSlot;
        result             = send( m_connectionToServer, sendBuffer, numBytesToSend, 0 );
        if ( result == SOCKET_ERROR )
        {
            int errorCode = ErrorHandling();
            // disconnected? - change state if server disconnected
            g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, Stringf( "[Error]   Failed to send data to client. Error: %d", errorCode ) );
            return;
        }
        else if ( result < numBytesToSend )
        {
            // partial send
        }
        else
        {
            // full send
        }
    }

    constexpr int RECV_BUFFER_SIZE               = 2048;
    char          recvBuffer[ RECV_BUFFER_SIZE ] = {};
    result                                       = recv( m_connectionToServer, recvBuffer, sizeof( recvBuffer ), 0 );

    if ( result > 0 )
    {
        for ( int byteIndex = 0; byteIndex < result; ++byteIndex )
        {
            m_incomingDataQueuedToReceive.push_back( recvBuffer[ byteIndex ] );
        }
    }

    auto nullIter = std::find( m_incomingDataQueuedToReceive.begin(), m_incomingDataQueuedToReceive.end(), '\0' );
    if ( nullIter == m_incomingDataQueuedToReceive.end() )
    {
        return;
    }

    std::string command;
    while ( !m_incomingDataQueuedToReceive.empty() )
    {
        char nextByte = m_incomingDataQueuedToReceive.front();
        m_incomingDataQueuedToReceive.pop_front();

        if ( nextByte == '\0' )
        {
            break;
        }

        command.push_back( nextByte );
    }

    command += " remote=true";
    g_engine->m_devConsole->Execute( command );
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::Server_EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::Client_EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::Disconnect()
{
    if ( m_state == NET_STATE_SERVER_LISTENING )
    {
        if ( m_listenSocket != INVALID_SOCKET )
        {
            closesocket( m_listenSocket );
            m_listenSocket = INVALID_SOCKET;
            g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, "[Network] Server stopped" );
        }

        if ( m_clientSocket != INVALID_SOCKET )
        {
            closesocket( m_clientSocket );
            m_clientSocket = INVALID_SOCKET;
        }

        m_state = NET_STATE_IDLE;
        return;
    }

    if ( m_state == NET_STATE_CLIENT_CONNECTED )
    {
        if ( m_connectionToServer != INVALID_SOCKET )
        {
            closesocket( m_connectionToServer );
            m_connectionToServer = INVALID_SOCKET;
            g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_RED, "[Network] Disconnected from server" );
        }

        m_state = NET_STATE_IDLE;
        return;
    }
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::Shutdown()
{
    /*
        WSACleanup - shutdown
    */

    Disconnect();
    WSACleanup();
    m_state = NET_STATE_INACTIVE;
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::BeginFrame()
{
    Server_BeginFrame();
    Client_BeginFrame();

    UDP_BeginFrame();
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::EndFrame()
{
    Server_EndFrame();
    Client_EndFrame();
}

//-----------------------------------------------------------------------------------------------
int NetworkSystem::ErrorHandling()
{
    return WSAGetLastError();
}

//-----------------------------------------------------------------------------------------------
char const* NetworkSystem::GetNetworkState() const
{
    switch ( m_state )
    {
        case NET_STATE_IDLE:
            {
                return "IDLE";
            }
        case NET_STATE_SERVER_LISTENING:
            {
                return "SERVER_LISTENING";
            }
        case NET_STATE_CLIENT_CONNECTING:
            {
                return "CLIENT_CONNECTING";
            }
        case NET_STATE_CLIENT_CONNECTED:
            {
                return "CLIENT_CONNECTED";
            }
    }

    return "INACTIVE";
}

//-----------------------------------------------------------------------------------------------
std::string NetworkSystem::GetErrorMessage( int errorCode )
{
    char*       errMsgBuffer = nullptr;
    DWORD       errMsgSize   = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                               nullptr,
                                               static_cast< DWORD >( errorCode ),
                                               0,
                                               reinterpret_cast< char* >( &errMsgBuffer ),
                                               0,
                                               nullptr );

    std::string errMsg = "UNKNOWN ERROR";
    if ( errMsgSize > 0 && errMsgBuffer != nullptr )
    {
        errMsg = errMsgBuffer;
        LocalFree( errMsgBuffer );
    }

    return errMsg;
}

//-----------------------------------------------------------------------------------------------
void NetworkSystem::SendStringToAll( std::string commandString )
{
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::Event_StartServer( [[maybe_unused]] EventArgs& args )
{
    if ( !g_engine->m_network )
    {
        return false;
    }
    return g_engine->m_network->StartServer();
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::Event_StartUDPServer( [[maybe_unused]] EventArgs& args )
{
    if ( !g_engine->m_network )
    {
        return false;
    }
    return g_engine->m_network->StartUDP();
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::Event_SendTime( [[maybe_unused]] EventArgs& args )
{
    if ( !g_engine->m_network )
    {
        return false;
    }

    auto        now         = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t( now );
    std::tm     localTime   = {};
    localtime_s( &localTime, &currentTime );
    int         year   = localTime.tm_year + 1900;
    int         month  = localTime.tm_mon + 1;
    int         day    = localTime.tm_mday;
    int         hour   = localTime.tm_hour;
    int         minute = localTime.tm_min;
    int         second = localTime.tm_sec;

    std::string time                   = Stringf( "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second );
    g_engine->m_network->m_testMessage = time;
    return true;
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::Event_StartClient( [[maybe_unused]] EventArgs& args )
{
    if ( !g_engine->m_network )
    {
        return false;
    }
    return g_engine->m_network->StartClient();
}

//-----------------------------------------------------------------------------------------------
bool NetworkSystem::Event_Disconnect( [[maybe_unused]] EventArgs& args )
{
    if ( !g_engine->m_network )
    {
        return false;
    }

    g_engine->m_network->Disconnect();
    return true;
}