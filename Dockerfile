#download the base image
FROM ubuntu:20.04

# LABEL about the custom image
LABEL maintainer="niki.hrovatin@famnit.upr.si"
LABEL version="0.1"
LABEL description="Custom Docker image for Privacy Preserving Wireless Sensor Network Simulator"

# Disable Prompt During Packages Installation
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        apt-transport-https=2.0.6 \
        autoconf=2.69-11.1 \
        automake=1:1.16.1-4ubuntu6 \
        ca-certificates=20210119~20.04.2 \
        curl \
        wget \
        libxml2 \ 
        libxml2-dev \
        g++=4:9.3.0-1ubuntu2 \
        git=1:2.25.1-1ubuntu3.2 \
        libsodium-dev=1.0.18-1 \
        libtool=2.4.6-14 \
        make=4.2.1-1.2 \
        pkg-config=0.29.1-0ubuntu4 \
        python3=3.8.2-0ubuntu2 \
        python3-dev=3.8.2-0ubuntu2 \
        python3-setuptools=45.2.0-1 \
        sqlite3=3.31.1-4ubuntu0.2 \
        unzip=6.0-25ubuntu1 \
    && rm -rf /var/lib/apt/lists/*



RUN update-ca-certificates


#install protobuf
RUN mkdir /home/simulator && \ 
    cd /home/simulator && \
    wget https://github.com/protocolbuffers/protobuf/releases/download/v3.19.1/protobuf-cpp-3.19.1.tar.gz && \
    tar -xvf protobuf-cpp-3.19.1.tar.gz && \
    cd protobuf-3.19.1 &&\
    ./configure && \
    make && \
    make check && \
    make install && \
    ldconfig


#Install ns3
RUN cd /home/simulator && \
    git clone https://gitlab.com/nsnam/ns-3-allinone.git && \
    cd ns-3-allinone/ && \
    ./download.py -n ns-3.35 




#download PPWSim code and move into ns-3 folder
RUN cd /home/simulator && \
    git clone http://github.com/nikih94/PPWSim && \
    cd PPWSim && \
    cp -r ./src/* ../ns-3-allinone/ns-3.35/src/ 


#build the protobuf object and configure & build ns-3 AND run hello sim
RUN cd /home/simulator/ns-3-allinone/ns-3.35/src/onion_routing_wsn/protobuf  && \
    protoc -I=./ --cpp_out=./ ./proto-packet.proto && \
    cd /home/simulator/ns-3-allinone/ns-3.35 && \
    ./waf configure && ./waf build 

#run the simulation
#RUN cd /home/simulator/ns-3-allinone/ns-3.35 && \
#    ./waf --run onion-routing-wsn



