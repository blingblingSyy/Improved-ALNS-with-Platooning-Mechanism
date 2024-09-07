#ifndef IUPDATABLE_H_
#define IUPDATABLE_H_

class ISolution;
class ALNS_Iteration_Status;

/// @brief An interface define object that should be updated using solution information or iteration status information.
class IUpdatable
{
public:
	IUpdatable(){};
	virtual ~IUpdatable() noexcept(false) {};
	virtual void update(ISolution& sol, ALNS_Iteration_Status& status)=0;
};

#endif