// DataReader.h

#ifndef _DATAREADER_H_    //include guards
#define _DATAREADER_H_

#include <vector>
#include <string>
#include <fstream>
using namespace std;

/// @brief a class to read and store the instance data
class RawInstance
{
    private: 
        
        //! the file path
        string filename;

        //! the number of rows in the file
        int rownum;

        //! the container to store all the information in the file
        vector<vector<int>> data_vec;

        //! count the rows in the file
        void countrows();

        //! read the file and store the information to data_vec
        void readdata();

    public:
        //! constructor
        RawInstance(string filepath);

        //! default constructor
        RawInstance() = default;

        //! destructor
        ~RawInstance(){};

        //! a simple getter
        int getRowNum();

        //! a simple getter
        int getNodesNum() {return rownum-1;};

        //! a simple getter
        vector<int> getRowData(int rowid);

        //! get the planning horizon of the system
        int getPlanHorizon() {return data_vec[1][5];};

        //! extract the number of vehicles from the instance data
        int extract_vehnum() {return data_vec[0][0];};

        //! extract the capacity of vehicles from the instance data
        int extract_vehcap() {return data_vec[0][1];};

        //! extract the speed of vehicles from the instance data
        int extract_vehspeed() {return data_vec[0][2];};

        //! extract demands from the instance data
        vector<int> extract_demands();

        //! extract coordinates from the instance data
        vector<vector<double>> extract_coordinates();

        //! extract service time windows from the instance data
        vector<vector<int>> extract_sertw();

        //! extract service time from the instance data
        vector<int> extract_sertime();
};

#endif