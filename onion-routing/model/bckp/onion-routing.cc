/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "onion-routing.h"

namespace ns3 {

/* ... */


//Zagotovi, da se registrira TypeId
NS_OBJECT_ENSURE_REGISTERED (OnionRouting);

NS_LOG_COMPONENT_DEFINE("onionrouting");


TypeId
OnionRouting::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::OnionRouting")
	                    .SetParent<Object> ()
	                    .SetGroupName("OnionRouting");
	return tid;

}


OnionRouting::OnionRouting(){}




/*
*
*	Constructor method
*	Used to set: 
*		m_keySize		--	size in bytes of encryption keys	
*		m_sealPadding 	--	size increase of the ciphertext in bytes, intorduced by the encryption method (usually needed in publickey cryptography)
*		m_addressSize	-- 	size in bytes of the used address type (4-Ipv4, 16-Ipv6)
*
*/


OnionRouting::OnionRouting(uint16_t keySize ,uint16_t sealPadding, uint16_t addressSize) {
	this->m_keySize = keySize;
	this->m_sealPadding = sealPadding;
	this->m_addressSize = addressSize; //4B for IPv4
}


OnionRouting::~OnionRouting(){}




/*
*
*	Method to manage the construction of the onion
*	manages the output to the log component.
*
*/

int 
OnionRouting::BuildOnion(uint8_t * cipher, uint8_t ** route, uint16_t routeLen, uint8_t ** keys, uint8_t * content = nullptr , uint16_t contentLen = 0)
{
	NS_LOG_INFO("Start creation of the onion");

	m_onionStream.str("");

	int status = CreateOnion(cipher,route,routeLen, routeLen,keys,content,contentLen);

	AddressToStream(route[0]);

	NS_LOG_INFO(m_onionStream.str() << "\nOnion ready");

	return status;
}


/*
*
*	Method to construct the onion of encryption layers
*	uint8_t * cipher 	-- 	pointer to reserved memory to store the onion, to compute the needed space see the method OnionLength()
*	uint8_t ** route 	-- 	array of pointers pointing to ip addresses defining the route of the onion, 
*								the route starts from the first node in the onion path and ends at the last hop in the onion path.
*								each ip address is given as an array of bytes
*	uint16_t index			--	index used to move through the route for the construction of the onion
*	uint16_t routeLen		--	needed to allow the construction of the onion from a route given in forward order
*	uint8_t ** keys 	--	array of pointers pointing to encryption keys, each encryption key is given as an array of bytes
*	uint8_t * content --	pointer to the memory location when the content is stored. The content must be serialized to an array of bytes							
*	uint16_t contentLen     --	number of bytes of the content
*
*
*
*	!! if content is given -> (contentLen > 0) the innermost layer of the onion will be constructed as <0.0.0.0content>, 
*				symbols <> used to denote the encryption using the encryption key of the last hop given in the route. 
*				0.0.0.0 is the zero ipv4 address. IF ipv6 is used, the layer will include 16 zero bytes denoting the zero address in ipv6.
*	!! if content is NOT given (contentLen == 0) the innermost layer of the onion will be constructed as <last ip in the route>,
*				symbols <> used to denote the encryption using the encryption key of penultimate hop given in the route. 
*/


int 
OnionRouting::CreateOnion(uint8_t * cipher, uint8_t ** route, uint16_t index, uint16_t routeLen, uint8_t ** keys, uint8_t * content = nullptr , uint16_t contentLen = 0){


	//number of bytes to be encrypted in this layer
	int plainLayerLen = m_addressSize + OnionLength(index - 1,contentLen);


	uint8_t * nextHopIp = route[routeLen - index + 1]; //next hop address
	uint8_t * key = keys[routeLen - index]; //public key to encrypt the current layer
	
	m_onionStream << "("; //fancy output

	if(index <= 2 && contentLen != 0){ //stop recursion

		//Insert the zero address -- 0.0.0.0 (ipv4)
		for (int i = 0; i < m_addressSize; ++i)
		{
			cipher[m_addressSize + m_sealPadding + m_sealPadding + i] = 0;
		}

		//Insert content & encrypt
		memcpy(&cipher[m_addressSize + m_sealPadding + m_sealPadding + m_addressSize], &content[0], contentLen);
		EncryptLayer(&cipher[m_addressSize + m_sealPadding],&cipher[m_addressSize + m_sealPadding + m_sealPadding],m_addressSize + contentLen,keys[routeLen - index + 1]);

	}else if(index > 2){ //recursion

		CreateOnion(&cipher[ m_sealPadding + m_addressSize ],route, index-1, routeLen, keys,content,contentLen);

	} 

	//Insert next hop address
	memcpy(&cipher[m_sealPadding], &nextHopIp[0], m_addressSize);

	AddressToStream(nextHopIp); //fancy output	

	EncryptLayer(cipher,&cipher[m_sealPadding],plainLayerLen,key);	//encrypt
	
	m_onionStream << ") "; //fancy output
	return 0;

} //create the onion


/*
*
*	Method to decipher the onion and extract the content
*	uint8_t * onion 	-- 	pointer to the array of bytes containing the onion 
*	uint16_t onionLen 	-- 	length of the onion in bytes
*	uint8_t * pk		--	pointer to the memory location containing the public key
*	uint8_t * sk		--	pointer to the memory location containing the secret key
*
*	Returns a struct containing the next hop address and the inner onion layer.
*	the length of the inner onion layer is given in the struct
*/

orLayer * OnionRouting::PeelOnion(uint8_t * onion, uint16_t onionLen, uint8_t * pk, uint8_t * sk)
{
	uint8_t * innerLayer = new uint8_t[onionLen - (m_sealPadding)];

	DecryptLayer(innerLayer,onion,onionLen,pk,sk);

	orLayer * layer = new orLayer;
	layer->nextHopIP = innerLayer;
	layer->innerLayer = &innerLayer[m_addressSize];
	layer->innerLayerLen = onionLen - m_sealPadding - m_addressSize;

	return layer;
}




/*
*	Computes the number of bytes needed to store the onion 
*	Input: length of the route & length of the content
*/

uint16_t OnionRouting::OnionLength(uint16_t routeLen,uint16_t contentLen)
{
	routeLen = routeLen - 1;
	if(contentLen == 0){
		return routeLen * (m_sealPadding + m_addressSize);
	}else{ 
		return routeLen * (m_sealPadding + m_addressSize) + m_sealPadding + m_addressSize + contentLen;
	}
}






/*
*	Print address on stream
*	4 fancy output
*/

void OnionRouting::AddressToStream(uint8_t* ip)
{
	m_onionStream << (int) ip[0];
	for (int i = 1; i < m_addressSize; ++i)
	{
		m_onionStream << "."  << (int) ip[i];
	}
}



}

