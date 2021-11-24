/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef SEGMENTNUM_H
#define SEGMENTNUM_H

#include "ns3/object-base.h"
#include "ns3/tag.h"
#include <stdint.h>

namespace ns3 {

/**
 * \ingroup onion_routing_wsn
 * 
 * \class SegmentNum
 * \brief Class for adding a tag to packets used to track different segments of packets packets are fragmented due to small MSS
 *        Just implementing methods from ns3 class ns3::Tag
 */

class SegmentNum : public Tag
{
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  SegmentNum ();
  SegmentNum (uint32_t seg_num);
  ~SegmentNum ();

  uint32_t GetSegNum ();
  void SetSegNum (uint32_t seg_num);

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  virtual void Print (std::ostream &os) const;

  //segment number
  uint32_t s_num;
};

} // namespace ns3

#endif /* SEGMENTNUM_H */