/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef SERIALIZATIONWRAPPER_H
#define SERIALIZATIONWRAPPER_H

#include <fstream>
#include <iostream>
#include <string>
#include "ns3/header.h"
#include "ns3/network-module.h"
#include "ns3/packet.h"
#include "ns3/proto-packet.pb.h"

namespace ns3 {

/**
 * \ingroup onion_routing_wsn
 * 
 * \class SerializationWrapper
 * \brief Class for the serialization-deserialization of the messagess to send in packets
 *
 */

class SerializationWrapper : public Header
{
public:
  /**
  *
  * \brief Setter of the data in the protocol header
  * 
  * \param [in] message the protobuff object containing the data to transmit
  *
  */
  void SetData (protomessage::ProtoPacket message);

  /**
  *
  * \brief Getter of the data in the protocol header
  * 
  * \param [in,out] message pointer to the protobuff object for storing data
  *
  */
  void GetData (protomessage::ProtoPacket *message);

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
  SerializationWrapper ();

  virtual ~SerializationWrapper ();

  /**
  *
  * \brief Constructor with argument
  * 
  * \param [in] message the protobuff object containing the data to transmit
  *
  */
  SerializationWrapper (protomessage::ProtoPacket message);

  /**
 *  
 *  \return The object TypeId.
 */
  virtual TypeId GetInstanceTypeId (void) const;

  /**
 * 
 *  \brief compute the serialized size of the data
 *  
 *  \return return the size of the serialized data in bytes
 */
  virtual uint32_t GetSerializedSize (void) const;

  /**
 * 
 *  \brief serialize the data
 *  
 *  \param [in,out] start	an iterator which points to where the data should be written.
 * 
 */
  virtual void Serialize (Buffer::Iterator start) const;
  /**
 * 
 *  \brief deserialize the data
 * 
 *  \param [in,out] start	an iterator which points to where the data should read from.
 * 
 *  \return the number of bytes read.
 *  
 */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
 * 
 *  \brief dummy printing of  the serialized data
 * 
 */
  virtual void Print (std::ostream &os) const;

  int m_dataSize = 0; //!< holds the size of the serialized data in bytes

  uint8_t *m_data; //!< the serialized data
};
} // namespace ns3

#endif /* SERIALIZATIONWRAPPER_H */