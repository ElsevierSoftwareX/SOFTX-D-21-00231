#include "serializationwrapper.h"
#include "ns3/header.h"
#include "ns3/network-module.h"

namespace ns3 {

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (SerializationWrapper);

TypeId
SerializationWrapper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SerializationWrapper")
                          .SetParent<Header> ()
                          .AddConstructor<SerializationWrapper> ()
                          .SetGroupName ("Network");
  return tid;
}

//nic vaznega neznam kaj rabi - obvezno, ker je bilo virtual
TypeId
SerializationWrapper::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

SerializationWrapper::SerializationWrapper ()
{
}

SerializationWrapper::~SerializationWrapper ()
{
  delete[] m_data;
}

//nastavi protobuf object
void
SerializationWrapper::SetData (protomessage::ProtoPacket message)
{
  //onion_message = onionmessage;

  m_dataSize = message.ByteSizeLong ();

  m_data = new uint8_t[m_dataSize + 4];

  m_data[0] = m_dataSize;
  m_data[1] = m_dataSize >> 8;
  m_data[2] = m_dataSize >> 16;
  m_data[3] = m_dataSize >> 24;

  message.SerializeToArray (&m_data[4], m_dataSize);
}



//vrne protobuf object
void
SerializationWrapper::GetData (protomessage::ProtoPacket *message)
{
  message->ParseFromArray (m_data, m_dataSize);
}

//Kontruktor, ki sprejme protobuf object
SerializationWrapper::SerializationWrapper (protomessage::ProtoPacket message)
{
  SetData (message);
}

uint32_t
SerializationWrapper::GetSerializedSize (void) const
{
  if (m_dataSize < 0)
    {
      std::cout << "message not set " << std::endl;
    }
  //4B where the size of the serialization is stored
  return m_dataSize + 4;
}

void
SerializationWrapper::Serialize (Buffer::Iterator start) const
{

  Buffer::Iterator i = start;
  i.Write (m_data, m_dataSize + 4);
}
uint32_t
SerializationWrapper::Deserialize (Buffer::Iterator start)
{

  Buffer::Iterator i = start;
  uint8_t *size_arr = new uint8_t[4];
  int size = 0;
  i.Read (size_arr, 4);

  memcpy (&size, &size_arr[0], 4);
  //std::cout << size << std::endl;
  m_data = new uint8_t[size];
  i.Read (m_data, size);
  m_dataSize = size;
  return size + 4;
}

//Ne implementirano
void
SerializationWrapper::Print (std::ostream &os) const
{
  //std::cout << "Print not implemented" << std::endl;

  os << "not printing the header";
}

} // namespace ns3