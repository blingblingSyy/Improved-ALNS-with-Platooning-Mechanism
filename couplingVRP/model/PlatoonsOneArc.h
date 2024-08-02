#ifndef PLATOONSONEARC_H_
#define PLATOONSONEARC_H_

#include <vector>
#include <tuple>
using namespace std;

/// @brief a class to store a platoon's configuration on one arc
class PlatoonsOneArc
{
    private:
        //! the arc on which the platoon is formed
        vector<int> arc;

        //! the length of the arc
        double arclen; 

        //! the length of the platoon
        int plen;

        //! the configuration of the platoon: <route_id, arc_pos_on_the_route>
        vector<pair<int, int>> config;

        //! the overlapping departure time window of the arc (start node of the arc)
        vector<int> overlap_deptw;

        //! the amount of energy saving by forming platoons on this arc
        double energy_saving;

        //! calculate the length of arc
        double calArcLen();

        //! calculate the overlapping departure time windows of the platoon on this arc
        vector<int> calOverlapDTW();

        //! calculate the energy saving by this platoon on this arc
        double calEnergySaving();

    public:
        PlatoonsOneArc() {}; 
        ~PlatoonsOneArc() {};
        
        //! a == comparator
        virtual bool operator ==(const APlatoon& other) const;

        //! a < comparator
        virtual bool operator <(const APlatoon& other) const;

        //! a > comparator
        virtual bool operator >(const APlatoon& other) const;

        //! a simple getter
        vector<int> getArc() {return arc;};
        int getArcLen() {return arclen;};
        int getPlen() {return plen;};
        vector<pair<int, int>> getPlatoonConfig() {return config;};
        vector<int> getOverlapDTW() {return overlap_deptw;};
        double getEnergySaving() {return energy_saving;};

        //! a simple setter
        void setArc(vector<int> inputArc) {arc = inputArc;};
        void setConfig(vector<pair<int, int>> inputConfig) {config = inputConfig;};
        void addVehicle(pair<int, int> inputVehicle) {config.push_back(inputVehicle);};

};

#endif