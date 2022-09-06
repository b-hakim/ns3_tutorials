//
// Created by bassel on 05/08/22.
//

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-mode.h"
#include "ns3/mobility-model.h"
#include "ns3/internet-module.h"
#include "ns3/aodv-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

#include "ns3/ssid.h"
// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

class MySenderApp : public Application
{
public:
    MySenderApp();
    virtual ~MySenderApp();

    void Setup(Ipv4Address remIP, uint16_t port,uint32_t packetSize, DataRate dataRate);
    void Setup(Ipv4Address remIP, uint16_t port,uint32_t packetSize, uint16_t maxDelay);
    void Setup(Ipv4Address remIP, uint16_t port,uint32_t packetSize, string filename);

    virtual void StartApplication(void) override;
    virtual void StopApplication(void) override;

private:
    void ScheduleTx(void);
    void SendPacket(void);

    Ptr<Socket> m_socket;
    Address m_peer;
    uint32_t m_packetSize;

    EventId m_sendEvent;
    bool m_running;
    uint32_t m_packetsSent;
    Ipv4Address m_remoteAddress;
    uint16_t m_portNum;
    uint8_t m_mode; // 0: Data Rate   1. Random Time    2: files
    DataRate m_dataRate;
    uint16_t m_maxDelay;
    std::string m_filename;

    Ptr<UniformRandomVariable> m_rn;
};

MySenderApp::MySenderApp ()
: m_socket(0), m_peer(), m_packetSize(1000), m_sendEvent(),m_running(false), m_packetsSent(0),
      m_maxDelay(0), m_filename(""), m_rn()
{
}

MySenderApp::~MySenderApp ()
{
    m_socket = 0;
}

void MySenderApp::Setup (ns3::Ipv4Address remIP, uint16_t port, uint32_t packetSize, DataRate dataRate)
{
    m_packetsSent = packetSize;
    m_remoteAddress = remIP;
    m_portNum = port;

    m_dataRate = dataRate;
    m_mode=0;
}

void MySenderApp::Setup (ns3::Ipv4Address remIP, uint16_t port, uint32_t packetSize, uint16_t maxDelay)
{
    m_packetsSent = packetSize;
    m_remoteAddress = remIP;
    m_portNum = port;

    m_maxDelay = maxDelay;
    m_mode=1;
}

void MySenderApp::Setup (ns3::Ipv4Address remIP, uint16_t port, uint32_t packetSize, std::string filename)
{
    m_packetsSent = packetSize;
    m_remoteAddress = remIP;
    m_portNum = port;

    m_filename = filename;
    m_mode = 2;

    if (m_filename != "")
    {
        ifstream timesFile (m_filename);
        double time;
        Time tNext;

        while(timesFile>>time)
        {
            tNext = Seconds(time);
            m_sendEvent = Simulator::Schedule(tNext, &MySenderApp::SendPacket, this);
        }
    }
    else
    {
        NS_LOG_UNCOND("The filename is empty!");
    }
}

void MySenderApp::StartApplication()
{
    if (m_socket == 0)
    {
        m_socket= Socket::CreateSocket(this->GetNode(), UdpSocketFactory::GetTypeId());
        m_peer = InetSocketAddress(m_remoteAddress, m_portNum);
        m_socket->Bind();
        m_socket->Connect(m_peer);
    }

    m_running = true;
    SendPacket();
}

void MySenderApp::SendPacket()
{
    if (m_socket==0)
        return;

    Ptr<Packet> packet = CreateObject<Packet>(m_packetSize);
    m_socket->Send(packet);
    NS_LOG_UNCOND("At time = " << Simulator::Now().GetSeconds() << "MySenderApp sent a packet.");

    if (m_running)
    {
        Time tNext;

        if (m_mode == 0)
            tNext = Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate()));
        else if(m_mode == 1)
            tNext = MilliSeconds(m_rn->GetInteger(0, m_maxDelay));
        else if (m_mode == 2)
            return;

        m_sendEvent = Simulator::Schedule(tNext, &MySenderApp::SendPacket, this);
    }

}

int main (int argc, char *argv[])
{
    printf("1");
    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    /////////////////////////      Reading Input      ///////////////////////////////////////////////
    uint32_t nodes;
    float start_power_level;
    float end_power_level;
    uint32_t seed;

    CommandLine cmd;

    cmd.AddValue ("Nodes", "Number of nodes", nodes);
    cmd.AddValue ("StartPowerLevel", "Start Power Level", start_power_level);
    cmd.AddValue ("EndPowerLevel", "End Power Level", end_power_level);
    cmd.AddValue ("Seed", "Seed number", seed);
    printf("hi");
    cmd.Parse (argc, argv);

    RngSeedManager::SetSeed(seed);

    /////////////////////////      Creating Nodes      ///////////////////////////////////////////////

    NodeContainer cornerNodes;
    cornerNodes.Create (2);

    NodeContainer centerNodes;
    centerNodes.Create (nodes);

    /////////////////////////       Creating devices       ///////////////////////////////////////////
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
    YansWifiPhyHelper phy;
    phy.SetChannel (channel.Create ());

    phy.Set("TxPowerLevels", UintegerValue(3));
    phy.Set("TxPowerStart", DoubleValue(start_power_level));
    phy.Set("TxPowerEnd", DoubleValue(end_power_level));

    WifiHelper wifi;
    WifiMacHelper mac;
    mac.SetType ("ns3::AdhocWifiMac");

    NetDeviceContainer wifiDevices;
    wifiDevices = wifi.Install (phy, mac, cornerNodes);
    wifiDevices.Add (wifi.Install (phy, mac, centerNodes));

    ///////////////////////////       Nodes Mobilities        ///////////////////////////////////////
    MobilityHelper centerMobility;
    centerMobility.SetPositionAllocator (
      "ns3::RandomRectanglePositionAllocator", "X",
      StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"), "Y",
      StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"));
    centerMobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Bounds",
                                   RectangleValue (Rectangle (0, 100, 0, 100)), "Speed",
                                   StringValue ("ns3::UniformRandomVariable[Min=10.0|Max=20.0]"));
    centerMobility.Install (centerNodes);

    MobilityHelper cornerMobility;
    Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator> ();
    pos->Add (Vector (0, 0, 0));
    pos->Add (Vector (100, 100, 0));
    cornerMobility.SetPositionAllocator (pos);
    cornerMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    cornerMobility.Install (cornerNodes);

    ///////////////////////////         Install Internet       ///////////////////////////////////////

    InternetStackHelper stack;
    AodvHelper aodv;
    stack.SetRoutingHelper (aodv);
    stack.Install (cornerNodes);
    stack.Install (centerNodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign (wifiDevices);

    ///////////////////////////    Install Applciation Server      ///////////////////////////////////

//    UdpEchoServerHelper echoServer (9);
//
//    ApplicationContainer serverApps = echoServer.Install (cornerNodes.Get (1));
//    serverApps.Start (Seconds (1.0));
//    serverApps.Stop (Seconds (10.0));

    ///////////////////////////    Install Application Client      ///////////////////////////////////

//    UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
//    echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
//    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
//    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
//
//    Ptr<UniformRandomVariable> rn = CreateObject<UniformRandomVariable>();
//    uint8_t t = rn->GetInteger(5, 20);
//
//    ApplicationContainer clientApps = echoClient.Install (cornerNodes.Get (0));
//    clientApps.Start (Seconds (t));
//    clientApps.Stop (Seconds (60));

//    NS_LOG_UNCOND ("t = " << int(t));

    Ptr<Ipv4> ipv4 = cornerNodes.Get(1)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
    Ipv4Address ipAddress = iaddr.GetLocal();
    int sinkPort = 2021;

    Ptr<MySenderApp> app = CreateObject<MySenderApp>();
    int pktSize = 1000;

    app->Setup(ipAddress, sinkPort, pktSize, "SentTimes.txt");

    centerNodes.Get(0)->AddApplication(app);
    app->SetStartTime(Seconds(1));

    //////////////////////////////////       Run Simulation      /////////////////////////////////////
//    AnimationInterface anim("/home/bassel/eclipse-workspace/ns-allinone-3.36.1/ns-3.36.1/animation.xml");
//    anim.SetStartTime (Seconds (1));
//    anim.SetStopTime (Seconds(60));

//    phy.EnablePcap("PCAP/wifiRM", false);

    Simulator::Stop(Seconds(10));

    Simulator::Run ();
    Simulator::Destroy ();
    cout <<" done \n";
    return 0;
}