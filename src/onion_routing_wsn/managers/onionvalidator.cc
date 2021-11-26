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