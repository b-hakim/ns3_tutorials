////
//// Created by bassel on 05/08/22.
////
//
//#include "ns3/core-module.h"
//#include "ns3/network-module.h"
//#include "ns3/applications-module.h"
//#include "ns3/wifi-mode.h"
//#include "ns3/mobility-model.h"
//#include "ns3/internet-module.h"
//#include "ns3/aodv-module.h"
//#include "ns3/yans-wifi-helper.h"
//#include "ns3/wifi-helper.h"
//#include "ns3/nqos-wifi-mac-helper.h"
//#include "ns3/mobility-module.h"
//
//#include "ns3/ssid.h"
//// Default Network Topology
////
////       10.1.1.0
//// n0 -------------- n1
////    point-to-point
////
//
//using namespace ns3;
//using namespace std;
//
//NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");
//
//int
//main (int argc, char *argv[])
//{
//  Time::SetResolution (Time::NS);
//  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
//  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
//
//  /////////////////////////      Creating Nodes      ///////////////////////////////////////////////
//
//  NodeContainer gridNodes;
//  gridNodes.Create (36);
//
//  NodeContainer centerNodes;
//  centerNodes.Create (50);
//
//  /////////////////////////       Creating devices       ///////////////////////////////////////////
//  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
//  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
//  phy.SetChannel (channel.Create ());
//
//  WifiHelper wifi;
//  wifi.SetRemoteStationManager ("ns3::AarfWifiManager"); // for the rate control algorithm
//  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();
//
//  Ssid ssid = Ssid ("ns-3-ssid");
//
////  mac.SetType ("ns3::StaWifiMac",
////  "Ssid", SsidValue (ssid),
////  "ActiveProbing", BooleanValue (false));
//
////  mac.SetType ("ns3::AdhocWifiMac");
//  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
//
//  NetDeviceContainer wifiDevices;
//  wifiDevices = wifi.Install (phy, mac, gridNodes);
//  wifiDevices.Add (wifi.Install (phy, mac, centerNodes));
//
//  ///////////////////////////       Nodes Mobilities        ///////////////////////////////////////
//  MobilityHelper centerMobility;
//  centerMobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
//	  "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=60.0]"),
//	  "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=60.0]"));
//
//  Ptr<RandomRectanglePositionAllocator> r = CreateObject<RandomRectanglePositionAllocator>();
//  r->SetAttribute("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"));
//  r->SetAttribute("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"));
//
//  centerMobility.SetMobilityModel("ns3::RandomWaypointMobilityModel",
//		  "Speed", StringValue("ns3::UniformRandomVariable[Min=10.0|Max=20.0]"),
//		  "Pause", StringValue("ns3::ConstantRandomVariable[Constant=2.0]"),
//		  "PositionAllocator", PointerValue(r));
//
//  centerMobility.Install(centerNodes);
//
//
//  MobilityHelper gridMobility;
//  gridMobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(0), "MinY", DoubleValue(0),
//		  "DeltaX", DoubleValue(10), "DeltaY", DoubleValue(10), "GridWidth", UintegerValue(6), "LayoutType", StringValue("RowFirst"));
//
//  gridMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//
//  gridMobility.Install (gridNodes);
//
//  ///////////////////////////         Install Internet       ///////////////////////////////////////
//
//  InternetStackHelper stack;
//  AodvHelper aodv;
//  stack.SetRoutingHelper (aodv);
//  stack.Install (gridNodes);
//  stack.Install (centerNodes);
//
//  Ipv4AddressHelper address;
//  address.SetBase ("10.1.1.0", "255.255.255.0");
//
//  Ipv4InterfaceContainer interfaces = address.Assign (wifiDevices);
//
//  ///////////////////////////    Install Applciation Server      ///////////////////////////////////
//
//  UdpEchoServerHelper echoServer (9);
//
//  ApplicationContainer serverApps = echoServer.Install (gridNodes.Get(35));
//  serverApps.Start (Seconds (1.0));
//  serverApps.Stop (Seconds (10.0));
//
//  ///////////////////////////    Install Application Client      ///////////////////////////////////
//
//  UdpEchoClientHelper echoClient (interfaces.GetAddress (35), 9);
//  echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
//  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
//  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
//
//  for(int i=0; i<35; i++)
//  {
//	  ApplicationContainer clientApps = echoClient.Install (gridNodes.Get (i));
//	  clientApps.Start (Seconds (1.0));
//	  clientApps.Stop (Seconds (9.5));
//  }
//  //////////////////////////////////       Run Simulation      /////////////////////////////////////
//
//  Simulator::Stop(Seconds(10));
//
//  Simulator::Run ();
//  Simulator::Destroy ();
//
//  cout << "Simulation Done!" << endl;
//
//  return 0;
//}
