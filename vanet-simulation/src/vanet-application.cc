#include "vanet-application.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/packet.h"
#include "ns3/log.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("VANETApplication");

VANETApplication::VANETApplication() : m_socket(0), m_peerAddress(), m_running(false) {}

VANETApplication::~VANETApplication() {
    if (m_socket) {
        m_socket->Close();
    }
}

void VANETApplication::StartApplication() {
    m_running = true;
    m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
    m_socket->Bind();
    m_socket->SetRecvCallback(MakeCallback(&VANETApplication::ReceivePacket, this));
}

void VANETApplication::StopApplication() {
    m_running = false;
    if (m_socket) {
        m_socket->Close();
    }
}

void VANETApplication::SendPacket() {
    if (m_running) {
        Ptr<Packet> packet = Create<Packet>(1024); // Create a packet of 1024 bytes
        m_socket->SendTo(packet, 0, m_peerAddress);
    }
}

void VANETApplication::ReceivePacket(Ptr<Socket> socket) {
    while (socket->Recv()) {
        NS_LOG_INFO("Packet received");
    }
}

void VANETApplication::SetPeerAddress(Ipv4Address address) {
    m_peerAddress = InetSocketAddress(address, 9); // Port 9 for UDP
}