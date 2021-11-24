/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

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
