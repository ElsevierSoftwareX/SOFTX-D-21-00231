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