/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ONIONVALIDATOR_H
#define ONIONVALIDATOR_H

// //Constants
// #define AODV 0
// #define DSR  1
// #define OLSR 2
// #define DSDV 3

#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include <vector>
#include "ns3/output-stream-wrapper.h"
#include "ns3/internet-module.h"
#include <time.h>

namespace ns3 {

/**
 *  \defgroup managers
 * 
 *  This section documents the API of the ns-3 Managers module.
 *  For a functional description, please refer to the ns-3 manual here:
 *  http://www.nsnam.org/docs/models/html/managers.html
 *
 *  Be sure to read the manual BEFORE going down to the API.
 */

/**
 * \ingroup managers
 * \class OnionValidator
 * \brief   Class shared between wsn nodes used to track how the onion is transiting in the WSN
 * 			The class uses two counters \p m_onionSeq and \p m_hopCount to identify when an onion neeeds to be aborted
 *
 */

class OnionValidator : public Object
{

public:
  /**
  *
  * \brief Default constructor
  *
  */
  OnionValidator ();

  /**
  *
  * \brief Default destructor
  *
  */
  ~OnionValidator ();

  /**
 *  
 *  \return The object TypeId.
 */
  static TypeId GetTypeId (void);

  /**
  *
  * \brief  Called by the sink node, set up the \p m_onionSeq to the onionId value for keeping 
  * 		track of the onion
  * 
  * \param [in] seq_n the onion ID
  */

  void StartOnion (int seq_n);

  /**
  *
  * \brief  Called by the sink node to check if the onion is still running or is aborted
  * 
  * \return TRUE if the onion is running, FALSE if the onion was aborted
  */
  bool OnionStatus (void);

  /**
  *
  * \brief  Called by the sensor nodes when the \p m_onionTimeout elapses
  * 		IF \p m_hopCount is equal to \p hop then the onion was not received by the next hop 
  * 		Therefore abort the onion.
  * 
  * \param [in] hop the \p m_hopCount number set when the onion was forwarded to the next node
  * 
  * \return TRUE if the onion is running, FALSE if the onion was aborted
  */
  bool CheckOnionReceived (int hop);
  /**
  *
  * \brief  Return the current hop sequence number
  * 
  * \return return the hop count
  */
  int OnionHopCount (void);

  /**
  *
  * \brief  Return the current onion sequence number (the onionId)
  * 
  * \return return the onion sequence number
  */
  int GetOnionSeq (void);

  /**
  *
  * \brief  The onion was correctly received, increment the hop count.
  * 
  */
  void OnionReceived (void);

  int m_onionSeq = 0; //!< the onionId of the onion currently transiting the network

  int m_hopCount =
      0; //!< a sequence number incremented each time a node correctly receives an onion message
};

} // namespace ns3

#endif /* ONIONVALIDATOR_H */