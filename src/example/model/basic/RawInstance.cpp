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
#include "example/model/basic/RawInstance.h"

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

//! extract demands from the instance data
vector<int> RawInstance::extract_demands()
{
    vector<int> initial_demands;
    for(int i = 1; i < rownum; i++)
    {
        initial_demands.push_back(data_vec[i][3]); //the first node is depot
    }
    return initial_demands;
}

//! extract coordinates from the instance data
vector<vector<double>> RawInstance::extract_coordinates()
{
    vector<vector<double>> initial_coord;
    for(int i = 1; i < rownum; i++)
    {
        initial_coord.push_back({double(data_vec[i][1]), double(data_vec[i][2])}); //the first node is depot
    }
    return initial_coord;
}

//! extract service time windows from the instance data
vector<vector<int>> RawInstance::extract_sertw()
{
    vector<vector<int>> initial_sertw;
    for(int i = 1; i < rownum; i++)
    {
        initial_sertw.push_back({data_vec[i][4], data_vec[i][5]}); //the first node is depot
    }
    return initial_sertw;
}

//! extract service time from the instance data
vector<int> RawInstance::extract_sertime()
{
    vector<int> initial_sertime;
    for(int i = 1; i < rownum; i++)
    {
        initial_sertime.push_back(data_vec[i][6]);
    }
    return initial_sertime;
}