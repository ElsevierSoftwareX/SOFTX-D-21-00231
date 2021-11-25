/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


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


#include "outputmanager.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (OutputManager);

NS_LOG_COMPONENT_DEFINE ("outputmanager");

TypeId
OutputManager::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::OutputManager")
          .SetParent<Object> ()
          .AddConstructor<OutputManager> ()
          .AddAttribute ("OutputPath", "Path to the directory where output files are stored",
                         StringValue ("./src/onion_routing_wsn/sim_results/"),
                         MakeStringAccessor (&OutputManager::m_outputFilePath),
                         MakeStringChecker ());
  return tid;
}

OutputManager::OutputManager ()
{
}

OutputManager::OutputManager (std::string name, uint16_t simNum, uint16_t numNodes,
                              enum Topology topology, enum Routing routing, bool printDescription)
{

  m_printDescription = printDescription;

  m_simName = name;

  m_simDetails = std::to_string (simNum) + "," + std::to_string (numNodes) + ",";

  switch (topology)
    {
    case Topology::GRID:
      m_simDetails = m_simDetails + "grid" + ",";
      break;
    case Topology::DISC:
      m_simDetails = m_simDetails + "disc" + ",";
      break;
    }

  switch (routing)
    {
    case Routing::AODV:
      m_simDetails = m_simDetails + "aodv";
      break;
    case Routing::DSR:
      m_simDetails = m_simDetails + "dsr";
      break;
    case Routing::OLSR:
      m_simDetails = m_simDetails + "olsr";
      break;
    case Routing::DSDV:
      m_simDetails = m_simDetails + "dsdv";
      break;
    }
  //set routing
  m_routing = routing;
}

void
OutputManager::CreateOutputFile ()
{
  //if filename exists -> remove
  std::remove (std::string (m_outputFilePath + m_simName + ".csv").c_str ());

  Ptr<OutputStreamWrapper> StreamWrapper =
      Create<OutputStreamWrapper> (m_outputFilePath + m_simName + ".csv", std::ios::app);
  m_simStreamWrapper = StreamWrapper;
}

OutputManager::~OutputManager ()
{
}

void
OutputManager::PrintIntro (std::string intro)
{

  NS_LOG_INFO (
      "---------------------------------Simulation description-----------------------------------\n"
      << intro
      << "-----------------------------------Simulation "
         "output--------------------------------------");
  if (m_printDescription)
    {
      PrintLine ("---------------------------------Simulation "
                 "description-----------------------------------\n" +
                 intro + "--csv headers--\n" + h_onionHeader + "\n" + h_routingHeader + "\n" +
                 h_timeoutHeader + "\n" + h_nodeDetailsHeader +
                 "\n-----------------------------------Simulation "
                 "output--------------------------------------");
    }
}

void
OutputManager::SimulationEnd (std::string end_at)
{
  if (m_printDescription)
    {
      PrintLine ("-------------------- Simulation end: " + end_at);
    }
}

void
OutputManager::SendOnion (int packet_size, int head_size, int body_size, int onion_path_len,
                          Time sent_at)
{
  m_onionId++;
  m_onionPathLength = onion_path_len;
  t_onionDelta = sent_at.GetSeconds ();
  m_onionData = "onion_details," + m_simName + "," + m_simDetails + "," +
                std::to_string (m_onionId) + "," + std::to_string (packet_size) + "," +
                std::to_string (head_size) + "," + std::to_string (body_size) + "," +
                std::to_string (onion_path_len) + "," + std::to_string (sent_at.GetSeconds ());

  NS_LOG_INFO ("--------------- Onion message sent at time: "
               << std::to_string (sent_at.GetSeconds ()) << ", with onion id: " << m_onionId
               << ", onion path length: " << onion_path_len
               << ", packet size: " + std::to_string (packet_size) << " B, head size: " << head_size
               << " B, body size: " << body_size << " B");
}

void
OutputManager::RecvOnion (Time recv_at)
{
  m_onionData = m_onionData + "," + std::to_string (recv_at.GetSeconds ()) + "," +
                std::to_string (recv_at.GetSeconds () - t_onionDelta);
  PrintLine (m_onionData);

  NS_LOG_INFO ("--------------- Onion message received back at time :"
               << std::to_string (recv_at.GetSeconds ()) << " with onion id: " << m_onionId
               << ", onion traveling time: "
               << std::to_string (recv_at.GetSeconds () - t_onionDelta));
}

void
OutputManager::OnionRoutingSend (Ipv4Address send_ip, Ipv4Address recv_ip, int packet_size,
                                 int head_size, int body_size, Time sent_at)
{
  t_hopDelta = sent_at.GetSeconds ();
  m_routingData = "onion_routing," + m_simName + "," + m_simDetails + "," +
                  std::to_string (m_onionId) + "," + Ipv4ToString (send_ip) + "," +
                  Ipv4ToString (recv_ip) + "," + std::to_string (packet_size) + "," +
                  std::to_string (head_size) + "," + std::to_string (body_size) + "," +
                  std::to_string (sent_at.GetSeconds ());

  m_routingLog = "Onion routing-- onion sent from node ip: " + Ipv4ToString (send_ip) +
                 ", of packet size: " + std::to_string (packet_size) +
                 " B, header size: " + std::to_string (head_size) +
                 " B, trailer size: " + std::to_string (body_size) +
                 " B, received at node ip: " + Ipv4ToString (recv_ip);
}

void
OutputManager::OnionRoutingRecv (Time recv_at)
{
  m_routingData = m_routingData + "," + std::to_string (recv_at.GetSeconds ()) + "," +
                  std::to_string (recv_at.GetSeconds () - t_hopDelta);
  PrintLine (m_routingData);

  NS_LOG_INFO (m_routingLog << ", received at time: " + std::to_string (recv_at.GetSeconds ())
                            << ", hop traveling time: "
                            << std::to_string (recv_at.GetSeconds () - t_hopDelta));
}

void
OutputManager::AbortOnion (Time abort_at)
{
  std::string abort_data = "onion_aborted," + m_simName + "," + m_simDetails + "," +
                           std::to_string (m_onionId) + "," + std::to_string (m_onionPathLength) +
                           "," + std::to_string (abort_at.GetSeconds ());
  PrintLine (abort_data);

  NS_LOG_INFO ("Onion Was aborted at time: " << std::to_string (abort_at.GetSeconds ())
                                             << " , with onion id: " << m_onionId);
}

//prints the output when a new node registers to the sink
void
OutputManager::NewHandshake (int node_num, Ipv4Address node_ip, Time recv_at)
{
  /* uncomment to print on file
  std::string data = "handshake," + m_simName + "," + m_simDetails + "," +
                     std::to_string (node_num) + "," + Ipv4ToString (node_ip) + "," +
                     std::to_string (recv_at.GetSeconds ());
  PrintLine (data); */
  NS_LOG_INFO ("New node, number: " << node_num << " ip: " << Ipv4ToString (node_ip)
                                    << " at time: " << std::to_string (recv_at.GetSeconds ()));
}

void
OutputManager::AddNodeDetails (Ipv4Address node_ip, int coord_x, int coord_y)
{
  std::string tmp = "node_details," + m_simName + "," + m_simDetails + "," +
                    Ipv4ToString (node_ip) + "," + std::to_string (coord_x) + "," +
                    std::to_string (coord_y) + ",null";
  this->m_nodeDetails[node_ip.Get ()] = tmp;
}

void
OutputManager::AddNodeDetails (Ipv4Address node_ip, int coord_x, int coord_y, int n_degree)
{
  std::string tmp = "node_details," + m_simName + "," + m_simDetails + "," +
                    Ipv4ToString (node_ip) + "," + std::to_string (coord_x) + "," +
                    std::to_string (coord_y) + "," + std::to_string (n_degree);
  //std::cout << tmp << std::endl;
  this->m_nodeDetails[node_ip.Get ()] = tmp;
}

void
OutputManager::PrintNodeDetails (std::map<uint32_t, std::string> reachable)
{
  for (auto const &x : m_nodeDetails)
    {
      if (reachable.find (x.first) != reachable.end ())
        {
          PrintLine (x.second);
        }
    }
}

void
OutputManager::PrintLine (std::string line)
{
  std::ostream *stream = m_simStreamWrapper->GetStream ();
  *stream << line << std::endl;
}

std::string
OutputManager::Ipv4ToString (Ipv4Address ip)
{
  std::stringstream ss;
  ip.Print (ss);
  return ss.str ();
}

std::string
OutputManager::CurrentTime ()
{
  time_t givemetime = time (NULL);
  std::string timme = std::string (ctime (&givemetime));
  return timme.substr (0, timme.length () - 5);
}

void
OutputManager::SetRouting (enum Routing routing)
{
  this->m_routing = routing;
}

enum Routing
OutputManager::GetRouting ()
{
  return m_routing;
}

} // namespace ns3