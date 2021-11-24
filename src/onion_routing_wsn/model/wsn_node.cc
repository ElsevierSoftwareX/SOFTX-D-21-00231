/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "wsn_node.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (Wsn_node);

TypeId
Wsn_node::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::Wsn_node")
          .SetParent<Application> ()
          .AddConstructor<Wsn_node> ()
          .AddAttribute ("ListenerPort", "Port on which we listen for incoming packets.",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue (4242), MakeUintegerAccessor (&Wsn_node::m_port),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("OutputManager", "Manage the output of the simulation", PointerValue (0),
                         MakePointerAccessor (&Wsn_node::m_outputManager),
                         MakePointerChecker<OutputManager> ())
          .AddAttribute ("OnionValidator", "Manage onions and when to abort them", PointerValue (0),
                         MakePointerAccessor (&Wsn_node::m_onionValidator),
                         MakePointerChecker<OnionValidator> ())
          .AddAttribute ("Delay", "Starting delay of sensor nodes, delay is given in milliseconds",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue (200), MakeUintegerAccessor (&Wsn_node::m_delay),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("MSS", "Maximum segment size",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue (536), MakeUintegerAccessor (&Wsn_node::f_mss),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("OnionTimeout",
                         "A watchdog timer set to abort onion messagess, if the timer elepses "
                         "before the onion returns back to the sink node",
                         TypeId::ATTR_CONSTRUCT | TypeId::ATTR_SET | TypeId::ATTR_GET,
                         UintegerValue (100), MakeUintegerAccessor (&Wsn_node::m_onionTimeout),
                         MakeUintegerChecker<uint16_t> ());

  return tid;
}

Wsn_node::Wsn_node ()
{
}

void
Wsn_node::StartApplication ()
{
}

void
Wsn_node::StopApplication ()
{
}

void
Wsn_node::Configure ()
{
  //Get the address of the node on which the application is running
  Ptr<Node> PtrNode = this->GetNode ();
  Ptr<Ipv4> ipv4 = PtrNode->GetObject<Ipv4> ();
  Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1, 0);
  Ipv4Address address = iaddr.GetLocal ();
  m_address = address;

  //TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
  //m_socket = Socket::CreateSocket (GetNode(),tid);
  m_socket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId ());
  //m_socket.SetInitialCwnd(20)
  //socket to listen
  InetSocketAddress local (Ipv4Address::GetAny (), m_port);
  m_socket->SetIpRecvTtl (true);
  m_socket->Bind (local);
  m_socket->Listen ();

  //Cordinates of the node
  Ptr<MobilityModel> mob = PtrNode->GetObject<MobilityModel> ();
  double coord_x = mob->GetPosition ().x;
  double coord_y = mob->GetPosition ().y;
  //mob->SetPosition(Vector(coord_x+1,coord_y+1,0));

  //if the routing is OLSR we can print out the number of one-hop neighbours from routing info
  if (m_outputManager->GetRouting () == Routing::OLSR)
    {
      //need to wait some seconds till routes are stable
      Simulator::Schedule (Seconds (5), &Wsn_node::NodeDegree, this, coord_x, coord_y);
    }
  else
    {
      m_outputManager->AddNodeDetails (m_address, coord_x, coord_y);
    }
}

/*
*
*	print out the number of one hop-neighbours of a node
* only valid in OLSR, for other routing you need to define another method
*
*/

void
Wsn_node::NodeDegree (double coord_x, double coord_y)
{

  Ptr<Node> PtrNode = this->GetNode ();
  ///Get one hop neighbours:
  Ptr<Ipv4> ipv4Ptr = PtrNode->GetObject<Ipv4> ();
  Ptr<Ipv4RoutingProtocol> ipv4Routing = ipv4Ptr->GetRoutingProtocol ();
  // Obtain the Ipv4ListRouting interface
  Ptr<Ipv4ListRouting> ipv4ListRouting = DynamicCast<Ipv4ListRouting> (ipv4Routing);
  // A list routing protocol has multiple Ipv4Routing protocols. Find OLSR in this list
  Ptr<olsr::RoutingProtocol> olsrProtocol;
  int16_t priority = 10;
  for (uint32_t i = 0; i < ipv4ListRouting->GetNRoutingProtocols (); i++)
    {
      Ptr<Ipv4RoutingProtocol> proto = ipv4ListRouting->GetRoutingProtocol (0, priority);
      olsrProtocol = DynamicCast<olsr::RoutingProtocol> (proto);
    }
  if (olsrProtocol != 0)
    {
      //break; // found the protocol we are looking for
    }
  else
    {
      NS_ASSERT_MSG (olsrProtocol, "Didn't find OLSR on this node");
    }

  ns3::olsr::OlsrState state = olsrProtocol->GetOlsrState ();
  ns3::olsr::NeighborSet n_set = state.GetNeighbors ();

  m_outputManager->AddNodeDetails (m_address, coord_x, coord_y, n_set.size ());
}

//calculate when the node will start based on the given address
//allow nodes to start sequentially
uint32_t
Wsn_node::getNodeDelay (Ipv4Address node_address)
{
  //Fixed delay
  Ipv4Address net_address;
  net_address.Set ("10.1.0.0");
  uint32_t delay = node_address.Get () - net_address.Get ();
  delay = delay * m_delay; //each node starts consequently after delay_ms

  return delay;
}

/*
*	Send a packet as a TCP segment to the remote node
*	add a tag to the packet, which defines the size of the whole packet
*	segment size is limited by MSS or MTU
*	The packet if to large is automatically splitted in many segments
*/

void
Wsn_node::SendSegment (InetSocketAddress remote, Ptr<Packet> packet, bool b_onion)
{

  Ptr<Socket> socket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId ());
  socket->Connect (remote);

  int pack_size = packet->GetSize ();
  int seg_num = pack_size / f_mss; //f_seg_size;//PAZIII!!! morda problemi zs packet size

  if (b_onion)
    {
      this->o_hopCount = m_onionValidator->OnionHopCount ();
      Simulator::Schedule (Seconds (m_onionTimeout), &Wsn_node::CheckSentOnion, this,
                           this->o_hopCount);
    }

  if (seg_num == 0)
    { //packet fits in segment size, just send-it
      //std::cout << "Small pack" <<std::endl;
      socket->Send (packet);
    }
  else
    {

      SegmentNum s_num (pack_size);
      packet->AddByteTag (s_num);
      socket->Send (packet);
      //socket->SetDataSentCallback(MakeCallback(&Wsn_node::meter,this));
    }
  //std::cout << "Sent" <<std::endl;
}

//receive a segment

Ptr<Packet>
Wsn_node::RecvSegment (Ptr<Socket> socket)
{
  Address from;
  Ptr<Packet> p = Create<Packet> ();

  p = socket->RecvFrom (from);

  return RecvSeg (socket, p, from);
}

//receive segment and store the sender's address

Ptr<Packet>
Wsn_node::RecvSegment (Ptr<Socket> socket, Address &from)
{
  Ptr<Packet> p = Create<Packet> ();
  p = socket->RecvFrom (from);

  return RecvSeg (socket, p, from);
}

/*
*	Receive a segment
*	The tag represents the size of the whole packet to be received
*	merge parts into the whole packet
*	pending_packet is the buffer where you aggregate segments
*
*/

Ptr<Packet>
Wsn_node::RecvSeg (Ptr<Socket> socket, Ptr<Packet> p, Address from)
{

  InetSocketAddress from_address = InetSocketAddress::ConvertFrom (from);

  //std::cout << "recv" <<std::endl;
  SegmentNum s_num;
  //if only one packet
  if (!p->FindFirstMatchingByteTag (s_num))
    {
      //tag not found
      socket->Close ();
      return p;
    }

  //std::stringstream ss;
  //p->PrintByteTags (ss);
  // std::cout << ss.str() << " at: " << std::to_string(Simulator::Now().GetSeconds ())<<std::endl;

  if (f_pendingPacket == NULL || from_address.GetIpv4 ().Get () != f_receivingAddress.Get ())
    {
      f_receivingAddress = from_address.GetIpv4 (); //Ipv4Address(0);
      f_pendingPacket = Create<Packet> ();
      f_segmentSize = s_num.GetSegNum ();
    }
  f_pendingPacket->AddAtEnd (p);

  f_segmentSize = f_segmentSize - p->GetSize ();

  if (f_segmentSize == 0)
    {
      p = f_pendingPacket->Copy ();
      f_receivingAddress = Ipv4Address::GetAny ();
      f_pendingPacket = NULL;
      socket->Close ();
      return p;
    }
  else
    {
      return NULL;
    }
}

///Checking onion

//Check onion sending
void
Wsn_node::CheckSentOnion (int count)
{
  //std::cout << "Hop count: " << o_hopCount << "count: " << count <<std::endl;
  if (!m_onionValidator->CheckOnionReceived (count))
    {
      m_outputManager->AbortOnion (Simulator::Now ());
    }
}

//Signal, that the whole onion was received
void
Wsn_node::OnionReceived (void)
{

  //std::cout << "Onion received " << onion_seq << " real seq:" <<  m_simManager->getOnionSeq() <<std::endl;
  m_onionValidator->OnionReceived ();
}

} // namespace ns3
