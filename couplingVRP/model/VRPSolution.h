#ifndef VRPSOLUTION_H_
#define VRPSOLUTION_H_

#include <vector>
#include "src/alns/ISolution.h"
using namespace std;

class AROUTE;
class ISolution;
class APlatoon;
class Nodes;
class Vehicles;

class VRPSolution: public ISolution {
public:
	//! Constructor
	VRPSolution(Nodes& nodeset, Vehicles& vehset);

	//! Destructor.
	virtual ~VRPSolution() {};

    //! build a new route with randomly selected node from the set of uninserted customers
    void buildNewRoute();

    //! build an initial solution from scratch
    void buildInitialSol();

	//! A getter for the value of the objective function.
	virtual double getObjectiveValue();

	//! \return a penalized version of the objective value if the solution is infeasible. 
    //! But in this algorithm, all solutions must be feasible. Therefore, the penalized objective value is the same as the objective value.
	virtual double getPenalizedObjectiveValue();

	//! A getter for the feasibility of the current solution.
	//! \return true if the solution is feasible, false otherwise.
	virtual bool isFeasible();

	//! A comparator.
	//! \return true if this solution is "better" than the solution it is compared to.
	virtual bool operator<(ISolution&);

	//! Compute the "distance" between solution.
	//! This feature can be used as part of the ALNS to favor the
	//! diversification process. If you do not plan to use this feature
	//! just implement a method returning 0.
	virtual int distance(ISolution&);

	//! This method create a copy of the solution.
	virtual ISolution* getCopy();

	//! Compute a hash key of the solution.
	virtual long long getHash();

	//! a simple getter
	std::vector<int>& getNonInserted(){return nonInserted;};
    
    //! recompute the costs for a new solution
	void recomputeCost();
    
    //! insert a node into one route at a position
	void insertNode(int insert_pos, int insert_nodeid, int routeid);

    //! remove a node from one route at a position
	void removeNode(int remove_pos, int routeid);

    //! modify the used path at a position in one route
	void modifyPath(int modified_arcpos, int modified_pathid, int routeid);
    
    //! evaluate the costs of inserting a node 
	double evaluateInsertNode(int insert_pos, int insert_nodeid, int routeid);

    //! evaluate the costs of removing a node
	double evaluateRemoveNode(int remove_pos, int routeid);

    //! evaluate the costs of modifying the used path in a route
    double evaluateModifyPath(int modified_arcpos, int modified_pathid, int routeid);

    //! make platoons for the current solution
    void makePlatoons();

    //! set the final arrival and departure time for all routes in the current solution
    void setFinalArrDepTime();

    //! calculate the total distance of the solution
    double calTotalDist();

    //! calculate the total energy-related distance of the solution
    double calTotalEnergyDist();

    //! calculat ethe total trip duration of the solution
    int calTotalTripDuration();

    //! calculate the total unserved requests of the solution
    int calTotalUnservedRequests();

    //! calculate the objective value after platooning
    double calTotalObjectiveValue();

private:
    //! the route configuration of the solution
    vector<AROUTE*> sol_config;

    //! the number of routes involved
	int routesNum;

    //! the configuration of all platoons
    vector<APlatoon*> platoons_config;

    //! the number of platoons invovled
    int platoonsNum;

    //! the total distance before making platoons
    double totalDistCostsBeforePlatooning;

    //! the total distance after making platoons
    double totalDistCostsAfterPlatooning;

    //! the total trip duration of the sol
    int totalTripDurationAllRoutes;

    //! the total unserved requests
    int totalUnservedRequests;

    //! the total objective value after platooning
    double totalObjValueAfterPlatooning;

    //! the customers that has not been inserted
    vector<int> nonInserted;

    //! the cpu time before making platoons
    double getCpuBeforePlatooning;

    //! the cpu time after making platoons
    double getCpuAfterPlatooning;

};

#endif /* TSPSOLUTION_H_ */