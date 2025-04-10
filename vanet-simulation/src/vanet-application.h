#ifndef VANET_APPLICATION_H
#define VANET_APPLICATION_H

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/address.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/event-id.h"
#include "ns3/timer.h"

namespace ns3 {

class VanetApplication : public Application
{
public:
    VanetApplication();
    virtual ~VanetApplication();

    void StartApplication() override; // Called when the application starts
    void StopApplication() override;  // Called when the application stops

private:
    void SetupUdpCommunication();
    void SendPacket();
    void ReceivePacket(Ptr<Socket> socket);

    Ptr<Socket> m_socket; // Socket for UDP communication
    Address m_peerAddress; // Address of the peer node
    EventId m_sendEvent; // Event for sending packets
    Timer m_timer; // Timer for periodic sending
};

} // namespace ns3

#endif // VANET_APPLICATION_H