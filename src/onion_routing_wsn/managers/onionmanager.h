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


#ifndef ONIONMANAGER_H
#define ONIONMANAGER_H

#include <fstream>
#include <iostream>
#include <string.h>
#include <sodium.h>
#include <stdio.h>

#include "ns3/onion-routing.h"
#include "ns3/internet-module.h"

namespace ns3 {

/**
 * \ingroup managers
 * \class OnionManager
 * \brief Class that manages encryption keys and the encryption and decryption of layers of onion messagess
 *        The class is implementing the OnionRouting abstract class with the encryption and decryption methods
 * 
 * 
 */

class OnionManager : public OnionRouting
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
  OnionManager ();

  /**
  *
  * \brief Default destructor
  *
  */
  ~OnionManager ();

  /**
*
* \brief Implementing encryption using the libsodium library
* 
*   \param [in,out] ciphertext memory on which the ciphertext will be stored
*   \param [in] mesage memory locations containing the data to be encrypted
*   \param [in] len length in bytes of the \p plaintext 
*   \param [in] key pointer to the encryption key 
*
*
*/

  virtual void EncryptLayer (unsigned char *ciphertext, unsigned char *message, int len,
                             unsigned char *key) const;

  /**
*
* \brief Implementing decryption using the libsodium library
* 
*   \param [in,out] innerLayer memory on which the inner onion layer will be stored
*   \param [in] onion memory locations containing the data to be decrypted
*   \param [in] len length in bytes of the \p onion 
*   \param [in] pk pointer to the public encryption key 
*   \param [in] sk pointer to the secret encryption key 
*
*/

  virtual void DecryptLayer (unsigned char *innerLayer, unsigned char *onion, uint16_t onionLen,
                             unsigned char *pk, unsigned char *sk) const;

  /**
  *
  * \brief Generate a new public/private keypair using the libsodium library
  *
  */
  void GenerateNewKeyPair (void);
  /**
  *
  * \brief accessor
  * 
  * \return Return the pointer to the public key
  *
  */
  unsigned char *GetPK (void);
  /**
  *
  * \brief accessor
  * 
  * \return Return the pointer to the secret key
  *
  */
  unsigned char *GetSK (void);

  /**
  *
  * \brief accessor
  * 
  * \return Return the public key as a string
  *
  */
  std::string GetPKtoString ();
  /**
  *
  * \brief accessor
  * 
  * \return Return the secret key as a string
  *
  */
  std::string GetSKtoString ();

  /**
  *
  * \brief setter
  * 
  * \param [in] pk pointer to the public encryption key
  *
  */
  void SetPK (unsigned char *pk);
  /**
  *
  * \brief setter
  * 
  * \param [in] pk pointer to the secret encryption key
  *
  */
  void SetSK (unsigned char *sk);

  /**
  *
  * \brief Convert a string to an array of unsigned chars
  * 
  * \param [in] in a std::string
  * 
  * \return the pointer to the array of unsigned chars
  *
  */
  unsigned char *StringToUchar (std::string in);
  /**
  *
  * \brief Convert an array of unsigned chars to a std::string
  * 
  * \param [in] seq an array of unsigned chars
  * \param [in] len the length of the \p seq the array of unsigned chars
  * 
  * \return a std::string
  *
  */
  std::string UcharToString (unsigned char *seq, int len);

  /**
  *
  * \brief Convert an Ipv4 address given as an unsigned integer value to buffer array of 4Bytes
  * 
  * \param [in] in an Ipv4 address given as an unsigned integer value
  * 
  * \return the pointer to the array of 4Bytes
  *
  */
  unsigned char *IpToBuff (uint32_t in);

private:
  unsigned char m_publickey[crypto_box_PUBLICKEYBYTES]; //!< the public encryption key
  unsigned char m_secretkey[crypto_box_SECRETKEYBYTES]; //!< the secret encryption key
};

} // namespace ns3

#endif /* ONIONMANAGER_H */
