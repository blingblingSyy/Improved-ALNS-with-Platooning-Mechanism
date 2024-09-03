# A MODULAR MULTI-PURPOSE VEHICLE ROUTING PROBLEM WITH COUPLING AND DECOUPLING: USING MODULAR AUTONOMOUS VEHICLES FOR COLLABORATIVE PASSENGER AND FREIGHT TRANSPORT

Copyright (C) 2024 Sun Yingying (the Contributor)

## Problem Statement

### System Description
* Consider a transportation system with only one central depot, where all MAVs start and end their trips. 
* The system involves customers of both passengers and freight, whose demands and service time windows are known in advance. 
* A set of identical MAVs, dedicated to either passenger-type or freight-type, are going to provide services for customers by either picking up or delivering their demands as requirements. 
* An MAV may bypass a customer several times, but can only serve a customer once, and the service must take place within the service time windows of the customer. 

### Platooning Mechanisms
* It is assumed in this system that in-vehicle transfers between modules are not allowed; however, MAVs are free to couple as a platoon regardless of their different types. 

* If several MAVs depart simultaneously for a common destination along the same route, they can automatically form a platoon. Moreover, a platoon may subsequently be divided into several sub-platoons when the modules involved are directed toward different destinations.

* To fully exploit the benefits of platooning, two mechanisms are proposed to increase the opportunities of coupling MAVs. 
    * On one hand, an MAV can wait at a location for upcoming MAVs that will traverse the same path segment. 
    * On the other hand, an MAV may take detours to meet other MAVs along alternative routes, rather than strictly adhering to the shortest path as typically assumed in a traditional VRP setting. 

* The platooning conditions among vehicles are dependent on the overlapping departure time windows of several vehicles travelling along the same arc.

## Algorithm: Improved ALNS
adapted based on Renaud Masson (2012): https://github.com/biblik/alns-framework/tree/master/trunk
* The improved ALNS consists of Node Operators and Path Operators, exploring opportunities for MAV coupling. All operators are chosen by the adaptive weight adjustment mechanism, with accumulative scores updated in each iteration.
* Two strategies are proposed for the improved ALNS, which are the Node First strategy and the Path First strategy. 
    * In Node First strategy, nodes-related operators are chosen and conducted first, afterwards, the algorithm can opt to choose or not choose paths-related operators. 
    * In Path First strategy, paths-related operators are chosen and conducted first, afterwards, the algorithm can opt to choose or not choose nodes-related operators.
    * The two strategies are chosen by the adaptive weight adjustment mechanism, with accumulative scores updated in each iteration.