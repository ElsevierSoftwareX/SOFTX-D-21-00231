#!/usr/bin/python

import sys
import os
from multiprocessing import Pool
	



def start_simulation(data):
	simNum = data[0]
	name = data[1]
	routing = data[2]
	topology = data[3]
	nodeNumber = data[4]
	os.system('./waf --run "onion-routing-wsn --a_simNum=%d --a_name=%s --a_routing=%s  --a_topology=%s  --a_nodeNumber=%d "' % (simNum,name,routing,topology,nodeNumber))


processes = 6 #number of processes to use at max

# create params
params = []


i=0
num_nodes = [10,20,40,60,80,100,150,200]



sim_name = "theName"

while i < len(num_nodes):
	params.append([i,sim_name,"aodv","grid",num_nodes[i]])
	params.append([i,sim_name,"aodv","disc",num_nodes[i]])
	i += 1

print("parameters Ok!")


# run
pool = Pool(processes=processes)
pool.map(start_simulation, params)