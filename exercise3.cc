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
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  /////////////////////////      Creating Nodes      ///////////////////////////////////////////////

  NodeContainer cornerNodes;
  cornerNodes.Create (2);

  NodeContainer centerNodes;
  centerNodes.Create (20);

  /////////////////////////       Creating devices       ///////////////////////////////////////////
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());

  phy.Set("TxPowerLevels", UintegerValue(3));
  phy.Set("TxPowerStart", DoubleValue(10));
  phy.Set("TxPowerEnd", DoubleValue(30));

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

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (cornerNodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  ///////////////////////////    Install Application Client      ///////////////////////////////////

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (cornerNodes.Get (0));
  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (10));

  //////////////////////////////////       Run Simulation      /////////////////////////////////////
  AnimationInterface anim("/home/bassel/eclipse-workspace/ns-allinone-3.36.1/ns-3.36.1/animation.xml");
  anim.SetStartTime (Seconds (1));
  anim.SetStopTime (Seconds(10));

  Simulator::Stop(Seconds(20));

  Simulator::Run ();
  Simulator::Destroy ();
  cout <<" done \n";
  return 0;
}
