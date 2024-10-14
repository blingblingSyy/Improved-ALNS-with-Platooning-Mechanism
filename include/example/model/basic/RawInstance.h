// RawInstance.h

#ifndef _RAWINSTANCE_H_    //include guards
#define _RAWINSTANCE_H_

#include <vector>
#include <string>
#include <fstream>
using namespace std;

/// @brief a class to read and store the instance data
class RawInstance
{
    private: 
        //! the number of rows in the file
        int rownum;

        //! the container to store all the information in the file
        vector<vector<double>> data_vec;

        // //! the number of customers in the file
        int nodenum_wodepot;

        //! the number of passenger-type nodes
        int pas_cusnum;

        //! the number of freight-type nodes
        int fre_cusnum;

        //! the number of intersections
        int sect_num;

        //! the types of nodes of all nodes
        vector<int> nodetypes;

        //! the coordinates of all nodes
        vector<vector<double>> coordinates;

        //! the service time windows of all nodes
        vector<vector<int>> serviceTW;

        //! the demands of all nodes
        vector<int> demands;

        //! the service time of all nodes
        vector<int> serviceTime;

        //! the passenger-type vehicle number
        int pas_vehnum;

        //! the freight-type vehicle number
        int fre_vehnum;

        //! the total vehicle number
        int vehnum;

        //! the vehicle capacity of passenger-type
        int pas_vehcap;

        //! the vehicle capacity of freight-type
        int fre_vehcap;

        //! the vehicle speed
        double veh_speed;

        //! count the rows in the file
        void countrows(string filepath);

        //! read the file and store the information to data_vec
        void readdata(string filepath);

        //! extract vehicle information from the file data
        void extractVehsInfo();

        //! extract node information from the file data
        void extractNodesInfo();

        //! design node types for all nodes
        void designNodeTypes(int nodenum_wodepot);

        //! design coordinates for all nodes - pattern = cluster if cluster == true, or else pattern = distributed
        void designCoord(bool cluster); //! cusnum from nodetypes

        //! design service time windows for all nodes - pattern = peak if peak == true, or else pattern = even
        void designSerTW(bool peak); //! cusnum from nodetypes

        //! design demands for all nodes -> depends on nodetypes
        void designDemands();

        //! design service time for all nodes -> depends on nodetypes
        void designSerTime();

        //! design vehicle numbers of each type
        void designVehInfo();

        //! create the dataset with four different scenarios and store in the data_vec
        void designdata();

    public:
        //! constructor: read dataset from the given path
        RawInstance(string filepath);

        //! default constructor: create dataset
        RawInstance(int nodenum_wodepot, bool cluster, bool peak);

        //! destructor
        ~RawInstance();

        //! write the created data to files
        void writedata(string dirpath, string filename);

        //! a simple getter
        int getRowNum() {return rownum;};

        //! a simple getter
        int getAllNodesNum() {return rownum-1;};

        //! a simple getter
        int getCusNum() {return pas_cusnum + fre_cusnum;};

        //! a simple getter
        vector<double> getRowData(int rowid);

        //! get the planning horizon of the system
        vector<int> getPlanHorizon();

        //! get the number of vehicles from the instance data - 0 for passenger-type, 1 for freight-type and 2 for all
        int getVehNum(int type = 2);

        //! set the number of vehicles 
        void setVehNum(int type, int vehnum);

        //! get the vehicle ids given the specific type - 0 for passenger-type and 1 for freight-type
        vector<int> getVehIDs(int type);

        //! get the capacity of vehicles given the specific type - 0 for passenger-type and 1 for freight-type
        int getVehCap(int type);

        //! set the capacity of vehicles 
        void setVehCap(int type, int vehcap);

        //! get the speed of vehicles from the instance data
        int getVehSpeed() {return veh_speed;};

        //! get the types of nodes from the input instance data
        vector<int> getNodeTypes() {return nodetypes;};

        //! get the node ids given the node type - 0 for passenger-type and 1 for freight-type
        vector<int> getNodeIDs(int type);

        //! get demands from the instance data
        vector<int> getDemands() {return demands;};

        //! get coordinates from the instance data
        vector<vector<double>> getCoordinates() {return coordinates;};

        //! get service time windows from the instance data
        vector<vector<int>> getSerTW() {return serviceTW;};

        //! get service time from the instance data
        vector<int> getSerTime() {return serviceTime;};
};

#endif