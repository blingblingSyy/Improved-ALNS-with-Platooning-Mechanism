#include "example/model/basic/ADijkstraSol.h"

ADijkstraSol::ADijkstraSol(ADijkstraSol& other)
{
    this->KSP_Dist = other.KSP_Dist;
    this->KSP_Path = other.KSP_Path;
}

bool ADijkstraSol::operator==(const ADijkstraSol &other) const
{
    if (KSP_Dist != other.KSP_Dist) return false; 
    return KSP_Path == other.KSP_Path;
}

bool ADijkstraSol::operator <(const ADijkstraSol& other) const
{
    return KSP_Dist < other.KSP_Dist;
}