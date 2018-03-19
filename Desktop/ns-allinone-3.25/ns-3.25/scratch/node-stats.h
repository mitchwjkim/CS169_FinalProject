// CS 169 - Final Project
// Automated Power and Rate Control in 802.11 Wireless Networks
// I implemented this case based on the code from the power-adaptation-distance.cc file.
//        ns3/examples/wifi/power-adaptation-distance.cc
//

#ifndef NODE_STATS_H
#define NODE_STATS_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/stats-module.h"
#include "ns3/mobility-module.h"

using namespace std;

namespace ns3 {
  class NodeStats {
    public:
      NodeStats (NetDeviceContainer ap, NetDeviceContainer sta);
      void SetupPhy (Ptr<WifiPhy> phy);
      void PowerCallback (std::string path, uint8_t power, Mac48Address dest);
      void RateCallback (std::string path, uint32_t rate, Mac48Address dest);
      void RxCallback (std::string path, Ptr<const Packet> packet, const Address &from);
      void PhyCallback (std::string path, Ptr<const Packet> packet);
      void getStats(Ptr<Node> node, int stepsSize, int stepsTime);
    private:
      typedef vector<pair<Time, WifiMode> > TxTime;
      Time GetCalcTxTime (WifiMode mode);
      
      bool first;
      double m_bitsTotal;
      double totalEnergy;
      std::map<Mac48Address, double> actualPower;
      std::map<Mac48Address, WifiMode> actualMode;
      Ptr<WifiPhy> myPhy;
      TxTime timeTable;
  };

  void NodeStats::SetupPhy (Ptr<WifiPhy> phy) {
    uint32_t nModes = phy->GetNModes ();
  for (uint32_t i = 0; i < nModes; i++) {
    WifiMode mode = phy->GetMode(i);
    WifiTxVector txVector;
    txVector.SetMode (mode);
    timeTable.push_back (make_pair (phy->CalculateTxDuration (1024, txVector, WIFI_PREAMBLE_LONG, phy->GetFrequency()), mode));
  }
}

  NodeStats::NodeStats(NetDeviceContainer ap, NetDeviceContainer sta) {
    Ptr<NetDevice> device = ap.Get(0);
    Ptr<WifiNetDevice> wifiDevice = DynamicCast<WifiNetDevice> (device);
    Ptr<WifiPhy> phy = wifiDevice->GetPhy();
    myPhy = phy;
    SetupPhy(phy);

    first = true;
    m_bitsTotal = 0;
    totalEnergy = 0;
  }

  Time NodeStats::GetCalcTxTime (WifiMode mode) {
    for(TxTime::const_iterator i = timeTable.begin(); i != timeTable.end(); i++) {
      if (mode == i->second) {
        return i->first;
      }
    }
    return Seconds(0);  
  }

  void NodeStats::getStats (Ptr<Node> node, int stepsSize, int stepsTime) {
  // Obtain position of node
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  Vector pos = mobility->GetPosition(); 
 
  //double mbs = ((m_bitsTotal * 1000.) / stepsTime);
  double atp = ((totalEnergy * 1000.) / stepsTime);
  
  // PRINT Statements
  //NS_LOG_UNCOND("Position: " << pos.x << " Throughput: " << mbs << " ATP: " << atp);
  NS_LOG_UNCOND(atp);  
 
  // Reset Variables
  m_bitsTotal = 0;
  totalEnergy = 0;
  first = false;

  // Reposition the node
  pos.x += stepsSize;
  mobility->SetPosition(pos);  

  Simulator::Schedule (MilliSeconds (stepsTime), &NodeStats::getStats, this, node, stepsSize, stepsTime);
}

void NodeStats::PhyCallback (string path, Ptr<const Packet> packet) {
  WifiMacHeader head;
  packet->PeekHeader (head);
  Mac48Address dest = head.GetAddr1 ();

  if (head.GetType() == WIFI_MAC_DATA) {
    totalEnergy += pow(10.0, actualPower[dest] / 10.0) * GetCalcTxTime (actualMode[dest]).GetSeconds();
  }
}

void NodeStats::RxCallback (string path, Ptr<const Packet> packet, const Address &from) {
  m_bitsTotal += ((packet->GetSize() * 8) / double(1000000));
}

  void
NodeStats::PowerCallback (std::string path, uint8_t power, Mac48Address dest)
{
  double   txPowerBaseDbm = myPhy->GetTxPowerStart ();
  double   txPowerEndDbm = myPhy->GetTxPowerEnd ();
  uint32_t nTxPower = myPhy->GetNTxPower ();
  double dbm;
  if(first) {
    dbm = 0;
  }
  else if (nTxPower > 1)
    {
      dbm = txPowerBaseDbm + power * (txPowerEndDbm - txPowerBaseDbm) / (nTxPower - 1);
    }
  else
    {
      dbm = txPowerBaseDbm;
    }
  actualPower[dest] = dbm;
}

void
NodeStats::RateCallback (std::string path, uint32_t rate, Mac48Address dest)
{
  actualMode[dest] = myPhy->GetMode (rate);
}

}

#endif
