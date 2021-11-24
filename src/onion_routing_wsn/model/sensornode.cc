/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "sensornode.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (SensorNode);

NS_LOG_COMPONENT_DEFINE ("sensornode");

TypeId
SensorNode::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SensorNode")
                          .SetParent<Wsn_node> ()
                          .AddConstructor<SensorNode> ()
                          .AddAttribute ("SinkNodeAddress", "Address to send packets.",
                                         Ipv4AddressValue (Ipv4Address::GetAny ()),
                                         MakeIpv4AddressAccessor (&SensorNode::m_sinkAddress),
                                         MakeIpv4AddressChecker ());

  return tid;
}

SensorNode::SensorNode ()
{
}

SensorNode::~SensorNode ()
{
  m_socket = 0;
}

//Send the public key to the source

void
SensorNode::Handshake ()
{
  std::string pk = m_onionManager.GetPKtoString ();

  //construct a new packet /w publickey type of sensor
  protomessage::ProtoPacket handshake_message;
  handshake_message.mutable_h_shake ()->set_publickey (pk); //publickey
  SerializationWrapper sw (handshake_message);
  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (sw);

  //send to the sink node
  InetSocketAddress remote (m_sinkAddress, m_port);
  Wsn_node::SendSegment (remote, p, false);
}

//callback, when the onion is received
void
SensorNode::ReceivePacket (Ptr<Socket> socket)
{

  Ptr<Packet> p = Wsn_node::RecvSegment (socket);

  if (p != NULL)
    {
      SerializationWrapper sw;
      //get the onion message
      protomessage::ProtoPacket onion;
      p->RemoveHeader (sw);
      sw.GetData (&onion);

      //get the onion ID
      o_sequenceNum = onion.mutable_o_head ()->onionid ();

      if (o_sequenceNum == m_onionValidator->GetOnionSeq ())
        { //message IDs are equal

          //Call that the onion was received
          m_outputManager->OnionRoutingRecv (Simulator::Now ());
          Wsn_node::OnionReceived ();

          //Process onion head and get next hop IP address
          uint32_t ip = ProcessOnionHead (onion.mutable_o_head ());

          //ProcessOnionHead();
          ProcessOnionBody (onion.mutable_o_body ());

          //create the packet
          Ptr<Packet> np = Create<Packet> ();
          sw.SetData (onion);
          np = Create<Packet> ();
          np->AddHeader (sw);

          //send further the message
          InetSocketAddress remote (Ipv4Address (ip), m_port);
          Wsn_node::SendSegment (remote, np, true);

          ///Log details about the onion
          m_outputManager->OnionRoutingSend (
              m_address, Ipv4Address (ip), np->GetSize (), onion.mutable_o_head ()->ByteSizeLong (),
              onion.mutable_o_body ()->ByteSizeLong (), Simulator::Now ());
        }
      else
        { //the onion should be deleted
          NS_LOG_INFO ("Ghost onion received, deleted with onion id: "
                       << o_sequenceNum << ", at ip: " << m_address
                       << ", at time: " << std::to_string (Simulator::Now ().GetSeconds ()));
        }
    }
}

uint32_t
SensorNode::ProcessOnionHead (protomessage::ProtoPacket_OnionHead *onionHead)
{
  std::string onion = onionHead->onion_message ();

  //get length in bytes of the onion
  int outer_layer_len = onion.length ();

  //get previous padding
  if (onionHead->has_padding ())
    {
      outer_layer_len += onionHead->padding ().length ();
    }

  //decrypt the onion
  unsigned char *serialized_onion = m_onionManager.StringToUchar (onion);
  orLayer *onionLayer = m_onionManager.PeelOnion (serialized_onion, onion.length (),
                                                  m_onionManager.GetPK (), m_onionManager.GetSK ());

  //convert ip from the onion to uint32
  uint32_t ip = DeserializeIpv4ToInt (onionLayer->nextHopIP);
  //convert onion to string for serialization
  onion = m_onionManager.UcharToString (onionLayer->innerLayer, onionLayer->innerLayerLen);

  //mount the onion head
  onionHead->set_onion_message (onion);

  //add padding if set
  if (onionHead->has_padding ())
    {
      std::string padding (outer_layer_len - onion.length (), '0');
      onionHead->set_padding (padding);
    }

  return ip;
}

void
SensorNode::ProcessOnionBody (protomessage::ProtoPacket_OnionBody *onionBody)
{
  if (onionBody->has_aggregatedvalue ())
    {
      //get the content - payload
      int value = onionBody->aggregatedvalue ();
      // Compute the aggregate
      value = value + m_sensorValue;
      //set the new value
      onionBody->set_aggregatedvalue (value);
    }
}

uint32_t
SensorNode::DeserializeIpv4ToInt (uint8_t *buff)
{
  uint32_t ip = 0;
  ip += buff[0];
  ip = ip << 8;
  ip += buff[1];
  ip = ip << 8;
  ip += buff[2];
  ip = ip << 8;
  ip += buff[3];
  return ip;
}

//callback, at a new connection
void
SensorNode::Accept (Ptr<Socket> socket, const ns3::Address &from)
{
  socket->SetRecvCallback (MakeCallback (&SensorNode::ReceivePacket, this));
}

// executes at start
void
SensorNode::StartApplication (void)
{
  //basic configuration
  Wsn_node::Configure ();

  //Generate encryption keys
  m_onionManager.GenerateNewKeyPair ();

  uint32_t delay = Wsn_node::getNodeDelay (m_address);

  Simulator::Schedule (MilliSeconds (delay), &SensorNode::Handshake, this);

  //get the new connection
  m_socket->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
                               MakeCallback (&SensorNode::Accept, this));
}

void
SensorNode::StopApplication (void)
{
  if (m_socket)
    {
      m_socket->Close ();
    }
}

} // namespace ns3
