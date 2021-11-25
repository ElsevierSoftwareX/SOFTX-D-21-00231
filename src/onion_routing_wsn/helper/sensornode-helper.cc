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


#include "sensornode-helper.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (SensorNodeHelper);

TypeId
SensorNodeHelper::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::SensorNodeHelper").SetParent<Object> ().AddConstructor<SensorNodeHelper> ();
  return tid;
}

SensorNodeHelper::SensorNodeHelper ()
{
  m_factory.SetTypeId ("ns3::SensorNode");
}

SensorNodeHelper::SensorNodeHelper (Ipv4Address address, Ptr<OutputManager> outputManager,
                                    Ptr<OnionValidator> onionValidator)
    : m_outputManager (outputManager), m_onionValidator (onionValidator)
{
  m_factory.SetTypeId ("ns3::SensorNode");
  m_factory.Set ("SinkNodeAddress", Ipv4AddressValue (address));
  m_factory.Set ("OutputManager", PointerValue (m_outputManager));
  m_factory.Set ("OnionValidator", PointerValue (m_onionValidator));
}

SensorNodeHelper::~SensorNodeHelper ()
{
}

void
SensorNodeHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
SensorNodeHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
SensorNodeHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();

  node->AddApplication (app);

  return app;
}

} // namespace ns3
