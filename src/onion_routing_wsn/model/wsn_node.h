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


#ifndef WSN_NODE_H
#define WSN_NODE_H

#include <fstream>
#include <iostream>
#include <string>

#include "ns3/outputmanager.h"
#include "ns3/segmentnum.h"

#include "ns3/mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/address.h"
#include "ns3/applications-module.h"
#include "ns3/olsr-module.h"
#include "ns3/outputmanager.h"
#include "ns3/onionmanager.h"
#include "ns3/onionvalidator.h"

namespace ns3 {

/**
 *  \defgroup node-application
 * 
 *
 *  Be sure to read repository README.md before going down to the API.
 */

/**
 * \ingroup node-application
 * \class Wsn_node
 * \brief The wsn node base class that manages the sending and receiving of packets and basic configuration of nodes
 *
 */

class Wsn_node : public Application
{
public:
  /**
 *  Register this type.
 *  \return The object TypeId.
 */
  static TypeId GetTypeId (void);

  /**
  *
  * \brief Default constructor
  *
  */
  Wsn_node ();

  /**
  *
  * \brief 1. configure basic attributes of nodes
  *        2. create socket and listen for connections
  *        3. get node position in the form of x and y coordinates
  *
  */
  void Configure (void);
  //Node Degree

  /**
  *
  * \brief  Retrieve the degreee this node from the OLSR routing protocol.
  *         Works only if using the OLSR protocol.
  *         We refer to the degreee of the current node as the number of nodes that are
  *         in the one-hop neighbourhood of the current node with respect to the multi-hop communication.
  * 
  * \param [in] coord_x x coordinate of the current node
  * \param [out] coord_y y coordinate of the current node
  */
  void NodeDegree (double coord_x, double coord_y);

  /**
  *
  * \brief  Send a packet through a TCP connection to the remote address.
  *         Set b_onion to true to send an onion message. 
  *         If b_onion is true, the method sets a callback after \p m_onionTimeout seconds
  *         The callback triggers the function ns3::Wsn_node::CheckSentOnion().
  *         If the packe size is larger than the MSS. Then add a tag to the packet, 
  *         which specifies the size of the whole packet.
  *        	The tag is used by ns3::Wsn_node::RecvSeg() to reconstruct a packet that was split into multiple segments.
  * 
  * \param [in] remote the receiving address
  * \param [in] packet the packet to send
  * \param [in] b_onion boolean value, set to true if sending an onion message. Adds the packet TAG to the packet
  */
  void SendSegment (InetSocketAddress remote, Ptr<Packet> packet, bool b_onion);

  /**
  *
  * \brief  method for receiving a segment 
  *         calls ns3::Wsn_node::RecvSeg() 
  * 
  * \param [in] socket the receiving socket 
  * 
  * \return the result of the ns3::Wsn_node::RecvSeg() function
  * 
  * */

  Ptr<Packet> RecvSegment (Ptr<Socket> socket);
  /**
  *
  * \brief  method for receiving a segment 
  *         calls ns3::Wsn_node::RecvSeg() 
  * 
  * \param [in] socket the receiving socket 
  * \param [in,out] from extract the sender IP from the receiving socket 
  * 
  * \return the result of the ns3::Wsn_node::RecvSeg() function
  * 
  * */
  Ptr<Packet> RecvSegment (Ptr<Socket> socket, Address &from);

  /**
  *
  * \brief  method for receiving packets able to merge segment fragments 
  *         if a packet was split into multiple segments due to being larger than the MSS
  *         Use the packet tag, to merge packet fragments into a single packet.
  * 
  * \param [in] socket the receiving socket 
  * \param [in] packet pointer to the receiving packet 
  * \param [in,out] from extract the sender IP from the receiving socket 
  * 
  * \return return pointer to packet if the whole packet is received OR return NULL value if only packet fragment is received
  * 
  * */

  Ptr<Packet> RecvSeg (Ptr<Socket> socket, Ptr<Packet> p, Address from);

  /**
  *
  * \brief  compute when the node should start the handshake process from the node ip address
  *         Used to not generate a network congestion if all nodes will strat the handshake at the same time
  * 
  * \param [in] node_address the ipv4 address
  * 
  * \return return the delay in milliseconds
  * 
  * */

  uint32_t getNodeDelay (Ipv4Address node_address);

  /**
  *
  * \brief Signal to the ns3::OnionValidator that the onion was corrctly received
  * 
  * */

  void OnionReceived (void);

  /**
  *
  * \brief The method checks if the onion was received by the next receiving node
  *        If the onion was not received then abort the current onion and 
  *         schedule the sending of another onion with equal parametrs as the aborted onion
  * 
  * */
  void CheckSentOnion (int count);

protected:
  uint16_t m_port; //!< port of the application
  Ptr<OutputManager> m_outputManager; //!< Pointer to the ns3::OutputManager
  Ptr<OnionValidator> m_onionValidator; //!< Pointer to the ns3::OnionValidator
  Ipv4Address m_address; //!< ns3::Ipv4Address of this node
  Ptr<Socket> m_socket; //!< listening socket
  uint16_t m_delay; //!< delay after which the handshake process will start
  OnionManager m_onionManager; //!< The ns3::OnionManager object

  //To manage fragments
  uint16_t f_mss; //!< maximum segment size
  int f_segmentSize; //!< the size of the whole packet
  Ptr<Packet>
      f_pendingPacket; //!< pointer to the packet where received segment fragments are stored
  Ipv4Address f_receivingAddress = Ipv4Address::GetAny (); //!< the receiving address

  // onion state
  int o_hopCount = 0; //!< track how the onion is is transiting in the network
  int o_sequenceNum = 0; //!< sequence number of the onion, should be same as onion_id

  uint16_t
      m_onionTimeout; //!< timer in seconds, if elepsed and the onion was not recieved by the next receiver, then delete the onion

private:
  /**
  *
  * \brief Start the application 
  * 
  * */

  virtual void StartApplication (void);

  /**
  *
  * \brief Stop the application 
  * 
  * */
  virtual void StopApplication (void);
};
} // namespace ns3

#endif /* WSN_NODE_H */
