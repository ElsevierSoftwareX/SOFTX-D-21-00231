/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ENUMS_H
#define ENUMS_H

namespace ns3 {

/**
 * 
 * \ingroup onion_routing_wsn
 * \enum Routing
 * \brief Enumeration defining different routing algorithms that can be used in the simulator
 */

enum Routing {
  AODV = 0, //!< Ad Hoc On-Demand Distance Vector ns3::Aodv
  DSR, //!< Dynamic Source Routing ns3::Dsr
  OLSR, //!< Optimized Link State Routing Protocol ns3::Olsr
  DSDV //!< Destination-Sequenced Distance Vector routing ns3::Dsdv
};

/**
 * 
 * \ingroup onion_routing_wsn
 * \enum Topology
 * \brief Enumeration defining different network topologies that can be used in the simulation
 */

enum Topology {
  GRID = 0, //!< Grid topology
  DISC //!< Random disc topology
};

/**
 * 
 * \ingroup onion_routing_wsn
 * \enum IEEE_80211n
 * \brief Enumeration defining different carrier frequencies of the standard IEEE 802.11n 
 */

enum IEEE_80211n {
  F_24GHz = 0, //!< 2.4GHz
  F_5GHz //!< 5GHz
};

/**
 * 
 * \ingroup onion_routing_wsn
 * \enum Verbosity
 * \brief Verbosity settings of the simulation
 */

enum Verbosity {
  NO =
      0, //!< No output on console log, except notifying simulation start and end, output data in csv file
  ConsoleLog, //!< Output data and simulation description on console log, output data in csv file
  PrintDescription, //!< No output on console log, print simulation description and data on csv file
  Both //!< print all simulation output on console log and csv file
};

/**
 * 
 * \ingroup onion_routing_wsn
 * \enum BodyOptions
 * \brief Specifies how the onion body must behave
 */

enum BodyOptions {
  NO_Body = 0, //!< The onion message won't have an onion body
  Aggregate, //!< The onion body will only aggregate a value
  FixedSize, //!< the onion body will have a fixed size specified by the ns3::Sink::BodySize attribute
  AggregateAndFixed //!< The onion body will aggregate a value and will maintain a fixed size apecified by the ns3::Sink::BodySize attribute
};

} // namespace ns3

#endif /* ENUMS_H */