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

#include "sink.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (Sink);

NS_LOG_COMPONENT_DEFINE ("sink");

TypeId
Sink::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::Sink")
          .SetParent<Wsn_node> ()
          .AddConstructor<Sink> ()
          .AddAttribute ("NumNodes", "The number of sensor nodes in the simulation.",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue (0), MakeUintegerAccessor (&Sink::m_numnodes),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute (
              "OnionDelay", "The sink will start sending onion messagess after OnionDelay seconds.",
              TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET, UintegerValue (5),
              MakeUintegerAccessor (&Sink::m_onionDelay), MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("FixedOnionSize", "Maintain a fixed onion size by adding padding.",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         BooleanValue (true), MakeBooleanAccessor (&Sink::m_fixedOnionSize),
                         MakeBooleanChecker ())
          .AddAttribute ("BodyOptions", "Modify the behaviour of the onion body",
                         EnumValue (BodyOptions::NO_Body), MakeEnumAccessor (&Sink::m_bodyOptions),
                         MakeEnumChecker (BodyOptions::NO_Body, "nobody", BodyOptions::Aggregate,
                                          "aggregate", BodyOptions::FixedSize, "fixed",
                                          BodyOptions::AggregateAndFixed, "both"))
          .AddAttribute (
              "BodySize", "Size of the onion body maintained fixed during the simulation",
              TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET, UintegerValue (128),
              MakeUintegerAccessor (&Sink::m_bodySize), MakeUintegerChecker<uint16_t> ());

  return tid;
}

Sink::Sink ()
{
}

Sink::~Sink ()
{
  m_socket = 0;
}

void
Sink::Setup (uint16_t *onionPathlengths, uint16_t numOnionLengths, int repeateTimes)
{
  this->m_onionPathLengths = new uint16_t[numOnionLengths];
  for (int i = 0; i < numOnionLengths; ++i)
    {
      this->m_onionPathLengths[i] = onionPathlengths[i];
    }
  this->m_numOnionLengths = numOnionLengths;
  this->m_repeateTimes = repeateTimes;
}

//callback, at a new connection

void
Sink::Accept (Ptr<Socket> socket, const ns3::Address &from)
{
  socket->SetRecvCallback (MakeCallback (&Sink::ReceivePacket, this));
}

//callback, when a new packet is received

void
Sink::ReceivePacket (Ptr<Socket> socket)
{
  Address from;
  Ptr<Packet> p = Wsn_node::RecvSegment (socket, from);

  if (p != NULL)
    {

      InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
      SerializationWrapper sw;
      p->RemoveHeader (sw);
      protomessage::ProtoPacket message;
      sw.GetData (&message);

      if (message.has_h_shake ())
        {
          //register node
          RecvHandshake (message.mutable_h_shake (), address);
        }
      else //onion message
        {
          //get the onion ID
          o_sequenceNum = message.mutable_o_head ()->onionid ();

          //
          if (o_sequenceNum == m_onionId - 1)
            { //message IDs are equal, -1 since m_onionId is larger for one value

              //call that onion was received
              Wsn_node::OnionReceived ();

              RecvOnion (message.mutable_o_body ());
            }
          else
            { //the onion should be deleted
              NS_LOG_INFO ("Ghost onion received, deleted with onion id: "
                           << o_sequenceNum << ", at ip: " << m_address
                           << ", at time: " << std::to_string (Simulator::Now ().GetSeconds ()));
            }
        }
    }
}

//execute when a new node register on the sink

void
Sink::RecvHandshake (protomessage::ProtoPacket_Handshake *handshake_message, InetSocketAddress from)
{
  std::string pk = handshake_message->publickey ();
  m_nodeManager[from.GetIpv4 ().Get ()] = pk;

  //print output to file
  m_outputManager->NewHandshake (m_nodeManager.size () - 1, from.GetIpv4 (), Simulator::Now ());
}

//Execute at the recv of the onion

void
Sink::RecvOnion (protomessage::ProtoPacket_OnionBody *onion_body)
{
  m_outputManager->OnionRoutingRecv (Simulator::Now ());
  m_outputManager->RecvOnion (Simulator::Now ());
  Simulator::Schedule (Seconds (0.5), &Sink::SinkTasks, this);
}

//defines the behaviour of the source how many OR sends, how to build the route ecc...
void
Sink::SinkTasks ()
{

  if (m_onionLengthIndex < m_numOnionLengths)
    {

      if (m_repeateCount < m_repeateTimes)
        {

          //selet the route of the onion
          int route[m_onionPathLengths[m_onionLengthIndex]];
          SelectRoute (route, m_onionPathLengths[m_onionLengthIndex]);
          //send the onion
          PrepareOnion (route, m_onionPathLengths[m_onionLengthIndex]);

          m_repeateCount++;
        }
      else
        {
          m_repeateCount = 0;
          m_onionLengthIndex++;
          SinkTasks (); // call again with new params
        }
    }
  else
    {
      //simulation ended, can print details of nodes
      m_outputManager->PrintNodeDetails (m_nodeManager);

      //end simulation
      Simulator::Stop ();
    }
}

//builds randomly the route to the sensor and back
//the length of the route is static!
//the route can have loops, but each node must not be placed consequently in the route
// the route must be of lentgh at least 3
void
Sink::SelectRoute (int *route, int routeLen)
{

  assert (routeLen >= 3); // the route must be of lentgh at least 3

  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  int node_id;

  //previous id -> previous and current must not be equal
  int prev_id = -1;

  //build the route
  int i = routeLen - 1;
  while (i >= 0)
    {
      node_id = x->GetInteger (0, m_nodeManager.size () - 1);
      if (node_id != prev_id)
        {
          route[i] = node_id;
          i--;
        }
      prev_id = node_id;
    }
}

//prepare the route to call the onion routing on the new class or.h

void
Sink::PrepareOnion (int *route, int routeLen)
{
  int cipherLen = m_onionManager.OnionLength (routeLen + 1, 0, 0);
  unsigned char cipher[cipherLen];

  unsigned char *ipRoute[routeLen + 1];

  unsigned char *keys[routeLen + 1];

  //sink details
  keys[routeLen] = m_onionManager.StringToUchar (m_publickey);
  ipRoute[routeLen] =
      m_onionManager.IpToBuff (m_address.Get ()); //set sink node as the last node in the onion path

  //define an iterator
  std::map<uint32_t, std::string>::iterator item;

  //fill other addresses and publickeys
  for (int i = 0; i < routeLen; ++i)
    {
      item = m_nodeManager.begin ();
      std::advance (item, route[i]);

      ipRoute[i] = m_onionManager.IpToBuff (item->first);

      keys[i] = m_onionManager.StringToUchar (item->second);
    }

  m_onionManager.BuildOnion (cipher, ipRoute, keys, routeLen + 1);

  item = m_nodeManager.begin ();
  std::advance (item, route[0]);
  SendOnion (item->first, routeLen, cipher, cipherLen);
}

void
Sink::SendOnion (uint32_t firstHop, int routeLen, unsigned char *cipher, int cipherLen)
{
  //first convert cipher to string!!
  std::string str_cipher = m_onionManager.UcharToString (cipher, cipherLen);
  protomessage::ProtoPacket onion;

  //Create the onion head
  onion.mutable_o_head ()->set_onion_message (str_cipher);
  onion.mutable_o_head ()->set_onionid (m_onionId);

  // fixed onion head size
  if (m_fixedOnionSize)
    {
      onion.mutable_o_head ()->set_padding ("");
    }

  //Specify how the onion body must behave
  std::string padding (m_bodySize, '0');
  switch (m_bodyOptions)
    {
    case BodyOptions::NO_Body:
      break;
    case BodyOptions::Aggregate:
      onion.mutable_o_body ()->set_aggregatedvalue (m_decoyNum);
      break;
    case BodyOptions::FixedSize:
      onion.mutable_o_body ()->set_padding (padding);
      break;
    case BodyOptions::AggregateAndFixed:
      onion.mutable_o_body ()->set_aggregatedvalue (m_decoyNum);
      onion.mutable_o_body ()->set_padding (padding);
      break;
    }

  //Create the packet
  SerializationWrapper sw (onion);
  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (sw);

  InetSocketAddress remote = InetSocketAddress (Ipv4Address (firstHop), m_port);

  Wsn_node::SendSegment (remote, p, true);

  m_onionValidator->StartOnion (m_onionId);
  //increment onion sequence number
  m_onionId++;

  m_outputManager->SendOnion (p->GetSize (), onion.mutable_o_head ()->ByteSizeLong (),
                              onion.mutable_o_body ()->ByteSizeLong (), routeLen,
                              Simulator::Now ());
  m_outputManager->OnionRoutingSend (m_address, Ipv4Address (firstHop), p->GetSize (),
                                     onion.mutable_o_head ()->ByteSizeLong (),
                                     onion.mutable_o_body ()->ByteSizeLong (), Simulator::Now ());
}

//Check if the onion is still in the network and valid each parameter milliseconds
void
Sink::CheckOnion (void)
{
  //no onion running then -> send an onion
  if (!m_onionValidator->OnionStatus ())
    { //Onion was aborted start a new one

      m_repeateCount--; // decrease value of m_repeateCount to re do the same onion size
      SinkTasks ();
    }

  //reset the the timer
  Simulator::Schedule (Seconds (5), &Sink::CheckOnion, this);
}

// executes at the start of the application
void
Sink::StartApplication (void)
{
  //basic configuration
  Wsn_node::Configure ();
  m_onionManager.GenerateNewKeyPair ();
  m_publickey = m_onionManager.GetPKtoString ();
  m_secretkey = m_onionManager.GetSKtoString ();

  //sprejme nov connection izvede callback
  m_socket->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
                               MakeCallback (&Sink::Accept, this));

  m_onionDelay = m_delay * m_numnodes + 5000;

  Simulator::Schedule (MilliSeconds (m_onionDelay), &Sink::SinkTasks, this);

  //start checkin if the onion is alive, and check each 5 seconds if it is alive
  Simulator::Schedule (MilliSeconds (m_onionDelay + 5000), &Sink::CheckOnion, this);
}

void
Sink::StopApplication (void)
{
  if (m_socket)
    {
      m_socket->Close ();
    }
}

} // namespace ns3
