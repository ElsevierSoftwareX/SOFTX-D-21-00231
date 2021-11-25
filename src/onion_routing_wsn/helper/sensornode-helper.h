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


#ifndef SENSORNODE_HELPER_H
#define SENSORNODE_HELPER_H

#include "ns3/sensornode.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/trace-helper.h"
#include "ns3/inet-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/string.h"
#include "ns3/names.h"
#include <stdint.h>
#include <string>
#include "ns3/uinteger.h"
#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/attribute-helper.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/application.h"
#include "ns3/outputmanager.h"
#include "ns3/onionvalidator.h"

namespace ns3 {


/**
 * \ingroup application-helper
 * \class SinkHelper
 * \brief Helper class for the creation of SensorNode applications
 *
 */


class SensorNodeHelper : public Object
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
  SensorNodeHelper ();

  /**
  *
  * \brief Constructor with attributes to set basic attributes of sensor nodes
  *
  */
  SensorNodeHelper (Ipv4Address address, Ptr<OutputManager> outputManager,
                    Ptr<OnionValidator> onionValidator);

  /**
  *
  * \brief Default constructor
  *
  */
  ~SensorNodeHelper ();

  /**
  *
  * \brief Setter of individual attributes
  *
  */

  void SetAttribute (std::string name, const AttributeValue &value);

  /*
   * Install an ns3::SensorNode application on the node configured with all the attributes set with SetAttribute.
   * \param NodeContainer
   * \returns ApplicationContainer
   *
   */
  ApplicationContainer Install (NodeContainer c) const;

  /*
   *Install an ns3::Sink application on the node configured with all the attributes set with SetAttribute.
   * \param node, the node on which an SensorNode will be installed
   * \returns Ptr to the installed application
   */

  Ptr<Application> InstallPriv (Ptr<Node> node) const;

private:
  Ipv4Address m_sinkAddress;  //!< Ipv4 address of the sink node
  Ptr<OutputManager> m_outputManager; //!< Manage the output of the simulation
  Ptr<OnionValidator> m_onionValidator; //!< Manage onions and when to abort them
  uint16_t m_onionTimeout; //!< A watchdog timer set to abort onion messagess, if the timer elepses before the onion returns back to the sink node
  ObjectFactory m_factory; //!< factory object
};

} // namespace ns3

#endif /* SENSORNODE_HELPER_H */
