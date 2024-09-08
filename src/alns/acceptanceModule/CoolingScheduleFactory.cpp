#include <iostream>
#include <stdlib.h>
#include "alns/acceptanceModule/CoolingScheduleFactory.h"
#include "alns/acceptanceModule/CoolingSchedule_Parameters.h"
#include "alns/acceptanceModule/ICoolingSchedule.h"
#include "alns/acceptanceModule/ExponentialCoolingSchedule.h"
#include "alns/acceptanceModule/MixExponentialCoolingSchedule.h"
#include "alns/improvedALNS/ISolution.h"


ICoolingSchedule* CoolingScheduleFactory::makeCoolingSchedule(ISolution& sol,
															 CoolingSchedule_Parameters& param)
{
	// Depending of the value of the field kind
	// of the cooling schedule parameters
	// a corresponding cooling schedule is build.
	if(param.kind == CoolingSchedule_Parameters::Exponential_it)
	{
		std::cout << "EXP" << std::endl;
		return dynamic_cast<ICoolingSchedule*>(new ExponentialCoolingSchedule(sol,param));
	}
	else if(param.kind == CoolingSchedule_Parameters::Exponential_mix)
	{
		std::cout << "EXP_MIX" << std::endl;
		return dynamic_cast<ICoolingSchedule*>(new MixExponentialCoolingSchedule(sol,param));
	}
	else
	{
		std::cerr << "This cooling schedule is not available yet" << std::endl;
		exit(EXIT_FAILURE);
	}
}


