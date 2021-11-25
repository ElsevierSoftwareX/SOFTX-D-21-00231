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


#include "onionmanager.h"

namespace ns3 {

/* ... */

//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (OnionManager);

TypeId
OnionManager::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::OnionManager").SetParent<OnionRouting> ().SetGroupName ("OnionRouting");
  return tid;
}

OnionManager::OnionManager () : OnionRouting (crypto_box_SEALBYTES, Ipv4L3Protocol::PROT_NUMBER)
{
}

OnionManager::~OnionManager ()
{
}

/** implement onion routing class
 * 
 * 
 * */

void
OnionManager::EncryptLayer (unsigned char *ciphertext, unsigned char *message, int len,
                            unsigned char *key) const
{
  if (crypto_box_seal (ciphertext, message, len, key) != 0)
    {
      std::cout << "Error during encryption" << std::endl;
    }
}

void
OnionManager::DecryptLayer (unsigned char *innerLayer, unsigned char *onion, uint16_t onionLen,
                            unsigned char *pk, unsigned char *sk) const
{
  if (crypto_box_seal_open (innerLayer, onion, onionLen, pk, sk) != 0)
    {
      std::cout << "messge corrupted or not for this node" << std::endl;
    }
}

/** handling encryption keys
 * 
 * 
 * */

//Generate new key pair
void
OnionManager::GenerateNewKeyPair ()
{
  crypto_box_keypair (m_publickey, m_secretkey);
}

//return public key
unsigned char *
OnionManager::GetPK ()
{
  // klici tako unsigned char* pk = kh.GetPK();
  return m_publickey;
}

//Get secret key
unsigned char *
OnionManager::GetSK ()
{
  return m_secretkey;
}

//Return public key in string form
std::string
OnionManager::GetPKtoString ()
{
  std::string strForm (&m_publickey[0], &m_publickey[0] + crypto_box_PUBLICKEYBYTES);
  return strForm;
}

//Return private key in string form
std::string
OnionManager::GetSKtoString ()
{
  std::string strForm (&m_secretkey[0], &m_secretkey[0] + crypto_box_SECRETKEYBYTES);
  return strForm;
}

//set public key
void
OnionManager::SetPK (unsigned char *pk)
{
  memcpy (&m_publickey, pk, crypto_box_PUBLICKEYBYTES);
}

//set secret key
void
OnionManager::SetSK (unsigned char *sk)
{
  memcpy (&m_secretkey, sk, crypto_box_SECRETKEYBYTES);
}

/** Other methods
 * 
 * 
 * */

//Convert string to unsigned char *
unsigned char *
OnionManager::StringToUchar (std::string in)
{
  unsigned char *out = new unsigned char[in.length ()];
  memcpy (&out[0], &in[0], in.length ());
  //std::cout << "Eruuuuuuuuuuuuuuuuuuuuuuuuuu " << std::endl;
  return out;
}

//convert unsigned char to string
std::string
OnionManager::UcharToString (unsigned char *seq, int len)
{
  std::string strForm (&seq[0], &seq[0] + len);
  return strForm;
}

//Convert ip to unsigned char *
unsigned char *
OnionManager::IpToBuff (uint32_t in)
{
  unsigned char *out = new unsigned char[4];
  Ipv4Address (in).Serialize (&out[0]);
  //std::cout << "Eruuuuuuuuuuuuuuuuuuuuuuuuuu " << std::endl;
  return out;
}

} // namespace ns3
