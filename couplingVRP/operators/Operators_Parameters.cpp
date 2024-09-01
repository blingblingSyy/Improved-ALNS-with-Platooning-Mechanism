#include "couplingVRP/operators/Operators_Parameters.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "src/lib/tinyxml/tinyxml.h"

using namespace std;

Operators_Parameters::Operators_Parameters()
{
	// The variables are initialized with default values.

}

Operators_Parameters::~Operators_Parameters()
{

}

void Operators_Parameters::loadXMLParameters(string path)
{
	TiXmlDocument doc(path.c_str());
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		TiXmlElement* root = doc.FirstChildElement("Parameters");
		if (root)
		{
			TiXmlElement* paramOps = root->FirstChildElement("Operators");
			if(paramOps)
			{
				TiXmlElement* pathDesRate = paramOps->FirstChildElement( "PathDestroyRate" );
				if(pathDesRate)
				{
					stringstream str;
					str << pathDesRate->GetText();
					str >> this->pathDestroyRate;
				}
				TiXmlElement* nodeDesRate = paramOps->FirstChildElement( "NodeDestroyRate" );
				if(nodeDesRate)
				{
					stringstream str;
					str << nodeDesRate->GetText();
					str >> this->nodeDestroyRate;
				}
				TiXmlElement* shawrate1 = paramOps->FirstChildElement( "ShawRate1" );
				if(shawrate1)
				{
					stringstream str;
					str << shawrate1->GetText();
					str >> this->shawRate1;
				}
				TiXmlElement* shawrate2 = paramOps->FirstChildElement( "ShawRate2" );
				if(shawrate2)
				{
					stringstream str;
					str << shawrate2->GetText();
					str >> this->shawRate2;
				}
				TiXmlElement* shawrate3 = paramOps->FirstChildElement( "ShawRate3" );
				if(shawrate3)
				{
					stringstream str;
					str << shawrate3->GetText();
					str >> this->shawRate3;
				}
				TiXmlElement* shawrate4 = paramOps->FirstChildElement( "ShawRate4" );
				if(shawrate4)
				{
					stringstream str;
					str << shawrate4->GetText();
					str >> this->shawRate4;
				}
				TiXmlElement* randshaw = paramOps->FirstChildElement( "RandShawDes" );
				if(randshaw)
				{
					stringstream str;
					str << randshaw->GetText();
					str >> this->randShawDes;
				}
				TiXmlElement* randworst = paramOps->FirstChildElement( "RandWorstDes" );
				if(randworst)
				{
					stringstream str;
					str << randworst->GetText();
					str >> this->randWorstDes;
				}
				TiXmlElement* randAL = paramOps->FirstChildElement( "RandAvgLenDes" );
				if(randAL)
				{
					stringstream str;
					str << randAL->GetText();
					str >> this->randWorstDes;
				}
				TiXmlElement* randcardi = paramOps->FirstChildElement( "RandCardiDes" );
				if(randcardi)
				{
					stringstream str;
					str << randcardi->GetText();
					str >> this->randCardiDes;
				}
				TiXmlElement* randNfreqDes = paramOps->FirstChildElement( "RandNfreqDes" );
				if(randNfreqDes)
				{
					stringstream str;
					str << randNfreqDes->GetText();
					str >> this->randNfreqDes;
				}
				TiXmlElement* randKL = paramOps->FirstChildElement( "RandKlenRep" );
				if(randKL)
				{
					stringstream str;
					str << randKL->GetText();
					str >> this->randKlenRep;
				}
				TiXmlElement* randNfreqRep = paramOps->FirstChildElement( "RandNfreqRep" );
				if(randNfreqRep)
				{
					stringstream str;
					str << randNfreqRep->GetText();
					str >> this->randNfreqRep;
				}
				TiXmlElement* tenure = paramOps->FirstChildElement( "Tenure" );
				if(tenure)
				{
					stringstream str;
					str << tenure->GetText();
					str >> this->tenure;
				}
			}
		}
	}
}

void Operators_Parameters::loadParameters(string path)
{
	ifstream comingFlow(path.data());  //! .data(): return the pointer to the contents
	string line;
    //! while(getline(comingFlow, line)): The operator void*() cast operator returns some non-NULL pointer 
    //! if the stream is in a good state, or NULL if it’s in a failed state
	while(getline(comingFlow, line))  //! the default delimiter is the endline character
	{
		cout << "[DBG] " << line << endl;
		if(line.size() > 0 && line.at(0) != '#')
		{
			stringstream str;
			str << line;
			string id;
			string eq;
			str >> id;
			str >> eq;
			if(id == "PathDestroyRate")
			{
				str >> pathDestroyRate;
			}
			else if(id == "NodeDestroyRate")
			{
				str >> nodeDestroyRate;
			}
			else if(id == "ShawRate1")
			{
				str >> shawRate1;
			}
			else if(id == "ShawRate2")
			{
				str >> shawRate2;
			}
			else if(id == "ShawRate3")
			{
				str >> shawRate3;
			}
			else if(id == "ShawRate4")
			{
				str >> shawRate4;
			}
			else if(id == "RandShawDes")
			{
				str >> randShawDes;
			}
			else if(id == "RandWorstDes")
			{
				str >> randWorstDes;
			}
			else if(id == "RandAvgLenDes")
			{
				str >> randAvgLenDes;
			}
			else if(id == "RandCardiDes")
			{
				str >> randCardiDes;
			}
			else if(id == "RandNfreqDes")
			{
				str >> randNfreqDes;
			}
			else if(id == "RandKlenRep")
			{
				str >> randKlenRep;
			}
			else if(id == "RandNfreqRep")
			{
				str >> randNfreqRep;
			}
			else if(id == "Tenure")
			{
				str >> tenure;
			}
			else
			{
				assert(false);
			}
		}
	}
}

void Operators_Parameters::sanityChecks()
{

}

Operators_Parameters::Operators_Parameters(Operators_Parameters& p)
{
	pathDestroyRate = p.pathDestroyRate;

	nodeDestroyRate = p.nodeDestroyRate;

	shawRate1 = p.shawRate1;

	shawRate2 = p.shawRate2;

	shawRate3 = p.shawRate3;

	shawRate4 = p.shawRate4;

	randShawDes = p.randShawDes;

	randWorstDes = p.randWorstDes;

	randAvgLenDes = p.randAvgLenDes;

	randCardiDes = p.randCardiDes;

	randNfreqDes = p.randNfreqDes;

	randKlenRep = p.randKlenRep;

	randNfreqRep = p.randNfreqRep;

	tenure = p.tenure;

	// minDestroyPerc = p.minDestroyPerc;

	// maxDestroyPerc = p.maxDestroyPerc;
}
