#ifndef ISOLUTION_H_
#define ISOLUTION_H_

/// @brief An interface to define Solutions.
class ISolution
{
public:
	virtual ~ISolution(){};
	
	//! A getter for the value of the objective function.
	//! \return the value of the objective function of this solution.
	virtual double getObjectiveValue()=0;

	//! \return a penalized version of the objective value if the solution is infeasible.
	virtual double getPenalizedObjectiveValue(bool modified = false) = 0;

	//! A getter for the feasibility of the current solution.
	//! \return true if the solution is feasible, false otherwise.
	virtual bool isFeasible()=0;

	//! A comparator.
	//! \return true if this solution is "better" than the solution it is compared to.
	virtual bool operator<(ISolution&)=0;

	//! Compute the "distance" between solution.
	//! This feature can be used as part of the ALNS to favor the
	//! diversification process. If you do not plan to use this feature
	//! just implement a method returning 0.
	virtual int distance(ISolution&)=0;

	//! This method create a copy of the solution.
	virtual ISolution* getCopy()=0;

	//! Compute a hash key of the solution.
	virtual long long getHash()=0;
	
	//! Make platoons for the solution
	virtual void makePlatoons() = 0;

	//! update the solution
	virtual void updateSol(bool make_platoon = false) = 0;

	//! recompute costs of the solution
	virtual void recomputeCost() = 0;  //bool make_platoon = false
};


#endif /* ISOLUTION_H_ */
