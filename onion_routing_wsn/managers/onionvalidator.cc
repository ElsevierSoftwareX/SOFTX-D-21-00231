/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "onionvalidator.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (OnionValidator);

NS_LOG_COMPONENT_DEFINE ("oniionvalidator");

TypeId
OnionValidator::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::OnionValidator").SetParent<Object> ().AddConstructor<OnionValidator> ();
  return tid;
}

OnionValidator::OnionValidator ()
{
}

OnionValidator::~OnionValidator ()
{
}

////Oniuon status checking

void
OnionValidator::StartOnion (int seq_n)
{
  m_onionSeq = seq_n;
}

/*
Returns information about the state of the onion

true - running
false - onion aborted

*/

bool
OnionValidator::OnionStatus (void)
{

  if (m_onionSeq == 0)
    {
      return false;
    }
  else
    {
      return true;
    }
}

/*
Check if onion at hop was received, othervise abort sending

*/

bool
OnionValidator::CheckOnionReceived (int hop)
{

  if (hop == this->m_hopCount)
    {
      //std::cout << "Onion aborted, at hop: "<< hop << " at time: " << Simulator::Now().GetSeconds() << std::endl;
      //abort sending the onion
      m_onionSeq = 0; // 0 is sequence 0 null value!!! onion will be deleted
      return false;
    }
  return true;
}

//changes state of the onion status
int
OnionValidator::OnionHopCount (void)
{
  return this->m_hopCount;
}

//Get the onion sequence number
int
OnionValidator::GetOnionSeq (void)
{
  return m_onionSeq;
}

//signals that the onion was completely received
void
OnionValidator::OnionReceived (void)
{
  this->m_hopCount++;
}

} // namespace ns3