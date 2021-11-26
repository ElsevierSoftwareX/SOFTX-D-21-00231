
<div align="center" >
    <h1 >
      <span style="color: #FF1493" >PPWSim</span>
    </h1>
    <h3>
        Privacy Preserving Wireless Sensor Network Simulator
    </h3>
    <h3>
        <img alt="Up-famnit Logo" src="https://www.famnit.upr.si/img/UP_FAMNIT.png"  height ="100px">
        <img alt="InnoRenew Logo" src="https://innorenew.eu/app/themes/innorenew/assets/img/logo_color.svg" style="vertical-align: middle" height ="100px">
  </h3>
  </div>
  <div align="left">
    <br>
    <a href="#">
        <img alt="GPL License" src="https://img.shields.io/badge/license-GPL-brightgreen"/>
    </a><br>
    <a href="#">
        <img alt="ns-3 simulator" src="https://img.shields.io/badge/simulator-ns--3-brightgreen"/>
    </a>
</div>



## Table of contents
* [General info](#general-info)
* [Acknowledgments](#acknowledgments)
* [Technologies](#technologies)
* [Version](#version)
* [Documentation](#documentation) 
* [How to Run](#how-to-run)
* [Installation and Setup](#installation-and-setup)

## General info
This project was developed to demonstrate the feasibility of the concept presented in the research paper:

Description of software functionalities and architecture can be found in the paper: 

A code ocean capsule can be found at:

A docker image can be found at:


## Acknowledgments

We would like to acknowledge the collegue dr. Jernej Vičič from the University of Primorska
Faculty of Mathematics, Natural Sciences and Information Technologies, and prof. dr. Mihael Mrissa from Innorenew CoE for the theoretical fundations, which are the basis for this implementation.

## Technologies
The project is created with:
* nsnam ns-3: 3.35
* libsodium: 1.0.18-1
* protobuf: 3.19.1
	
## Version

Version 1.0

## How to Run

First install the recuired software following the: [Installation and Setup](#installation-and-setup)
<br>
Then move to the directory:


```
 ns-3-allinone/ns-3.35/
```

To execute the simulator and source parameters from the configuration file, run:


```
 $ ./waf --run onion-routing-wsn
```


To execute the simulator and source parameters from CLI and the configuration file, run:


```
 $ ./waf --run "onion-routing-wsn --a_simNum=0 --a_name=test --a_routing=aodv  --a_topology=grid  --a_nodeNumber=13 "
```

To execute multiple simulations in parallel, check the Python script: [SimulationManager](link-to-sim-manager)



## Documentation

### Configuration file & CLI arguments

Simulation parameters can be setup via the configuration file [config](configParameters.xml).
Moreover, we implemented the setup of the following parameters (SimulationName,SimulationSeed,Routing,Topology,NodeNumber) through arguments provided at CLI.

### Description of parameters affecting the simulator output

(Descriptions will include parameters in *italic*)

Allow the selection of a name to identify the simulation. String value.

```xml
 <default name="ns3::WsnConstructor::SimulationName" value="default"/> 
```


Allow specifying the seed number of the random number generator to allow the reproducibility of results.

```xml
 <default name="ns3::WsnConstructor::SimulationSeed" value="0"/>
```

The relative path of the directory for storing the simulation output. 

```xml
<default name="ns3::OutputManager::OutputPath" value="./src/onion_routing_wsn/sim_results/"/> 
```


Allows to select the number of sensor nodes in the network. The network is including always one sink node.


```xml
<default name="ns3::WsnConstructor::NodeNumber" value="25"/> 
```



Allows the selection of the network topology:
* grid - deploy sensor node according to a grid structure; each sensor node is equidistant from the closest sensor nodes in cardinal directions. 
* disc - sensor nodes are randomly deployed on a disc-shaped plane. 

The sink node is deployed always in the central position of the selected topology.


```xml
 <default name="ns3::WsnConstructor::Topology" value="grid"/> 
```

The parameter *Radius* is used, to compute the **network_radius** of the network deployed using the random disc topology scheme.
First, A is obtaind, A being the sum of circular area's covered by *NodeNumber* sensor nodes at radius *Radius*. 
Then the radius of the network is obtained from **network_radius**=sqrt(A/PI).
<br>
Therefore, the average sensor node density of the network is maintained fixed at diverse *NodeNumber*.
The value of *Radius* is interpreted as meters.


```xml
 <default name="ns3::WsnConstructor::Radius" value="50"/>  
```


The length of a cell side in the grid topology. The length is representing the distance between neighbouring nodes on the cardinal directions of an arbitrary node. The value of *CellSide* is interpreted as meters.

```xml
  <default name="ns3::WsnConstructor::CellSide" value="60"/> 
```

Carrier frequency of the IEEE 802.11n, choodes between:
* 2_4GHz
* 5GHz

```xml
  <default name="ns3::WsnConstructor::IEEE80211n_carrier" value="2_4GHz"/> 
```


Modulation Coding Scheme of the IEEE 802.11n data mode, choodes from: HtMcs1 -- HtMcs7 

```xml
   <default name="ns3::ConstantRateWifiManager::DataMode" value="HtMcs1"/>  
```

Modulation Coding Scheme of the IEEE 802.11n control mode, choodes from: HtMcs1 -- HtMcs7 

```xml
   <default name="ns3::ConstantRateWifiManager::ControlMode" value="HtMcs1"/> 
```


Maximum Transmission Unit in bytes

```xml
   <default name="ns3::WifiNetDevice::Mtu" value="1280"/>  
```

Maximum Segment Size in bytes

```xml
   <default name="ns3::WsnConstructor::MSS" value="536"/>  
```

Routing algorithm for ad-hoc multi-hop wireless networks, choose between:
* aodv - Ad Hoc On-Demand Distance Vector
* dsr - Dynamic Source Routing (in the ns3-3.35 version the dsr routing is causing errors)
* olsr - Optimized Link State Routing Protocol 
* dsdv - Destination-Sequenced Distance Vector

```xml
 <default name="ns3::WsnConstructor::Routing" value="aodv"/>   
```

The *Delay* parameter sets the time interval in milliseconds for the sequential starting of nodes in the first phase of the simulation.

```xml
 <default name="ns3::Wsn_node::Delay" value="200"/>    
```


String of values delimited by the symbol **,** each value representing the onion message path length. (the number of hops the onion will travel to return back to the sink node issuer of the onion) 

```xml
  <default name="ns3::WsnConstructor::Paths" value="5,10,15"/> 
```

An integer specifying the number of times to generate the onion message for each value of the parameter *Paths*.

```xml
  <default name="ns3::WsnConstructor::RepeatePaths" value="1"/>  
```


Sets the verbosity of the simulator, choose between:
* no -  No output on stdout, except notifying simulation start and end, output data in csv file
* consoleLog - Output data and simulation description on console log, output data in csv file
* description - No output on console log, print simulation description and data on csv file
* both - print all simulation output on console log and csv file

```xml
   <default name="ns3::WsnConstructor::Verbosity" value="both"/>  
```

The watchdog timer set to abort onion messages, in seconds.

```xml
    <default name="ns3::Wsn_node::OnionTimeout" value="100"/>  
```


The onion head size is maintained uniform by adding padding to the onion head when a layer of the onion head is decrypted. true/false

```xml
     <default name="ns3::Sink::FixedOnionSize" value="true"/>   
```

Set properties of the onion body, choose between:
* nobody - The onion message won't have an onion body
* aggregate - The onion body will include only a value, and sensor nodes will aggregate their sensor value as the onion message travels the network
* fixed - The onion body will have a fixed size specified by the *BodySize* parameter
* both - The onion body will aggregate a value and will maintain a fixed size apecified by the *BodySize* parameter


```xml
     <default name="ns3::Sink::BodyOptions" value="both"/> 
```

Set the onion body size to emulate the transportation of data / computer code in the onion body


```xml
      <default name="ns3::Sink::BodySize" value="128"/>  
```




  
## Installation and Setup

### Docker

A docker file can be found at: [Dockerfile](install.sh)

### Ubuntu 20.04

To run this project, install it locally:

Required packages

```
$ sudo apt-get install autoconf automake curl wget libxml2 libxml2-dev g++ git libsodium-dev libtool make pkg-config python3 python3-dev python3-setuptools sqlite3
```

Install the protobuf library

```
$ mkdir simulator
$ cd simulator
$ wget https://github.com/protocolbuffers/protobuf/releases/download/v3.19.1/protobuf-cpp-3.19.1.tar.gz
$ tar -xvf protobuf-cpp-3.19.1.tar.gz
$ cd protobuf-3.19.1
$ ./configure
$ make 
$ make check 
$ make install
$ ldconfig
$ cd ..
```
INSTALL NS-3 3.35

```
$ git clone https://gitlab.com/nsnam/ns-3-allinone.git 
$ cd ns-3-allinone/ 
$ ./download.py -n ns-3.35 
$ cd ..
```



INSTALL the PPWSim

```
$ git clone http://github.com/nikih94/PPWSim 
$ cd PPWSim 
$ cp -r ./src/* ../ns-3-allinone/ns-3.35/src/
$ cd ..

```


Build the protobuf object and configure & build ns-3 


```
$ cd ns-3-allinone/ns-3.35/src/onion_routing_wsn/protobuf 
$ protoc -I=./ --cpp_out=./ ./proto-packet.proto 	
$ cd ../../..
$ ./waf configure && ./waf build 

```

NS-3 & PPWSim successfully installed





