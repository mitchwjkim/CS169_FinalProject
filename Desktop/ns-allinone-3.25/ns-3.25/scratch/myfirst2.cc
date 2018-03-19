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

using namespace ns3;

// Create log components
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  // Set time resolution
  Time::SetResolution (Time::NS);
  // Enable log components and set log levels
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Topology ----------------------------------------------------------
  // Create nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Create Channel
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("4ms"));
  
  //PointToPointHeper pointToPoint2;
  //pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  //pointToPoint2.SetChannelAttribute ("Delay", StringValue ("4ms"));

  // Create NetDevice and bind them to Channel
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes.Get(0), nodes.Get(2));
  //NetDeviceContainer devices2;
  //devices2 = pointToPoint2.Install (nodes.Get(1), nodes.Get(2));
  // -------------------------------------------------------------------

  // Protocal Stack ----------------------------------------------------
  // Create InternetStack
  InternetStackHelper stack;
  stack.Install (nodes);

  // Set IP network address
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  // Ipv4AddressHelper address2;
  // address2.SetBase ("10.1.1.0", "255.255.255.0");

  // Assign IP to NetDevice
  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  //Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);
  // -------------------------------------------------------------------

  // Building UDP Echo Server ------------------------------------------
  // Create echo server
  UdpEchoServerHelper echoServer (9);

  // Install echo server to node 1, mark it application, and set start and stop time
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  // -------------------------------------------------------------------

  // Building UDP Echo Client ------------------------------------------
  // Create echo client and set its attributes
  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (4));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (65507));

  //UdpEchoClientHelper echoClient2 (interfaces2.GetAddress (2), 9);
  //echoClient2.SetAttribute ("MaxPackets", UintegerValue (4));
  //echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  //echoClient2.SetAttribute ("PacketSize", UintegerValue (65507));

  // Install echo client to node 0, mark it application, and set start and stop time
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  //ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get(1));
  //clientApps2.Start (Seconds (2.0));
  //clientApps2.Stop (Seconds (10.0));
  // --------------------------------------------------------------------

  // Run simulator ------------------------------------------------------
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

