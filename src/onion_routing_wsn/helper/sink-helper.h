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


#ifndef SINK_HELPER_H
#define SINK_HELPER_H

#include "ns3/sink.h"

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
 *  \defgroup application-helper
 * 
 *
 *  Be sure to read repository README.md before going down to the API.
 */

/**
 * \ingroup application-helper
 * \class SinkHelper
 * \brief Helper class for the creation of Sink applications
 *
 */

class SinkHelper : public Object
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
  SinkHelper ();

  /**
  *
  * \brief Constructor with attributes to set basic attributes of the sink node
  *
  */
  SinkHelper (uint16_t numnodes, Ptr<OutputManager> outputManager,
              Ptr<OnionValidator> onionValidator, uint16_t *m_onionPathLengths);

  /**
  *
  * \brief Default constructor
  *
  */
  ~SinkHelper ();

  /**
  *
  * \brief Setter of individual attributes
  *
  */

  void SetAttribute (std::string name, const AttributeValue &value);

  /*
   * Install an ns3::Sink application on the node configured with all the attributes set with SetAttribute.
   * \param NodeContainer
   * \returns ApplicationContainer
   *
   */

  ApplicationContainer Install (NodeContainer c) const;

  /*
   *Install an ns3::SensorNode application on the node configured with all the attributes set with SetAttribute.
   * \param node, the node on which an Sink will be installed
   * \returns Ptr to the installed application
   */

  Ptr<Application> InstallPriv (Ptr<Node> node) const;

private:
  uint16_t m_numnodes; //!< Number of sensor nodes in the network
  ObjectFactory m_factory; //!< factory object
  Ptr<OutputManager> m_outputManager; //!< Manage the output of the simulation
  Ptr<OnionValidator> m_onionValidator; //!< Manage onions and when to abort them
  uint16_t
      m_onionTimeout; //!< A watchdog timer set to abort onion messagess, if the timer elepses before the onion returns back to the sink node
};

} // namespace ns3

#endif /* SINK_HELPER_H */
