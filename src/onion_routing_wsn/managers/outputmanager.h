/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include <vector>
#include "ns3/output-stream-wrapper.h"
#include "ns3/internet-module.h"
#include <time.h>
#include "ns3/enums.h"
#include <experimental/filesystem> 

namespace ns3 {

/**
 *  \defgroup managers
 * 
 *  This section documents the API of the ns-3 Managers module.
 *  For a functional description, please refer to the ns-3 manual here:
 *  http://www.nsnam.org/docs/models/html/managers.html
 *
 *  Be sure to read the manual BEFORE going down to the API.
 */

/**
 * \ingroup managers
 * \class OutputManager
 * \brief Class that manages the output of the simulation. ConsoleLog output and Output on .csv file   
 * 
 * 
 */

class OutputManager : public Object
{

public:
  /**
  *
  * \brief Default constructor
  *
  */
  OutputManager ();

  /**
  *
  * \brief constructor that takes simulation details to print on the csv file or on the console log
  *
  */

  OutputManager (std::string name, uint16_t simNum, uint16_t numNodes, enum Topology, enum Routing,
                 bool printDescription);

  /**
  *
  * \brief Default destructor
  *
  */
  ~OutputManager ();


     /**
  *
  * \brief Create stream wrapper and the output file, if the output file already exists, then delete the content
  *
  */

  void CreateOutputFile ();

  /**
 *  Register this type.
 *  \return The object TypeId.
 */
  static TypeId GetTypeId (void);

  /**
  *
  * \brief Print the simulation description containing simulation settings on the csv file or on the console log
  *
  */
  void PrintIntro (std::string intro);
  /**
  *
  * \brief Print at the end of the simulation
  *
  */
  void SimulationEnd (std::string end_at);

  /**
  *
  * \brief Called by the sink node when it sends a new onion message
  *
  */
  void SendOnion (int packet_size, int head_size, int body_size, int onion_path_len, Time sent_at);
  /**
  *
  * \brief Called by the sink node when it receives back the onion message
  *
  */
  void RecvOnion (Time recv_at);

  /**
  *
  * \brief Called when an onion is deleted
  *
  */
  void AbortOnion (Time abort_at);

  /**
  *
  * \brief Called when the sink receives a new handhake message
  *
  */
  void NewHandshake (int node_num, Ipv4Address node_ip, Time recv_at);

  /**
  *
  * \brief Called by each node that sends an onion message
  *
  */
  void OnionRoutingSend (Ipv4Address send_ip, Ipv4Address recv_ip, int packet_size, int head_size,
                         int body_size, Time sent_at);

  /**
  *
  * \brief Called by each node that receives an onion message
  *
  */
  void OnionRoutingRecv (Time recv_at);

  /**
  *
  * \brief print the given argument on the csv file
  *
  */
  void PrintLine (std::string line);

  /**
  *
  * \brief Convert an IpV4 address to a string
  *
  */
  std::string Ipv4ToString (Ipv4Address ip);

  /**
  *
  * \brief return the current time as a string
  *
  */
  std::string CurrentTime (void);

  /**
  *
  * \brief register node details: IP, location on the network
  *
  */
  void AddNodeDetails (Ipv4Address node_ip, int coord_x, int coord_y);

  /**
  *
  * \brief register node details: IP, location on the network and node degree 
  *         for node degree reference check ns3::Wsn_node::NodeDegree()
  *
  */
  void AddNodeDetails (Ipv4Address node_ip, int coord_x, int coord_y, int n_degree);

  /**
  *
  * \brief print node details on the csv file, print only nodes reachable by the sink node
  * 
  * 
  * \param [in] reachable the set of nodes reachable by the sink node
  *
  */
  void PrintNodeDetails (std::map<uint32_t, std::string> reachable);

  /**
  *
  * \brief return the enum of the current routing algorithm used in the network
  *
  */
  enum Routing GetRouting (void);

  /**
  *
  * \brief set the enum of the current routing algorithm used in the network
  *
  */
  void SetRouting (enum Routing routing);

  Ptr<OutputStreamWrapper> m_simStreamWrapper; //!< stream wrapper to write on file

  bool m_printDescription; //!< boolean choice to print the description of the simulation parameters

  std::string m_simName; //!< holds the name of the simulation

  std::string m_simDetails; //!< holds details of the simulation

  //HEADERS
  std::string h_onionHeader =
      "onion_details,sim_name,sim_num,num_of_nodes,topology,routing,onion_"
      "id,packet_size,onion_head_size,onion_body_size,onion_"
      "path_length,sent_at,recv_at,query_time_to_return"; //!< header of CSV format
  std::string h_routingHeader = "onion_routing,sim_name,sim_num,num_of_nodes,topology,routing,"
                                "onion_id,send_ip,recv_ip,packet_size,onion_head_size,onion_body_"
                                "size,sent_at,recv_at,hop_time"; //!< header of CSV format
  std::string h_timeoutHeader = "timeout,sim_name,sim_num,num_of_nodes,topology,routing,onion_id,"
                                "onion_path_length,abort_time"; //!< header of CSV format
  std::string h_nodeDetailsHeader = "node_details,sim_name,sim_num,num_of_nodes,topology,routing,"
                                    "coord_x,coord_y,node_degree"; //!< header of CSV format

  //reference to the onion we are executing
  std::string m_onionData; //!< holds data of the onion message currently executing in the network
  std::string m_routingData; //!< holds data of the onion message traveling from hop to hop
  std::string m_routingLog; //!< holds data of the onion message traveling from hop to hop
  int m_onionId = 0; //!< identifies the onion message
  int m_onionPathLength = 0; //!< the onion path length

  double t_onionDelta = 0; //!< Hold time information of the onion message traveling in the network
  double t_hopDelta = 0; //!<  Hold time information of the onion message traveling from hop to hop

  enum Routing m_routing; //!< information on the routing protocol

  std::map<uint32_t,std::string>
      m_nodeDetails; //!< holds details of nodes in the network for printing at the end of the csv file.

  std::string m_outputFilePath; //!< path to the directory where output files are stored
};

} // namespace ns3

#endif /* OUTPUTMANAGER_H */