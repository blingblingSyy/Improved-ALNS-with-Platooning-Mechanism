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
        RawInstance(string filepath);
        ~RawInstance(){};

        //! a simple getter
        int getRowNum();

        //! a simple getter
        vector<int> getRowData(int rowid);
};

#endif