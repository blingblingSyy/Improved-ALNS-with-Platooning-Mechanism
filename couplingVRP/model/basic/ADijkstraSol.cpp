#include "couplingVRP/model/basic/ADijkstraSol.h"

bool ADijkstraSol::operator==(const ADijkstraSol &other) const
{
    if (KSP_Dist != other.KSP_Dist) return false; 
    return KSP_Path == other.KSP_Path;
}

bool ADijkstraSol::operator <(const ADijkstraSol& other) const
{
    return KSP_Dist < other.KSP_Dist;
}