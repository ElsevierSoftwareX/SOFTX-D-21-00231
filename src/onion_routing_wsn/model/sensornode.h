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


#ifndef SENSORNODE_H
#define SENSORNODE_H

#include "ns3/packet.h"

#include <sodium.h>
#include "ns3/proto-packet.pb.h"
#include "ns3/serializationwrapper.h"
#include "ns3/wsn_node.h"

namespace ns3 {



/**
 * \ingroup node-application
 * 
 * \class SensorNode
 * \brief The application of the sensor node. 
 *
 */

class SensorNode : public Wsn_node
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
  SensorNode ();

  /**
  *
  * \brief Default destructor
  *
  */
  virtual ~SensorNode ();

  /**
 *  \brief Executed when a new onion is received.
 *         Repeatedly call ns3::Wsn_node::RecvSegment() until the whole packet is received.
 *         Then, check if the onion is valid by comparing the onionID of the onion and 
 *         the onion sequence number in the ns3::OnionValidator::GetOnionSeq().
 *         If the onion is not valid then delete the onion.
 *         Otherwise:
 *            Signal that the onion was received.
 *            Process the onion head and retrieve the next hop ip value. (ns3::SensorNode::ProcessOnionHead())
 *            Processs the onion body. (ns3::SensorNode::ProcessOnionBody())
 *            Send the onion meesage to the next hop ip.
 *              
 * 
 *  \param [in] socket the incoming connection
 * 
 */

  void ReceivePacket (Ptr<Socket> socket);

  /**
 *  \brief Decrypt the outer layer of the onion head, obtain the information of the next IP address, 
 *         reasemble the onion head by including padding if required.
 *              
 * 
 *  \param [in] onionHead pionter to the protobuf object holding informations of the onion head
 * 
 *  \return an IpV4 address as an unisgned integer of 32b
 */

  uint32_t ProcessOnionHead (protomessage::ProtoPacket_OnionHead *onionHead);

  /**
 *  \brief If the onion body contains the aggregated value, 
 *        then aggregate the sensor (dummy) value to the value carried in the onion body
 *          
 *              
 * 
 *  \param [in] onionBody pionter to the protobuf object holding informations of the onion body
 * 
 */

  void ProcessOnionBody (protomessage::ProtoPacket_OnionBody *onionbody);

  /**
 *  \brief Accept new TCP connections
 * 
 *  \param [in] socket
 *  \param [in] from sending address
 * 
 */

  void Accept (Ptr<Socket> socket, const ns3::Address &from);

  /**
 *  \brief Convert an IPV4 address given as a buffer
 * 
 *  \param [in] buff pointer to an array containing an IpV4 address
 *  
 *  \return an IpV4 address as an unisgned integer of 32b
 * 
 */

  uint32_t DeserializeIpv4ToInt (uint8_t *buff);

private:
  /**
  *
  * \brief 1.Start the application run ns3::Wsn_node::Configure()
  *        2.Generate new encryption keys
  *        3.Schedule the execution of ns3::SensorNode::Handshake() after \p delay milliseconds, the delay is computed based on the node ip address
  *        4.Set callback at new socket connection   
  * */

  virtual void StartApplication (void);

  /**
  *
  * \brief Stop the application 
  * 
  * */
  virtual void StopApplication (void);

  /**
  *
  * \brief Construct a new protobuf object containing the node publickey and send it to the sink node
  *
  * 
  * */

  void Handshake (void);

  Ipv4Address m_sinkAddress; //!<  address of the sink node
  //the reading of the sensor
  uint32_t m_sensorValue = 20; //!< dummy reading of a sensor equipped on the node
};

} // namespace ns3

#endif /* SENSORNODE_H */
