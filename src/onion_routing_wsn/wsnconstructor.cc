
/*
* Copyright (c) 2020 DLTLT 
*
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
*
* Corresponding author: Niki Hrovatin <niki.hrovatin@famnit.upr.si>
*/

#include "wsnconstructor.h"

// Network topolgy -> set of wireless devices
//
//
//    Wireless - adhoc network
//            10.1.1.0
//
//        ((*))  ((*))  ((*))
//
//
//

NS_OBJECT_ENSURE_REGISTERED (WsnConstructor);

NS_LOG_COMPONENT_DEFINE ("wsnconstructor");

TypeId
WsnConstructor::GetTypeId ()
{
  static TypeId tid =
      TypeId ("ns3::WsnConstructor")
          .SetParent<Object> ()
          .AddConstructor<WsnConstructor> ()
          .AddAttribute ("SimulationSeed", "Seed value of the simulation",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue ((uint32_t) 0),
                         MakeUintegerAccessor (&WsnConstructor::m_simulationSeed),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("SimulationName", "Name of the simulation", StringValue ("default"),
                         MakeStringAccessor (&WsnConstructor::m_simulationName),
                         MakeStringChecker ())
          .AddAttribute ("Topology", "Network topology (random disc topology OR grid topology)",
                         EnumValue (Topology::GRID), MakeEnumAccessor (&WsnConstructor::m_topology),
                         MakeEnumChecker (Topology::GRID, "grid", Topology::DISC, "disc"))
          .AddAttribute (
              "IEEE80211n_carrier", "Carrier frequency of the IEEE 802.11n",
              EnumValue (IEEE_80211n::F_24GHz), MakeEnumAccessor (&WsnConstructor::m_mac),
              MakeEnumChecker (IEEE_80211n::F_24GHz, "2_4GHz", IEEE_80211n::F_5GHz, "5GHz"))
          .AddAttribute ("Routing", "Routing algorithm for wireless multi-hop networks",
                         EnumValue (Routing::OLSR), MakeEnumAccessor (&WsnConstructor::m_routing),
                         MakeEnumChecker (Routing::AODV, "aodv", Routing::DSR, "dsr", Routing::OLSR,
                                          "olsr", Routing::DSDV, "dsdv"))
          .AddAttribute ("NodeNumber", "Number of sensor nodes in the network",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue ((uint16_t) 50),
                         MakeUintegerAccessor (&WsnConstructor::m_numNodes),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("Radius", "Parameter for the setup of the random disc topology",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue ((uint16_t) 15),
                         MakeUintegerAccessor (&WsnConstructor::m_radius),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("CellSide",
                         "The length of a cell side in the grid topology. The length is "
                         "representing the distance between neighbouring nodes on the cardinal "
                         "directions of the reference node.",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue ((uint16_t) 20),
                         MakeUintegerAccessor (&WsnConstructor::m_cellSide),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("Verbosity", "Verbosity of the simulation", EnumValue (Verbosity::Both),
                         MakeEnumAccessor (&WsnConstructor::m_verbosity),
                         MakeEnumChecker (Verbosity::NO, "no", Verbosity::ConsoleLog, "consoleLog",
                                          Verbosity::PrintDescription, "description",
                                          Verbosity::Both, "both"))
          .AddAttribute ("MSS", "Maximum segment size",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue (536), MakeUintegerAccessor (&WsnConstructor::m_mss),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute (
              "CommOverhead", "Collect statistics about the communication overhead",
              EnumValue (CommunicationStatistics::Y), MakeEnumAccessor (&WsnConstructor::m_stats),
              MakeEnumChecker (CommunicationStatistics::N, "n", CommunicationStatistics::Y, "y"))
          .AddAttribute (
              "Paths",
              "String of values delimited by the symbol \",\" each value representing the number "
              "of hops the onion will travel to return back to the sink node issuer of the onion.",
              StringValue ("5,10,20,30,40,50"),
              MakeStringAccessor (&WsnConstructor::m_pathsLengths), MakeStringChecker ())
          .AddAttribute ("RepeatePaths",
                         "Integer specifying the number of times to generate the onion message for "
                         "each value of the parameter Paths",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue ((uint16_t) 1),
                         MakeUintegerAccessor (&WsnConstructor::m_onionRepeate),
                         MakeUintegerChecker<uint16_t> ());
  return tid;
}

//constructor
WsnConstructor::WsnConstructor ()
{
}

void
WsnConstructor::Configure ()
{
  //seed cannot be zero
  RngSeedManager::SetSeed (m_simulationSeed + 1); //random seeder

  switch (m_routing)
    {
    case Routing::AODV:
      m_simulationName = m_simulationName + "_AODV";
      break;
    case Routing::DSR:
      m_simulationName = m_simulationName + "_DSR";
      break;
    case Routing::OLSR:
      m_simulationName = m_simulationName + "_OLSR";
      break;
    case Routing::DSDV:
      m_simulationName = m_simulationName + "_DSDV";
      break;
    }

  switch (m_topology)
    {
    case Topology::GRID:
      m_simulationName = m_simulationName + "_GRID_";
      break;
    case Topology::DISC:
      m_simulationName = m_simulationName + "_DISC_";
      break;
    }

  //set the maximum segment size of the network
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (m_mss));
  Config::SetDefault ("ns3::Wsn_node::MSS", UintegerValue (m_mss));

  m_simulationName =
      m_simulationName + std::to_string (m_numNodes) + "_" + std::to_string (m_simulationSeed);

  m_simulationDescription = m_simulationDescription + "Simulation name: " + m_simulationName + "\n";

  int run = RngSeedManager::GetRun ();
  int seed = RngSeedManager::GetRun ();
  m_simulationDescription = m_simulationDescription +
                            "Simulation randomstream setup, run: " + std::to_string (run) +
                            ", seed:" + std::to_string (seed) + "\n";

  m_onionValidator = CreateObject<OnionValidator> ();

  //verbosity of the simulation
  if (m_verbosity == Verbosity::Both || m_verbosity == Verbosity::ConsoleLog)
    {
      LogComponentEnable ("wsnconstructor", LOG_LEVEL_INFO);
      LogComponentEnable ("sink", LOG_LEVEL_INFO);
      LogComponentEnable ("sensornode", LOG_LEVEL_INFO);
      LogComponentEnable ("outputmanager", LOG_LEVEL_INFO);
      LogComponentEnable ("onionrouting", LOG_LEVEL_INFO);
    }
  if (m_verbosity == Verbosity::Both || m_verbosity == Verbosity::PrintDescription)
    {
      m_outputManager = CreateObject<OutputManager> (m_simulationName, m_simulationSeed, m_numNodes,
                                                     m_topology, m_routing, true);
    }
  else
    {
      m_outputManager = CreateObject<OutputManager> (m_simulationName, m_simulationSeed, m_numNodes,
                                                     m_topology, m_routing, false);
    }
  m_outputManager->CreateOutputFile ();
}

/**
 * Run the simulation, showing the progress of it
 */
void
WsnConstructor::Run ()
{
  ProcessPathString ();

  CreateNodes ();
  CreateDevices ();

  switch (m_topology)
    {
    case Topology::GRID:
      BuildGridTopology ();
      break;
    case Topology::DISC:
      BuildDiscTopology ();
      break;
    }

  InstallInternetStack ();
  InstallApplications ();

  if (CommunicationStatistics::Y == m_stats)
    {
      CaptureStatistics ();
    }

  time_t givemetime = time (NULL);
  m_simulationDescription =
      m_simulationDescription + "Simulation started at: " + ctime (&givemetime);

  m_outputManager->PrintIntro (m_simulationDescription);

  givemetime = time (NULL);
  std::cout << "Simulation: " << m_simulationName
            << " started at time: " << std::string (ctime (&givemetime)) << std::endl;

  Simulator::Run ();

  givemetime = time (NULL);

  m_outputManager->SimulationEnd (std::string (ctime (&givemetime)));

  givemetime = time (NULL);
  std::cout << "Simulation: " << m_simulationName
            << " ended at time: " + std::string (ctime (&givemetime)) << std::endl;

  if (CommunicationStatistics::Y == m_stats)
    {
      //save data collected by the data object
      Ptr<DataOutputInterface> output;
      output = CreateObject<OmnetDataOutput> ();
      output->SetFilePrefix ("./src/onion_routing_wsn/sim_results/comm-overhead-stats-" +
                             m_simulationName);
      output->Output (data);
    }

  Simulator::Destroy ();
}

/**
 *  Convert the string given as parameter containing path lenghts in the array
 *  m_onionPathsLenghts containing a path length in each cell
 *
 */

void
WsnConstructor::ProcessPathString ()
{
  //count the number of paths
  int start = 0;
  int end = m_pathsLengths.find (',');
  while (end != -1)
    {
      m_numOnionPaths++;
      start = end + 1; //increase by the delimiter size in this case one symbol (,)
      end = m_pathsLengths.find (',', start);
    }
  m_onionPathsLengths = new uint16_t[++m_numOnionPaths];

  //insert the path length in single cells of the array m_onionPathsLengths
  start = 0;
  end = m_pathsLengths.find (',');
  int i = 0;
  while (end != -1)
    {
      m_onionPathsLengths[i] =
          (uint16_t) (unsigned int) std::stoi (m_pathsLengths.substr (start, end - start));
      start = end + 1; //increase by the delimiter size in this case one symbol (,)
      end = m_pathsLengths.find (',', start);
      i++;
    }
  m_onionPathsLengths[i] =
      (uint16_t) (unsigned int) std::stoi (m_pathsLengths.substr (start, end - start));
}

void
WsnConstructor::CaptureStatistics ()
{

  data.DescribeRun ("onion-routing-wsn", m_simulationDescription, "communication overhead", "");
  data.AddMetadata ("Captured data:",
                    "Count of packets and statistics about packet size. (sum is in bytes)");

  Ptr<PacketSizeMinMaxAvgTotalCalculator> macTxPkts =
      CreateObject<PacketSizeMinMaxAvgTotalCalculator> ();
  macTxPkts->SetKey ("total-wsn-communication-overhead-transmitted");
  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                   MakeCallback (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate, macTxPkts));
  data.AddDataCalculator (macTxPkts);
  Ptr<PacketSizeMinMaxAvgTotalCalculator> macRxPkts =
      CreateObject<PacketSizeMinMaxAvgTotalCalculator> ();
  macRxPkts->SetKey ("total-wsn-communication-overhead-received");
  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
                   MakeCallback (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate, macRxPkts));
  data.AddDataCalculator (macRxPkts);
  Ptr<PacketSizeMinMaxAvgTotalCalculator> appRx =
      CreateObject<PacketSizeMinMaxAvgTotalCalculator> ();
  appRx->SetKey ("app-layer-data-received");
  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::Wsn_node/AppRx",
                   MakeCallback (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate, appRx));
  data.AddDataCalculator (appRx);
}

/**
 * Create nodes and split them between node containers, to split nodes by functionality in the application
 */
void
WsnConstructor::CreateNodes ()
{
  NS_LOG_INFO ("--------------- Create " << m_numNodes << " sensor nodes and 1 sink node");

  m_sink.Create (1);
  sensornodes.Create (m_numNodes);

  wifiNodes.Add (m_sink);
  wifiNodes.Add (sensornodes);

  m_simulationDescription = m_simulationDescription +
                            "Total sensornodes: " + std::to_string (sensornodes.GetN ()) +
                            " and 1 sink node \n";
}

/**
 * Set up the network: configure the physical mode, the wi-fi parameters (seting an adhoc wifi), etc.
 */
void
WsnConstructor::CreateDevices ()
{
  NS_LOG_INFO ("--------------- Creating and configuring devices & setup WiFi channel");

  WifiHelper wifi;
  //define the used standard
  switch (m_mac)
    {
    case IEEE_80211n::F_24GHz:
      wifi.SetStandard (WIFI_STANDARD_80211n_2_4GHZ);
      break;
    case IEEE_80211n::F_5GHz:
      wifi.SetStandard (WIFI_STANDARD_80211n_5GHZ);
      break;
    }

  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager");
  //htmcs = High throughput modulation coding schemes

  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper ();

  //Configure the channel:
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

  //get details about MCS
  StringValue ptr;
  Ptr<ConstantRateWifiManager> cr = CreateObject<ConstantRateWifiManager> ();
  cr->GetAttribute ("DataMode", ptr);
  std::string mcs_details = ", DataMode: " + ptr.Get ();
  cr->GetAttribute ("ControlMode", ptr);
  mcs_details = mcs_details + ", ControlMode: " + ptr.Get ();

  switch (m_mac)
    {
    case IEEE_80211n::F_24GHz:
      //if wifiPhy is 2.4Ghz
      wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel", "Exponent",
                                      DoubleValue (3.0), "ReferenceLoss", DoubleValue (40.0459));
      m_simulationDescription =
          m_simulationDescription + "Wireless: IEEE 802.11n at 2.4GHz, " + mcs_details;
      break;
    case IEEE_80211n::F_5GHz:
      //if wifiPhy is default - 5Ghz
      wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel", "Exponent",
                                      DoubleValue (3.0));
      m_simulationDescription =
          m_simulationDescription + "Wireless: IEEE 802.11n at 5GHz, " + mcs_details;
      break;
    }

  wifiPhy.SetChannel (wifiChannel.Create ());

  //MAC configuration
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac", "BE_MaxAmpduSize", UintegerValue (65535));

  //install devices
  wifiDevices = wifi.Install (wifiPhy, wifiMac, wifiNodes);
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HtConfiguration/"
               "ShortGuardIntervalSupported",
               BooleanValue (false)); //set guard interval -- interval betweensymbols to 800ns

  //get MTU and MSS details
  UintegerValue mtu;
  wifiDevices.Get (0)->GetAttribute ("Mtu", mtu);
  m_simulationDescription = m_simulationDescription + ", MTU:" + std::to_string (mtu.Get ()) +
                            ", MSS:" + std::to_string (m_mss) + "\n";
}

/*
* allocate nodes on physical positions in space
*
* construct the random disc topology
* sensor nodes are randomly deployed on a disc shaped plane
*/

void
WsnConstructor::BuildDiscTopology ()
{
  //allocate3 positions
  NS_LOG_INFO ("--------------- Building disc topology");

  //calculate radius of the disc based on node density and communication range
  // compute the radius of a circle that covers the sum of the areas of each node
  int r_disc = m_radius * sqrt (m_numNodes);

  m_simulationDescription =
      m_simulationDescription +
      "Network topology: RANDOM DISC, node radius: " + std::to_string (m_radius) +
      "m , disc topology radius: " + std::to_string (r_disc) +
      "m. Sink node located at x:" + std::to_string ((int) r_disc) +
      ",y:" + std::to_string ((int) r_disc) + " \n";

  //Random disc
  ObjectFactory rndDisc;
  rndDisc.SetTypeId ("ns3::RandomDiscPositionAllocator");
  rndDisc.Set ("Rho", StringValue ("ns3::UniformRandomVariable[Min=40|Max=" +
                                   std::to_string (r_disc) + "]"));
  rndDisc.Set ("X", DoubleValue (r_disc));
  rndDisc.Set ("Y", DoubleValue (r_disc));
  rndDisc.Set ("Z", DoubleValue (0));

  Ptr<PositionAllocator> posAlloc = rndDisc.Create ()->GetObject<PositionAllocator> ();

  mobility.SetPositionAllocator (posAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (sensornodes);

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (r_disc, r_disc, 0.0)); // only one position for sink
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (m_sink);
}

/*
* allocate nodes on physical positions in space
*
* construct the grid topology
* sensor nodes are deployed in a plane following a grid structure; each sensor node is equidistant from the closest sensor nodes in cardinal directions
*/

void
WsnConstructor::BuildGridTopology ()
{
  NS_LOG_INFO ("--------------- Building grid topology");

  //calculate row size based on node number
  int row_size = sqrt (m_numNodes);
  int deltaX = m_cellSide;
  int deltaY = m_cellSide;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0), "MinY",
                                 DoubleValue (0), "DeltaX", DoubleValue (deltaX), "DeltaY",
                                 DoubleValue (deltaY), "GridWidth", UintegerValue (row_size),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //put the sink around the middle of the grid
  mobility.Install (sensornodes);
  mobility.Install (m_sink);

  //change the location of the sink node to center by swap it with a node in the center
  Ptr<MobilityModel> sinkMobility = m_sink.Get (0)->GetObject<MobilityModel> ();

  //Get the middle sensor node
  int m = row_size / 2 * row_size + row_size / 2;

  Ptr<MobilityModel> middleNodeMobility = sensornodes.Get (m)->GetObject<MobilityModel> ();
  double middleNode_x = middleNodeMobility->GetPosition ().x;
  double middleNode_y = middleNodeMobility->GetPosition ().y;
  //swap node positions
  middleNodeMobility->SetPosition (
      Vector (sinkMobility->GetPosition ().x, sinkMobility->GetPosition ().y, 0));
  sinkMobility->SetPosition (Vector (middleNode_x, middleNode_y, 0));

  m_simulationDescription = m_simulationDescription +
                            "Network topology: GRID with row size: " + std::to_string (row_size) +
                            ". Distance between nodes on X-axis: " + std::to_string (deltaX) +
                            "m. Distance between nodes on Y-axis: " + std::to_string (deltaY) +
                            "m. Sink node located at x:" + std::to_string ((int) middleNode_x) +
                            ",y:" + std::to_string ((int) middleNode_y) + " \n";
}

/**
 * Set internet stack (ipv4) and its addresses
 */
void
WsnConstructor::InstallInternetStack ()
{

  NS_LOG_INFO ("--------------- Install routing & internet stack ");

  switch (m_routing)
    {
    case Routing::AODV:
      m_outputManager->SetRouting (Routing::AODV);
      AODVrouting ();
      break;
    case Routing::DSR:
      m_outputManager->SetRouting (Routing::DSR);
      DSRrouting ();
      break;
    case Routing::OLSR:
      m_outputManager->SetRouting (Routing::OLSR);
      OLSRrouting ();
      break;
    case Routing::DSDV:
      m_outputManager->SetRouting (Routing::DSDV);
      DSDVrouting ();
      break;
    }

  Ipv4AddressHelper address;

  address.SetBase ("10.1.0.0", "255.255.0.0");

  wifiInterfaces = address.Assign (wifiDevices);
}

void
WsnConstructor::DSRrouting ()
{
  //DSR routing
  m_simulationDescription = m_simulationDescription + "Routing: DSR\n";

  DsrMainHelper dsrMain;
  DsrHelper dsr;

  InternetStackHelper stack;
  stack.Install (wifiNodes);
  dsrMain.Install (dsr, wifiNodes);
}

void
WsnConstructor::AODVrouting ()
{
  //AODV routing
  m_simulationDescription = m_simulationDescription + "Routing: AODV\n";

  AodvHelper aodv;

  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (aodv, 10);

  InternetStackHelper stack;
  stack.SetRoutingHelper (list);
  stack.Install (wifiNodes);
}

void
WsnConstructor::OLSRrouting ()
{
  //OLSR routing
  m_simulationDescription = m_simulationDescription + "Routing: OLSR\n";

  OlsrHelper olsr;

  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (olsr, 10);

  InternetStackHelper stack;
  stack.SetRoutingHelper (list);
  stack.Install (wifiNodes);
}

void
WsnConstructor::DSDVrouting ()
{
  //DSDV routing
  m_simulationDescription = m_simulationDescription + "Routing: DSDV\n";

  DsdvHelper dsdv;

  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (dsdv, 10);

  InternetStackHelper stack;
  stack.SetRoutingHelper (list);
  stack.Install (wifiNodes);
}

/**
 * Install an aplication on network
 */
void
WsnConstructor::InstallApplications ()
{
  NS_LOG_INFO ("--------------- Configuring Applications ");

  //olsr is a proactive routing algo and need some time to compute routes
  //at least 30s
  int routing_setup_time = 20;

  if (m_routing == Routing::OLSR || m_routing == Routing::DSDV)
    {
      routing_setup_time = 60;
    }

  //sink helper
  SinkHelper msh (m_numNodes - 1, m_outputManager, m_onionValidator, m_onionPathsLengths);
  //node helper - create nodes helpers to install node application
  SensorNodeHelper mnh (wifiInterfaces.GetAddress (0), m_outputManager, m_onionValidator);

  //install sink apps
  sinkApps = msh.Install (m_sink);
  //install node apps
  sensornodeApps.Add (mnh.Install (sensornodes));

  //setup onion routing settings on the sink node
  sinkApps.Get (0)->GetObject<Sink> ()->Setup (m_onionPathsLengths, m_numOnionPaths,
                                               m_onionRepeate);

  //start apps
  sinkApps.Start (Seconds (1.0 + routing_setup_time));
  sensornodeApps.Start (Seconds (2.0 + routing_setup_time));

  UintegerValue node_delay;
  sinkApps.Get (0)->GetObject<Sink> ()->GetAttribute ("Delay", node_delay);
  int start_onion = (node_delay.Get () * m_numNodes) / 1000 +
                    5; // when the sink node will start send onions in seconds

  m_simulationDescription =
      m_simulationDescription + "Routing setup time: " + std::to_string (routing_setup_time) +
      "s, nodes are starting sequentially with " + std::to_string (node_delay.Get ()) +
      "ms interval, onion starts at: " + std::to_string (start_onion + routing_setup_time) + "s\n";

  m_simulationDescription = m_simulationDescription + "Onion path lengths: ";

  for (int i = 0; i < m_numOnionPaths; ++i)
    {
      m_simulationDescription =
          m_simulationDescription + std::to_string (m_onionPathsLengths[i]) + " ";
    }
  m_simulationDescription = m_simulationDescription + " repeate each path lenght " +
                            std::to_string (m_onionRepeate) + " times.\n";
}

int
main (int argc, char **argv)
{

  //Load attributes from xml configuration file
  Config::SetDefault ("ns3::ConfigStore::Filename",
                      StringValue ("src/onion_routing_wsn/configParameters.xml"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("Xml"));
  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  //load attributes setup from commandline
  CommandLine cmd;
  cmd.AddValue (
      "a_simNum",
      "ns3::WsnConstructor::SimulationSeed"); //number of the simulation, that defines the seed
  cmd.AddValue ("a_name", "ns3::WsnConstructor::SimulationName"); // a name used as identifier
  cmd.AddValue ("a_routing",
                "ns3::WsnConstructor::Routing"); //the selected routing protocol
  cmd.AddValue ("a_topology", "ns3::WsnConstructor::Topology"); //the selected topology
  cmd.AddValue ("a_nodeNumber", "ns3::WsnConstructor::NodeNumber"); //number of nodes in the network
  cmd.Parse (argc, argv);

  Ptr<WsnConstructor> bt = CreateObject<WsnConstructor> ();

  bt->Configure (); //configure the simulation

  bt->Run ();

  return 0;
}
