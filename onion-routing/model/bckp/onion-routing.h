/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ONION_ROUTING_H
#define ONION_ROUTING_H


#include <fstream>
#include <iostream>
#include <string.h>
#include <sodium.h>
#include <stdio.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"


namespace ns3 {

//struct containing the content of one onion layer
struct orLayer {
   uint8_t *   nextHopIP;
   uint8_t *   innerLayer;
   uint16_t          innerLayerLen;
};


class OnionRouting : public Object {
 public:
   static TypeId GetTypeId (void);
   OnionRouting ();
   OnionRouting (uint16_t keySize, uint16_t sealPadding, uint16_t addressSize);
   virtual ~OnionRouting ();
 
	int BuildOnion(uint8_t * cipher, uint8_t ** route, uint16_t routeLen, uint8_t ** keys, uint8_t * content , uint16_t contentLen); //setup creation of the onion
  int CreateOnion(uint8_t * cipher, uint8_t ** route, uint16_t index, uint16_t routeLen, uint8_t ** keys, uint8_t * content , uint16_t contentLen); //create the onion

 	orLayer * PeelOnion(uint8_t * onion, uint16_t onionLen, uint8_t * pk, uint8_t * sk); //peel one layer of the onion

 	virtual void EncryptLayer(uint8_t * ciphertext, uint8_t* message, int len, uint8_t * key) const = 0;
 	virtual	void DecryptLayer(uint8_t * innerLayer, uint8_t* onion, uint16_t onionLen, uint8_t * pk, uint8_t * sk) const = 0;

 	//Helper methods
  uint16_t OnionLength(uint16_t routeLen, uint16_t contentLen);
  void AddressToStream(uint8_t* ip);


	//other useful variables
	uint16_t 			   m_sealPadding; //in libsodium is 48B, 32 public key, and 16B of zeros	
	uint16_t			   m_keySize; // key size in bytzes
	uint16_t			   m_addressSize;


   //VErbosity of onion
   std::stringstream    m_onionStream;
 };
	

}

#endif /* ONION_ROUTING_H */

