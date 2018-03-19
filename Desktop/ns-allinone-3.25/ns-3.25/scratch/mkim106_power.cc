// CS 169 - Final Project
// Automated Power and Rate Control in 802.11 Wireless Networks
// I implemented this case based on the code from the power-adaptation-distance.cc file.
//        ns3/examples/wifi/power-adaptation-distance.cc
//

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/stats-module.h"

#include "ns3/mparf-wifi-manager.h"
#include "scratch/node-stats.h"

using namespace std;
using namespace ns3;

int main (int argc, char *argv[]) {
  // VARIABLES
  double maxPower = 30;			// TxPowerEnd
  double minPower = 0;			// TxPowerStart
  uint32_t powerLvl = 31;		// PowerLevels

  uint32_t rtsThreshold = 2400;		// Set to a value above 1000 to disable RTS/CTS
  
  // AP & MS Coordinates
  // AP is statically located at (0, 0) 
  int ap_x = 0;
  int ap_y = 0;

  // MS is initially located at (5, 0)
  int sta_x = 5;
  int sta_y = 0;

  // STEPS Information
  uint32_t numberOfsteps = 100;		// Number of Steps
  uint32_t stepsSize = 1;		// Power Step Size
  uint32_t stepsTime = 100;		// 100 milliseconds

  CommandLine cmd;
  cmd.AddValue("rtsThreshold", "RTS threshold", rtsThreshold);
  cmd.AddValue("numberOfsteps", "amount of steps", numberOfsteps);
  cmd.AddValue("stepsTime", "Time on each step", stepsTime);
  cmd.AddValue("stepsSize", "Distance between steps", stepsSize);
  cmd.AddValue("maxPower", "Maximum available transmission level (dbm).", maxPower);
  cmd.AddValue("minPower", "Minimum available transmission level (dbm).", minPower);
  cmd.AddValue("powerLvl", "Number of transmission power levels avialable", powerLvl);
  
  cmd.Parse(argc, argv);

  // SAFETY Checkpoint - numberOfsteps
  if (numberOfsteps == 0 ) {
    cout << "Exiting without running simulation; Number of Steps is 0.\n";
    return 1;
   }

  // Set the Simulation Time
  uint32_t simuTime = numberOfsteps * stepsTime;

  // Define the AP
  NodeContainer wifiApNodes;
  wifiApNodes.Create(1);

  // Define the STA
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create(1);

  // Set up the wifi network
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel(wifiChannel.Create());

  NetDeviceContainer wifiApDevices, wifiStaDevices, wifiDevices;

  // Configuring the nodes
  // Configuring the STA node. MinstrelWifiManager for the MS with TxPowerStart = 30 and TxPowerEnd = 30
  wifi.SetRemoteStationManager("ns3::MinstrelWifiManager", "RtsCtsThreshold", UintegerValue (rtsThreshold));
  wifiPhy.Set("TxPowerStart", DoubleValue (30.));
  wifiPhy.Set("TxPowerEnd", DoubleValue(30.));

  // SSID
  Ssid ssid = Ssid("AP");
  wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue(false));
  wifiStaDevices.Add(wifi.Install (wifiPhy, wifiMac, wifiStaNodes.Get(0)));

  // Configuring the AP node. MparfWifiManager for the AP with TxPowerStart = 0, TxPowerEnd = 30, TxPowerLevels = 31, and PowerStepSize = pStepSize
  wifi.SetRemoteStationManager("ns3::MparfWifiManager", "DefaultTxPowerLevel", UintegerValue(30), "RtsCtsThreshold", UintegerValue (rtsThreshold));
  wifiPhy.Set("TxPowerStart", DoubleValue (0.));
  wifiPhy.Set("TxPowerEnd", DoubleValue (30.));
  wifiPhy.Set("TxPowerLevels", UintegerValue (31));

  // SSID
  ssid = Ssid("AP");
  wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
  wifiApDevices.Add (wifi.Install (wifiPhy, wifiMac, wifiApNodes.Get(0)));
  
  // Add DEVICES
  wifiDevices.Add (wifiStaDevices);
  wifiDevices.Add (wifiApDevices);

  // Configure the mobility
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  
  // Initial Position of AP and STA
  positionAlloc->Add (Vector (ap_x, ap_y, 0.0));
  NS_LOG_UNCOND ("Setting initial AP position to " << Vector (ap_x, ap_y, 0.0));
  
  positionAlloc->Add (Vector (sta_x, sta_y, 0.0));
  NS_LOG_UNCOND ("Setting initial STA position to " << Vector (sta_x, sta_y, 0.0));

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNodes.Get (0));
  mobility.Install (wifiStaNodes.Get(0));

  NodeStats stats = NodeStats (wifiApDevices, wifiStaDevices);

  Simulator::Schedule (MilliSeconds (stepsTime), &NodeStats::getStats, &stats, wifiStaNodes.Get(0), stepsSize, stepsTime);

  // Configuring the IP stack
  InternetStackHelper stack;
  stack.Install (wifiApNodes);
  stack.Install (wifiStaNodes);

  // Address
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = address.Assign (wifiDevices);
  
  // Packet Sink Helper
  Ipv4Address sinkAddress = i.GetAddress (0);
  uint16_t port = 9;

  PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress(sinkAddress, port));
  ApplicationContainer apps_sink = sink.Install (wifiStaNodes.Get (0));

  // On Off Helper
  OnOffHelper onoff ("ns3::UdpSocketFactory", InetSocketAddress (sinkAddress, port));
  onoff.SetConstantRate (DataRate ("54Mb/s"), 1024);
  onoff.SetAttribute ("StartTime", TimeValue (MilliSeconds (0.5)));
  onoff.SetAttribute ("StopTime", TimeValue (MilliSeconds (simuTime)));
  ApplicationContainer apps_source = onoff.Install (wifiApNodes.Get (0));

  apps_sink.Start (MilliSeconds (0.5));
  apps_sink.Stop (MilliSeconds (simuTime)); 

  // Collect Data. Call necessary functions to obtain statistics ---------------------------------
  Config::Connect("/NodeList/1/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&NodeStats::RxCallback, &stats));
  
  Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/$ns3::MparfWifiManager/PowerChange", MakeCallback(&NodeStats::PowerCallback, &stats));
  Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/$ns3::MparfWifiManager/RateChange", MakeCallback(&NodeStats::RateCallback, &stats));

  Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin", MakeCallback(&NodeStats::PhyCallback, &stats));


  Simulator::Stop (MilliSeconds(simuTime));
  Simulator::Run();

  Simulator::Destroy();
  
  return 0;

}
