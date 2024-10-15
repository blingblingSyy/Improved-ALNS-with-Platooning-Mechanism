// #define NDEBUG
#include <iostream>  
#include <vector>  
#include <cmath>
#include <string>  
#include <algorithm>  
#include <random>
#include <fstream>
#include <cassert>
#include <iomanip>
#include "example/model/basic/RawInstance.h"
#include "example/model/basic/config.h"
#include "utility.h"

using namespace std;  

RawInstance::RawInstance(string filepath)
{
    rownum = 0;
    countrows(filepath);
    readdata(filepath);
    extractVehsInfo();
    extractNodesInfo();
}

RawInstance::RawInstance(int nodenum_wodepot, bool cluster, bool peak)
{
    rownum = nodenum_wodepot + 2; //! nodenum exclude the depot
    designNodeTypes(nodenum_wodepot); //! nodetypes
    designCoord(cluster);   //! coordinates
    designSerTW(peak); //! serviceTW
    designDemands(); //! demands
    designSerTime();  //! serviceTime
    designVehInfo();  //! vehnum
}

RawInstance::~RawInstance()
{
    data_vec.clear();
    coordinates.clear();
    demands.clear();
    serviceTW.clear();
    serviceTime.clear();
    nodetypes.clear();
}

void RawInstance::countrows(string filepath)
{
    ifstream inputstr(filepath);
	char chr;
	while(inputstr.get(chr))
	{
		if(chr == '\n')
			rownum++;
	}
	inputstr.close();
}

void RawInstance::readdata(string filepath)
{
	vector<double> raw_data;
	vector<double>::iterator it;
	ifstream file(filepath);    //string filename = "xxx.txt";
	
	double num;
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
                data_vec[i].resize(8);
                for(int j = 0; j < 8; j++)
                {
                    data_vec[i][j] = *it;
                    it++;
                }
            }
            else  //i == 0
            {
                data_vec[i].resize(5);
                for(int j = 0; j < 5; j++)
                {
                    data_vec[i][j] = *it;
                    it++;
                }
            }
        }
    }
}

void RawInstance::extractVehsInfo()
{
    pas_vehnum = data_vec[0][0];
    fre_vehnum = data_vec[0][1];
    vehnum = pas_vehnum + fre_vehnum;
    pas_vehcap = data_vec[0][2];
    fre_vehcap = data_vec[0][3];
    veh_speed = data_vec[0][4];
}

void RawInstance::extractNodesInfo()
{
    //! coordinates; demands; service time windows; service time; nodetypes
    for(int i = 1; i < rownum; i++)
    {
        coordinates.push_back({data_vec[i][1], data_vec[i][2]}); //the first node is depot
        demands.push_back(static_cast<int>(data_vec[i][3]));
        serviceTW.push_back({static_cast<int>(data_vec[i][4]), static_cast<int>(data_vec[i][5])});
        serviceTime.push_back(static_cast<int>(data_vec[i][6]));
        nodetypes.push_back(static_cast<int>(data_vec[i][7]));
    }
    pas_cusnum = 0; fre_cusnum = 0, sect_num = 0;
    for(int i = 1; i < nodetypes.size(); i++)
    {
        if(nodetypes[i] == 0)
        {
            pas_cusnum += 1;
        }
        else if(nodetypes[i] == 1)
        {
            fre_cusnum += 1;
        }
        else if(nodetypes[i] == 2)
        {
            sect_num += 1;
        }
    }
    nodenum_wodepot = rownum - 2; //! pas_cusnum + fre_cusnum + sect_num;
}

void RawInstance::designNodeTypes(int nodenum_wodepot)
{
    nodetypes.clear();
    nodetypes.push_back(2); //! the first is depot
    pas_cusnum = int(nodenum_wodepot * PAS_CUS_PROP);
    fre_cusnum = int(nodenum_wodepot * FRE_CUS_PROP);
    sect_num = nodenum_wodepot - pas_cusnum - fre_cusnum;
    this->nodenum_wodepot = nodenum_wodepot;
    for(int i = 0; i < pas_cusnum; i++)
    {
        nodetypes.push_back(0);
    }
    for(int i = 0; i < fre_cusnum; i++)
    {
        nodetypes.push_back(1);
    }
    for(int i = 0; i < sect_num; i++)
    {
        nodetypes.push_back(2);
    }
    // random_device rd;
    // mt19937 gen(rd());
    // shuffle(nodetypes.begin()+1, nodetypes.end(), gen);
}

void RawInstance::designCoord(bool cluster)
{
    coordinates.push_back({AREA_SIZE/2, AREA_SIZE/2});
    vector<double> x_coord;
    vector<double> y_coord;
    random_device rd;
    mt19937 gen(rd());
    if(cluster)
    {
        RandomNumber r;
        int cluster_num = r.get_rint(5, 10);
        uniform_real_distribution<> uniform_dis(0, AREA_SIZE);
        for(int i = 0; i < cluster_num; i++)
        {
            coordinates.push_back({uniform_dis(gen), uniform_dis(gen)});
        }
        int unset_cusnum = nodenum_wodepot - cluster_num;
        int per_cusnum = int(unset_cusnum / cluster_num);
        // int last_cusnum = unset_cusnum - per_cusnum * (cluster_num - 1);
        for(int k = 1; k < cluster_num+1; k++)
        {
            double cluster_k_x = coordinates[k][0];
            double cluster_k_y = coordinates[k][1];
            normal_distribution<> normal_dis_x(cluster_k_x, 1.25);
            normal_distribution<> normal_dis_y(cluster_k_y, 1.25);
            int cusnum_k = (k < cluster_num) ? per_cusnum : (unset_cusnum - per_cusnum * (cluster_num - 1));
            for(int z = 0; z < cusnum_k; z++)
            {
                double x, y;
                do {
                    x = normal_dis_x(gen);
                } while(x < 0 || x > AREA_SIZE);
                do {
                    y = normal_dis_y(gen);
                } while(y < 0 || y > AREA_SIZE);
                coordinates.push_back({x, y});
            }
        }
        shuffle(coordinates.begin()+1, coordinates.end(), gen);
    }
    else //! uniform
    {
        // random_device rd;
        // mt19937 gen(rd());
        uniform_real_distribution<> uniform_dis(0, AREA_SIZE);
        for(int i = 0; i < nodenum_wodepot; i++)
        {
            coordinates.push_back({uniform_dis(gen), uniform_dis(gen)});
        }
    }
}

void RawInstance::designSerTW(bool peak)
{
    serviceTW.push_back(PLAN_HORIZON); //! service time window for the depot
    vector<int> serve_start;
    random_device rd;
    mt19937 gen(rd());
    if(peak)
    {
        vector<int> sampleNumPerMode_Pas, sampleNumPerMode_Fre;
        int per_cusnum0 = int(pas_cusnum / PAS_DMD_PEAK.size());
        int per_cusnum1 = int(fre_cusnum / FRE_DMD_PEAK.size());
        for(int p = 0; p < PAS_DMD_PEAK.size(); p++)
        {
            int cusnum_p = (p < PAS_DMD_PEAK.size()-1) ? per_cusnum0 : (pas_cusnum - per_cusnum0 * (PAS_DMD_PEAK.size()-1));
            sampleNumPerMode_Pas.push_back(cusnum_p);
        }
        for(int f = 0; f < FRE_DMD_PEAK.size(); f++)
        {
            int cusnum_f = (f < FRE_DMD_PEAK.size()-1) ? per_cusnum1 : (fre_cusnum - per_cusnum1 * (FRE_DMD_PEAK.size()-1));
            sampleNumPerMode_Fre.push_back(cusnum_f);
        }
        vector<int> raw_serve_start_0 = convert_vectype_to_int(generateMultiModalData(sampleNumPerMode_Pas, PAS_DMD_PEAK, {100,100}, PLAN_HORIZON[0], PLAN_HORIZON[1]));
        vector<int> raw_serve_start_1 = convert_vectype_to_int(generateMultiModalData(sampleNumPerMode_Fre, FRE_DMD_PEAK, {100,100}, PLAN_HORIZON[0], PLAN_HORIZON[1]));
        serve_start.insert(serve_start.end(), raw_serve_start_0.begin(), raw_serve_start_0.end());
        serve_start.insert(serve_start.end(), raw_serve_start_1.begin(), raw_serve_start_1.end());
    }
    else //! even
    {
        uniform_int_distribution<> uniform_dis(PLAN_HORIZON[0], PLAN_HORIZON[1]);
        for(int i = 0; i < pas_cusnum + fre_cusnum; i++)
        {
            serve_start.push_back(uniform_dis(gen));
        }
    }
    RandomNumber r;
    for(int i = 0; i < nodenum_wodepot; i++)
    {
        if(nodetypes[i+1] == 0)
        {
            serviceTW.push_back({serve_start[i], serve_start[i] + r.get_rint(MIN_SER_PRD_0, MAX_SER_PRD_0)});
        }
        else if(nodetypes[i+1] == 1)
        {
            serviceTW.push_back({serve_start[i], serve_start[i] + r.get_rint(MIN_SER_PRD_1, MAX_SER_PRD_1)});
        }
        else if(nodetypes[i+1] == 2)
        {
            serviceTW.push_back(PLAN_HORIZON);
        }
        else
        {
            assert(false);
        }
    }
}

void RawInstance::designDemands() 
{
    RandomNumber r;
    for(int i = 0; i < nodetypes.size(); i++)
    {
        if(nodetypes[i] == 0)
        {
            demands.push_back(r.get_rint(DMD_RANGE_0[0], DMD_RANGE_0[1]));
        }
        else if(nodetypes[i] == 1)
        {
            demands.push_back(r.get_rint(DMD_RANGE_1[0], DMD_RANGE_1[1]));
        } 
        else if(nodetypes[i] == 2)
        {
            demands.push_back(0);
        }
    }
    for(int i = 0; i < demands.size(); i++) //! some are pickup, some are dropoff
    {
        if(nodetypes[i] != 2)
        {
            if(r.get_rflt() < PICKUP_DROPOFF_PROB)
            {
                demands[i] *= -1;
            }
        }
    }
}

void RawInstance::designSerTime()
{
    RandomNumber r;
    for(int i = 0; i < nodetypes.size(); i++)
    {
        if(nodetypes[i] == 0)
        {
            serviceTime.push_back(int(round(abs(demands[i])*SER_RATE_0)));
        }
        else if(nodetypes[i] == 1)
        {
            serviceTime.push_back(int(round(abs(demands[i])*SER_RATE_1)));
        } 
        else if(nodetypes[i] == 2)
        {
            serviceTime.push_back(0);
        }
    }
}

void RawInstance::designVehInfo()
{
    int pas_dmd = 0, fre_dmd = 0;
    for(int i = 0; i < nodetypes.size(); i++)
    {
        if(nodetypes[i] == 0)
        {
            pas_dmd += abs(demands[i]);
        }
        else if(nodetypes[i] == 1)
        {
            fre_dmd += abs(demands[i]);
        }
    }
    pas_vehnum = int(ceil(pas_dmd * 1.0 / VCAP_0));
    fre_vehnum = int(ceil(fre_dmd * 1.0 / VCAP_1));
    vehnum = pas_vehnum + fre_vehnum;
    pas_vehcap = VCAP_0;
    fre_vehcap = VCAP_1;
    veh_speed = SPEED;
}

void RawInstance::designdata()
{
    // rownum = nodenum + 2;
    // designNodeTypes(nodenum); //! nodetypes
    // designCoord(cluster);   //! coordinates
    // designSerTW(peak); //! serviceTW
    // designDemands(); //! demands
    // designSerTime();  //! serviceTime
    // designVehInfo();  //! vehnum
    data_vec.resize(rownum);
    data_vec[0].push_back(pas_vehnum);
    data_vec[0].push_back(fre_vehnum);
    data_vec[0].push_back(pas_vehcap);
    data_vec[0].push_back(fre_vehcap);
    data_vec[0].push_back(veh_speed);
    for(int i = 1; i < rownum; i++)
    {
        data_vec[i].push_back(i-1);
        data_vec[i].push_back(coordinates[i-1][0]);
        data_vec[i].push_back(coordinates[i-1][1]);
        data_vec[i].push_back(demands[i-1]);
        data_vec[i].push_back(serviceTW[i-1][0]);
        data_vec[i].push_back(serviceTW[i-1][1]);
        data_vec[i].push_back(serviceTime[i-1]);
        data_vec[i].push_back(nodetypes[i-1]);
    }
}

void RawInstance::writedata(string dirpath, string filename)
{
    designdata(); //! data_vec
    ofstream outputFile(dirpath + "/" + filename + ".txt");
    if (outputFile.is_open())
    {
        for(int i = 0; i < data_vec.size(); i++)
        {
            for(int j = 0; j < data_vec[i].size(); j++)
            {
                if((i > 0) && (j == 1 || j == 2)) //! coordinates
                {
                    outputFile << fixed << setprecision(2) << data_vec[i][j] << "\t";
                }
                else
                {
                    outputFile << int(data_vec[i][j]) << "\t";
                }
            }
            outputFile << "\n";
        }
        outputFile.close();
        cout << "Dataset " + filename + " is saved to " + filename + ".txt\n";
    }
}

vector<double> RawInstance::getRowData(int rowid)
{
    assert(rowid < rownum);
    return this->data_vec[rowid];
}

vector<int> RawInstance::getPlanHorizon()
{
    return {static_cast<int>(data_vec[1][4]), static_cast<int>(data_vec[1][5])};
}

int RawInstance::getVehNum(int type)
{
    if(type == 0)
    {
        return pas_vehnum;
    }
    else if(type == 1)
    {
        return fre_vehnum;
    }
    else if(type == 2)
    {
        return pas_vehnum + fre_vehnum;
    }
    assert(false);
    return 0;
}

void RawInstance::setVehNum(int type, int vehnum)
{
    if(type == 0)
    {
        this->pas_vehnum = vehnum;
    }
    else if(type == 1)
    {
        this->fre_vehnum = vehnum;
    }
    assert(false);
}

vector<int> RawInstance::getVehIDs(int type)
{
    vector<int> vehids;
    if(type == 0)
    {
        for(int i = 0; i < pas_vehnum; i++)
        {
            vehids.push_back(i);
        }
        return vehids;
    }
    else if(type == 1)
    {
        for(int i = pas_vehnum; i < fre_vehnum; i++)
        {
            vehids.push_back(i);
        }
        return vehids;
    }
    assert(false);
    return {};
}

int RawInstance::getVehCap(int type)
{
    if(type == 0)
    {
        return pas_vehcap;
    }
    else if(type == 1)
    {
        return fre_vehcap;
    }
    assert(false);
    return 0;
}

void RawInstance::setVehCap(int type, int vehcap)
{
    if(type == 0)
    {
        this->pas_vehcap = vehcap;
    }
    else if(type == 1)
    {
        this->fre_vehcap = vehcap;
    }
    assert(false);
}

vector<int> RawInstance::getNodeIDs(int type)
{
    vector<int> nodeids;
    for(int i = 0; i < nodetypes.size(); i++)
    {
        if(nodetypes[i] == type)
        {
            nodeids.push_back(i);
        }
    }
    return nodeids;
}