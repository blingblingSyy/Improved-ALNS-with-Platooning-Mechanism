#include "couplingVRP/model/ADijkstraSol.h"

ADijkstraSol::ADijkstraSol()
{
    KSP_Dist = 0;
    KSP_Path = {};
}

bool ADijkstraSol::operator==(const ADijkstraSol &other) const
{
    if (KSP_Dist != other.KSP_Dist) return false; 
    return KSP_Path == other.KSP_Path;
}