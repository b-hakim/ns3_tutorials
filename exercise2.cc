/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  if (argc != 3)
  {
    cout<<"Please enter the DataRate and the Delay values!" << endl;
    return 0;
  }

  string dataRate = argv[1];
  dataRate += "Mbps";
  string delay = argv[2];
  delay += "ms";

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  NodeContainer nodes2;
  nodes2.Add(nodes.Get (1));
  nodes2.Create (1);

  /////////////////////////////////////////////////////////////////////////////////////////////////

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (dataRate));
  pointToPoint.SetChannelAttribute ("Delay", StringValue (delay));

  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("6Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("3ms"));

  /////////////////////////////////////////////////////////////////////////////////////////////////

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  NetDeviceContainer devices2;
  devices2 = pointToPoint2.Install (nodes2);

  /////////////////////////////////////////////////////////////////////////////////////////////////

  InternetStackHelper stack;
  stack.Install (nodes);
  stack.Install (nodes2.Get(1));

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4AddressHelper address2;
  address2.SetBase ("10.1.2.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);

  /////////////////////////////////////////////////////////////////////////////////////////////////

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  OnOffHelper onOffServer("ns3::UdpSocketFactory",
                           Address(InetSocketAddress(Ipv4Address ("10.1.2.0"), 9)));
  onOffServer.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffServer.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  ApplicationContainer serverApps2 = onOffServer.Install (nodes2.Get (1));
  serverApps2.Start (Seconds (1.0));
  serverApps2.Stop (Seconds (10.0));

  /////////////////////////////////////////////////////////////////////////////////////////////////

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient2 (interfaces2.GetAddress (1), 9);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps2 = echoClient2.Install (nodes2.Get (0));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}