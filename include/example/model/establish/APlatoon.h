#ifndef APLATOON_H_
#define APLATOON_H_

#include <vector>
#include <tuple>
using namespace std;

class Nodes;

/// @brief a class to store a platoon's configuration on one arc
class APlatoon
{
    private:
        //! the input nodeset
        Nodes* nodeset;
        
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

    public:
        //! constructor
        APlatoon(Nodes& nodeset); 

        //! copy constructor
        APlatoon(APlatoon& p);

        //! default constructor
        APlatoon() = default;

        //! destructor
        ~APlatoon();
        
        //! a == comparator
        virtual bool operator ==(const APlatoon& other) const;

        //! a < comparator
        virtual bool operator <(const APlatoon& other) const;

        //! a > comparator
        virtual bool operator >(const APlatoon& other) const;

        //! a simple getter
        vector<int> getArc() {return arc;};

        //! a simple getter: <route_id, arc_pos_on_the_route>
        vector<pair<int, int>>& getPlatoonConfig() {return config;};

        //! a simple getter
        vector<int> getOverlapDTW() {return overlap_deptw;};

        //! a simple getter to get the common vehicles involved in the platoon
        vector<int> getCommonRoutes();

        //! a simple getter
        double getEnergySaving() {return energy_saving;};

        //! calculate the length of arc
        double calArcLen();

        //! calculate the length of the platoon
        int calPLen();

        //! calculate the energy saving by this platoon on this arc
        double calEnergySaving();

        //! a simple setter of the arc
        void setArc(vector<int> inputArc);

        //! a simple setter of the platoon configuration
        void setConfig(vector<pair<int, int>> inputConfig);

        //! a simple setter to expand the platoon
        void addOneConfig(pair<int, int> inputVehiclePos) {config.push_back(inputVehiclePos);};

        //! a simple setter to reduce the platoon
        void removeOneVeh(int vehid);

        //! a simple setter of the overlapping departure time windows
        void setOverlapDTW(vector<int> input_overlaptw) {overlap_deptw = input_overlaptw;};

};

#endif