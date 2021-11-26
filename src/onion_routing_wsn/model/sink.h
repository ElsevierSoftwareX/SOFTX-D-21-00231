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


#ifndef SINK_H
#define SINK_H

#include <sodium.h>

#include <assert.h> /* assert */
#include <iostream>
#include <map>

#include "ns3/wsn_node.h"
#include "ns3/proto-packet.pb.h"
#include "ns3/serializationwrapper.h"
#include "ns3/address.h"
#include "ns3/enums.h"

namespace ns3 {

/**
 * \ingroup node-application
 * 
 * \class Sink
 * \brief The application of the sink node. The node that generates onion messagess.
 *
 */

class Sink : public Wsn_node
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
  Sink ();

  /**
  *
  * \brief Default destructor
  *
  */
  virtual ~Sink ();

  /**
 *  \brief Setup sink node parameters after the application installation
 * 
 * \param [in] onionPathLengths pointer to the array where each value is representing the number 
 *            of hops the onion will travel to return back to the sink node issuer of the onion.
 * \param [in] numOnionLengths number of cells of the array \p onionPathLengths
 * \param [in] repeateTimes integer specifying the number of times to generate the onion message
 *           for each value of the array \p onionPathsLengths
 * 
 */
  void Setup (uint16_t *onionPathlengths, uint16_t numOnionLengths, int repeateTimes);

  /**
 *  \brief Accept new TCP connections
 * 
 *  \param [in] socket
 *  \param [in] from sending address
 * 
 */

  void Accept (Ptr<Socket> socket, const ns3::Address &from);

  /**
 *  \brief Schedule the creation of a new onion based on the path length specified in \p m_onionPathLengths
 *         If all onions specified in \p m_onionPathLengths were executed for \p m_repeateTimes 
 *         Then end the simulation.
 */
  void SinkTasks ();

  /**
 *  \brief Each five seconds call the ns3::OnionValidator and check if at least one onion message is alive
 *          Otherwise the onion was aborted, decrease the value of \p m_repeateCount and schedule a new execution of the function ns3::Sink::SinkTasks() 
 */
  void CheckOnion (void);

private:
  /**
  *
  * \brief 1.Start the application run ns3::Wsn_node::Configure()
  *        2.Generate new encryption keys
  *        3.Set callback at new socket connection
  *        4.Schedule the execution of ns3::Sink::SinkTasks() after \p m_onionDelay milliseconds
  *        5.Schedule the execution of ns3::Sink::CheckOnion() after \p m_onionDelay milliseconds and 5 seconds 
  * 
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
  * \brief Recieve a new packet from \p socket 
  *       Check if the packet is a handshake packet or a packet containing an onion message
  *       1. Handshake packet -> call ns3::Sink::RecvHandshake()
  *       2. Onion message -> call ns3::Sink::RecvOnion()
  * 
  * \param [in] socket the receiving tcp socket  
  * 
  * */

  void ReceivePacket (Ptr<Socket> socket);

  /**
  *
  * \brief When receiving a new handshake with a node. The sink node stores the sensor node IP address and publickey (PK)
  *         in std::map structure \p m_nodeManager , where IP is the key and PK is the value 
  * 
  * \param [in] handshake_data pointer to the protobuf object holding message data
  * \param [in] from the IP address of the sensor node
  * 
  * */

  void RecvHandshake (protomessage::ProtoPacket_Handshake *handshake_data, InetSocketAddress from);

  /**
  *
  * \brief Triggered when an onion message is received back at the sink node
  *       Data about the message is captured and a new onion message is scheduled
  * 
  * \param [in] onion_body pointer to the protobuf object holding onion data
  * 
  * */

  void RecvOnion (protomessage::ProtoPacket_OnionBody *onion_body);

  /**
  *
  * \brief  The method builds the path of the onion message by randomly selecting 
  *         sensor nodes from the \p m_nodeManager structure. The path can have loops, but the same node cannot
  *         be placed in two consequent postions in the onion message path.
  *         The onion path must be of length >= 3.
  * 
  * \param [in,out] route pointer to an array of length \p routeLen
  * \param [in] routeLen length of the array \p route
  * 
  * */

  void SelectRoute (int *route, int routeLen);

  /**
  *
  * \brief  Method that constructs the onion head from \p route and \p routeLen parameters.
  *         The method constructs two arrays \p keys , \p ipRoute containing respectively encryption keys and IP addreses
  *         of sensor nodes in the \p m_nodeManager structure at indexes specified by the \p route array.
  *         The onion head is constructed by calling ns3::OnionRouting::BuildOnion().
  * 
  * \param [in,out] route pointer to an array of length \p routeLen cointaining indexes of sensor nodes in the \p m_nodeManager structure. 
  * \param [in] routeLen length of the array \p route
  * 
  * */

  void PrepareOnion (int *route, int routeLen);

  /**
  *
  * \brief  The method constructs the onion message as a protobuf object.
  *         The method sends the onion message to the sesor node at the \p firstHop address 
  *         The method sets the tracking of the onion by calling ns3::OnionValidator::StartOnion()
  * 
  * \param [in] firstHop IP address of the first sensor node in the onion path
  * \param [in] routeLen length of the onion path
  * \param [in,out] cipher pointer to the ciphertext of the onion head
  * \param [in] cipherLen length in the number of bytes of the \p cipher
  * 
  * */

  void SendOnion (uint32_t firstHop, int routeLen, unsigned char *cipher, int cipherLen);

  uint16_t m_numnodes; //!<  The number of sensor nodes in the simulation
  uint32_t m_onionDelay; //!<  The sink will start sending onion messagess after OnionDelay seconds
  std::map<uint32_t, std::string>
      m_nodeManager; //!<  hashmap to manage data about nodes in the WSN// pair <IP,publickey>
  uint32_t m_decoyNum =
      1203; //!< dummy decoy value used to obfuscate the value carried in the onion body
  bool
      m_fixedOnionSize; //!< maintain the onion size fixed by adding padding (after layer decryption)
  enum BodyOptions m_bodyOptions; //<!  Specifies how the onion body must behave
  uint16_t m_bodySize; //<! the onion body size in bytes if ns3::BodyOptions is set appropriately

  //simulation task
  int m_repeateTimes; //!< Integer specifying the number of times to generate the onion message for each value in the m_onionPathsLengths
  int m_repeateCount = 0; //!< how many times the onion was sent at the current path length
  int m_onionLengthIndex = 0; //!< index of the current onion path length
  uint16_t *m_onionPathLengths; //!< array holding onion path lengths
  uint16_t m_numOnionLengths; //!< size of the array m_onionPathsLengths

  //onion sequence number
  int m_onionId = 1; //!< onion ID incremented each time a new onion is issued

  //VErbosity of onion
  std::stringstream m_onionStream; //!< a string stream holding the onion represented as a string

  //sink node keys
  std::string m_publickey; //!< the encryption key: publickey
  std::string m_secretkey; //!< the encryption key: secretkey
};

} // namespace ns3

#endif /* SINK_H */
