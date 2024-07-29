#include "couplingVRP/model/Vehicles.h"
#include "couplingVRP/model/config.h"
#include "couplingVRP/model/RawInstance.h"
#include "couplingVRP/model/VehiclesManager.h"

class RawInstance;
class VehiclesManager;

Vehicles::Vehicles(RawInstance& inputInstance, VehiclesManager& VehsMan)
{
    this->rawInstance = inputInstance;
    this->VehsMan = VehsMan;
    veh_num = rawInstance.extract_vehnum();
    buildVehsStruct();
}

void Vehicles::buildVehsStruct()
{
    veh_speed = rawInstance.extract_vehspeed();
    veh_types = VehsMan.set_mavtype(veh_num);
    veh_cap = VehsMan.set_mavcap(veh_types);
    veh_wlim_pernode = VehsMan.set_mavwlim(veh_types);
    veh_wlim_max = WAIT_MAX;
    veh_range = MAX_DIST;
    veh_plmax = PLEN_MAX;
}