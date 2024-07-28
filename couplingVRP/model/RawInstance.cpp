// #define NDEBUG
#include <iostream>  
#include <vector>  
#include <ctime>  
#include <cmath>
#include <cstdlib>  
#include <string>  
#include <algorithm>  
#include <fstream>
#include <cassert>
#include "couplingVRP/model/RawInstance.h"

using namespace std;  

RawInstance::RawInstance(string filepath)
{
    filename = filepath;
    rownum = 0;
    countrows();
    readdata();
}

void RawInstance::countrows()
{
    ifstream inputstr(filename);
	char chr;
	while(inputstr.get(chr))
	{
		if(chr == '\n')
			rownum++;
	}
	inputstr.close();
}

void RawInstance::readdata()
{
	vector<int> raw_data;
	vector<int>::iterator it;
	ifstream file(filename);    //string filename = "lc101.txt";
	
	int num;
	while(file >> num)
	{
		raw_data.push_back(num);
	}
	file.close();

    it = raw_data.begin();
    data_vec.resize(rownum);
    while (it != raw_data.end())
    {
       for(int i = 0; i < rownum; i++)
        {
            if(i != 0)
            {
                data_vec[i].resize(9);
                for(int j = 0; j < 9; j++)
                {
                    data_vec[i][j] = *it;
                    it++;
                }
            }
            else  //i == 0
            {
                data_vec[i].resize(3);
                for(int j = 0; j < 3; j++)
                {
                    data_vec[i][j] = *it;
                    it++;
                }
            }
        }
    }
}

int RawInstance::getRowNum()
{
    return this->rownum;
}

vector<int> RawInstance::getRowData(int rowid)
{
    // assert(rowid < rownum);
    return this->data_vec[rowid];
}