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


#include "sink-helper.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (SinkHelper);

TypeId
SinkHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SinkHelper").SetParent<Object> ().AddConstructor<SinkHelper> ();

  return tid;
}

SinkHelper::SinkHelper () : m_numnodes (0)
{
  m_factory.SetTypeId ("ns3::Sink");
}

SinkHelper::SinkHelper (uint16_t numnodes, Ptr<OutputManager> outputManager,
                        Ptr<OnionValidator> onionValidator, uint16_t *m_onionPathLengths)
    : m_numnodes (numnodes), m_outputManager (outputManager), m_onionValidator (onionValidator)
{
  m_factory.SetTypeId ("ns3::Sink");
  m_factory.Set ("NumNodes", UintegerValue (m_numnodes));
  m_factory.Set ("OutputManager", PointerValue (m_outputManager));
  m_factory.Set ("OnionValidator", PointerValue (m_onionValidator));
}

SinkHelper::~SinkHelper ()
{
}

void
SinkHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
SinkHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
SinkHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();

  node->AddApplication (app);

  return app;
}

} // namespace ns3
