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
	CommandLine cmd;
	uint32_t nPackets = 1;
	cmd.AddValue("nPackets", "Number of packets to echo", nPackets);
	cmd.Parse(argc, argv);
	
  // Set time resolution
  Time::SetResolution (Time::NS);
  // Enable log components and set log levels
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Topology ----------------------------------------------------------
  // Create nodes
  NS_LOG_INFO("Creating Topology");
  NodeContainer nodes;
  nodes.Create (2);

  // Create Channel
  PointToPointHelper pointToPoint;
  //pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  //pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
/*  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  PointToPointHelper pointToPoint3;
  pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint3.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  PointToPointHelper pointToPoint4;
  pointToPoint4.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint4.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  PointToPointHelper pointToPoint5;
  pointToPoint5.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint5.SetChannelAttribute ("Delay", StringValue ("2ms"));
  */

  // Create NetDevice and bind them to Channel
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes.Get(0), nodes.Get(1));
/*  NetDeviceContainer devices2;
  devices2 = pointToPoint2.Install (nodes.Get(0), nodes.Get(2));
  NetDeviceContainer devices3;
  devices3 = pointToPoint3.Install (nodes.Get(0), nodes.Get(3));
  NetDeviceContainer devices4;
  devices4 = pointToPoint4.Install (nodes.Get(0), nodes.Get(4));
  NetDeviceContainer devices5;
  devices5 = pointToPoint5.Install (nodes.Get(0), nodes.Get(5));
  */
  // -------------------------------------------------------------------

  // Protocal Stack ----------------------------------------------------
  // Create InternetStack
  InternetStackHelper stack;
  stack.Install (nodes);

  // Set IP network address
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  // Assign IP to NetDevice
  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  address.NewNetwork();
/*  Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);
  address.NewNetwork();
  Ipv4InterfaceContainer interfaces3 = address.Assign (devices3);
  address.NewNetwork();
  Ipv4InterfaceContainer interfaces4 = address.Assign (devices4);
  address.NewNetwork();
  Ipv4InterfaceContainer interfaces5 = address.Assign (devices5);
  address.NewNetwork();
  */
  // -------------------------------------------------------------------

  // Building UDP Echo Server ------------------------------------------
  // Create echo server
  UdpEchoServerHelper echoServer (9);

  // Install echo server to node 1, mark it application, and set start and stop time
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  // -------------------------------------------------------------------

  // Building UDP Echo Client ------------------------------------------
  // Create echo client and set its attributes
  UdpEchoClientHelper echoClient (interfaces.GetAddress (0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (nPackets));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (65507));

 /* UdpEchoClientHelper echoClient2 (interfaces2.GetAddress (0), 9);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (4));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (65507));
  
  UdpEchoClientHelper echoClient3 (interfaces3.GetAddress (0), 9);
  echoClient3.SetAttribute ("MaxPackets", UintegerValue (4));
  echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient3.SetAttribute ("PacketSize", UintegerValue (65507));
  
  UdpEchoClientHelper echoClient4 (interfaces4.GetAddress (0), 9);
  echoClient4.SetAttribute ("MaxPackets", UintegerValue (4));
  echoClient4.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient4.SetAttribute ("PacketSize", UintegerValue (65507));
  
  UdpEchoClientHelper echoClient5 (interfaces5.GetAddress (0), 9);
  echoClient5.SetAttribute ("MaxPackets", UintegerValue (4));
  echoClient5.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient5.SetAttribute ("PacketSize", UintegerValue (65507));
*/

  // Install echo client to node 0, mark it application, and set start and stop time
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

/*  ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get(2));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));
  
  ApplicationContainer clientApps3 = echoClient3.Install (nodes.Get(3));
  clientApps3.Start (Seconds (2.0));
  clientApps3.Stop (Seconds (10.0));
  
  ApplicationContainer clientApps4 = echoClient4.Install (nodes.Get(4));
  clientApps4.Start (Seconds (2.0));
  clientApps4.Stop (Seconds (10.0));
  
  ApplicationContainer clientApps5 = echoClient5.Install (nodes.Get(5));
  clientApps5.Start (Seconds (2.0));
  clientApps5.Stop (Seconds (10.0));
  */
  
  // --------------------------------------------------------------------

  // ASCII Tracing ------------------------------------------------------
  
  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream ("myfirst.tr"));
  pointToPoint.EnablePcapAll("myfirst");
  
  // --------------------------------------------------------------------

  // Run simulator ------------------------------------------------------
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

