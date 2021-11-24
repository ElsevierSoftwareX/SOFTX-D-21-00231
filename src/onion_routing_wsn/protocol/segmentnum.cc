/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/segmentnum.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SegmentNum);

TypeId
SegmentNum::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SegmentNum")
                      .SetParent<Tag> ()
                      .SetGroupName("Network")
                      .AddConstructor<SegmentNum> ();
  return tid;
}


TypeId
SegmentNum::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


SegmentNum::SegmentNum()
{
  s_num = 0;
}

SegmentNum::SegmentNum(uint32_t seg_num)
{
  s_num = seg_num;
}

SegmentNum::~SegmentNum() {}

uint32_t SegmentNum::GetSegNum()
{
  return s_num;
}

void SegmentNum::SetSegNum(uint32_t seg_num)
{
  s_num = seg_num;
}


uint32_t
SegmentNum::GetSerializedSize (void) const
{
  return 4;
}
void
SegmentNum::Serialize (TagBuffer i) const
{
  i.WriteU32 (s_num);
}
void
SegmentNum::Deserialize (TagBuffer i)
{
  s_num = i.ReadU32 ();
}
void
SegmentNum::Print (std::ostream &os) const
{
  os << "Segment num=" << (uint32_t) s_num;
}


} // namespace ns3