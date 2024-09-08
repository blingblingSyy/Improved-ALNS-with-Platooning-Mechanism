#include "alns/improvedALNS/ALNS_Parameters.h"
#include "lib/tinyxml/tinyxml.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

ALNS_Parameters::ALNS_Parameters()
{
	// The variables are initialized with default values.
	maxNbIterations = 10000;

	lookBackIterations = 1000;

	objImpThreshold = 0.001;

	maxRunningTime = 10;

	maxNbIterationsNoImp = 1000;

	stopCrit = ALL;

	globalNoise = false;

	timeSegmentsIt = 100;

	// nbItBeforeReinit = 1000;

	sigma1 = 9;

	sigma2 = 7;

	sigma3 = 2;

	rho = 0.8;

	minimumWeight = 0.1;

	maximumWeight = 10;

	probabilityOfNoise = 0;

	acKind = SA;

	// acPath = "";

	logFrequency = 1;

	performLocalSearch = true;

	reloadFrequency = 1000;

	lock = false;
}

ALNS_Parameters::~ALNS_Parameters()
{

}

void ALNS_Parameters::loadXMLParameters(std::string path)
{
	TiXmlDocument doc(path.c_str());
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		TiXmlElement* root = doc.FirstChildElement( "Parameters" );
		if ( root )
		{
			TiXmlElement* paramALNS = root->FirstChildElement( "ALNS" );
			if(paramALNS)
			{
				TiXmlElement* maxNbIt = paramALNS->FirstChildElement( "MaxNbIterations" );
				if(maxNbIt)
				{
					stringstream str;
					str << maxNbIt->GetText();
					str >> this->maxNbIterations;
				}
				TiXmlElement* minNbIt = paramALNS->FirstChildElement( "MinNbIterations" );
				if(minNbIt)
				{
					stringstream str;
					str << minNbIt->GetText();
					str >> this->minNbIterations;
				}
				TiXmlElement* lookbackIters = paramALNS->FirstChildElement( "LookBackIterations" );
				if(lookbackIters)
				{
					stringstream str;
					str << lookbackIters->GetText();
					str >> this->lookBackIterations;
				}
				TiXmlElement* threshold = paramALNS->FirstChildElement( "ObjImpThreshold" );
				if(threshold)
				{
					stringstream str;
					str << threshold->GetText();
					str >> this->objImpThreshold;
				}
				TiXmlElement* maxRt = paramALNS->FirstChildElement( "MaxRunTime" );
				if(maxRt)
				{
					stringstream str;
					str << maxRt->GetText();
					str >> this->maxRunningTime;
				}
				TiXmlElement* maxNoImp = paramALNS->FirstChildElement( "MaxNbIterationsNoImp" );
				if(maxNoImp)
				{
					stringstream str;
					str << maxNoImp->GetText();
					str >> this->maxNbIterationsNoImp;
				}
				TiXmlElement* stopCriter = paramALNS->FirstChildElement( "StoppingCriterion" );
				if(stopCriter)
				{
					stringstream str;
					string temp;
					str << stopCriter->GetText();
					str >> temp;
					if(temp == "MAX_IT")
					{
						stopCrit = MAX_IT;
					}
					else if(temp == "MAX_RT")
					{
						stopCrit = MAX_RT;
					}
					else if(temp == "MAX_IT_NO_IMP")
					{
						stopCrit = MAX_IT_NO_IMP;
					}
					else if(temp == "ALL")
					{
						stopCrit = ALL;
					}
					else
					{
						assert(false);
					}
				}
				TiXmlElement* noiseP = paramALNS->FirstChildElement( "Noise" );
				if(noiseP)
				{
					stringstream str;
					string temp;
					str << noiseP->GetText();
					str >> temp;
					if(temp=="true")
					{
						globalNoise = true;
					}
					else
					{
						globalNoise = false;
					}
				}
				TiXmlElement* timeSeg = paramALNS->FirstChildElement( "TimeSegmentIt" );
				if(timeSeg)
				{
					stringstream str;
					str << timeSeg->GetText();
					str >> this->timeSegmentsIt;
				}
				// TiXmlElement* nbItBefRe = paramALNS->FirstChildElement( "NbItBeforeReinit" );
				// if(nbItBefRe)
				// {
				// 	stringstream str;
				// 	str << nbItBefRe->GetText();
				// 	str >> this->nbItBeforeReinit;
				// }
				TiXmlElement* sig1 = paramALNS->FirstChildElement( "Sigma1" );
				if(sig1)
				{
					stringstream str;
					str << sig1->GetText();
					str >> this->sigma1;
				}
				TiXmlElement* sig2 = paramALNS->FirstChildElement( "Sigma2" );
				if(sig2)
				{
					stringstream str;
					str << sig2->GetText();
					str >> this->sigma1;
				}
				TiXmlElement* sig3 = paramALNS->FirstChildElement( "Sigma3" );
				if(sig3)
				{
					stringstream str;
					str << sig3->GetText();
					str >> this->sigma1;
				}
				TiXmlElement* rho = paramALNS->FirstChildElement( "Rho" );
				if(rho)
				{
					stringstream str;
					str << rho->GetText();
					str >> this->rho;
				}
				TiXmlElement* miniW = paramALNS->FirstChildElement( "MinimumWeight" );
				if(miniW)
				{
					stringstream str;
					str << miniW->GetText();
					str >> this->minimumWeight;
				}
				TiXmlElement* maxiW = paramALNS->FirstChildElement( "MaximumWeight" );
				if(maxiW)
				{
					stringstream str;
					str << maxiW->GetText();
					str >> this->maximumWeight;
				}
				TiXmlElement* pbNoise = paramALNS->FirstChildElement( "ProbabilityOfNoise" );
				if(pbNoise)
				{
					stringstream str;
					str << pbNoise->GetText();
					str >> this->probabilityOfNoise;
				}
				TiXmlElement* acK = paramALNS->FirstChildElement( "AcKind" );
				if(acK)
				{
					stringstream str;
					string temp;
					str << acK->GetText();
					str >> temp;
					if(temp == "SA")
					{
						acKind = SA;
					}
					else
					{
						assert(false);
					}
				}
				TiXmlElement* relFreq = paramALNS->FirstChildElement( "ReloadFrequency" );
				if(relFreq)
				{
					stringstream str;
					str << relFreq->GetText();
					str >> this->reloadFrequency;
				}
				TiXmlElement* sglPath = paramALNS->FirstChildElement( "StatsGlobPath" );
				if(sglPath)
				{
					this->statsGlobPath = sglPath->GetText();
				}
				TiXmlElement* sopPath = paramALNS->FirstChildElement( "StatsOpPath" );
				if(sopPath)
				{
					this->statsOpPath = sopPath->GetText();
				}
				TiXmlElement* logFreq = paramALNS->FirstChildElement( "LogFrequency" );
				if(logFreq)
				{
					stringstream str;
					str << logFreq->GetText();
					str >> this->logFrequency;
				}
				TiXmlElement* forbOp = paramALNS->FirstChildElement( "ForbiddenOperators" );
				if(forbOp)
				{
					TiXmlElement* firstForb = forbOp->FirstChildElement();
					while(firstForb)
					{
						addForbiddenOperator(firstForb->GetText());
						firstForb = firstForb->NextSiblingElement();
					}
				}
				TiXmlElement* doLS = paramALNS->FirstChildElement( "PerformLocalSearch" );
				if(doLS)
				{
					stringstream str;
					string temp;
					str << doLS->GetText();
					str >> temp;
					if(temp=="true")
					{
						performLocalSearch = true;
					}
					else
					{
						performLocalSearch = false;
					}
				}
				TiXmlElement* forbLSOp = paramALNS->FirstChildElement( "ForbiddenLSOperators" );
				if(forbLSOp)
				{
					TiXmlElement* firstOp = forbLSOp->FirstChildElement();
					while(firstOp)
					{
						addForbiddenLsOperator(firstOp->GetText());
						firstOp = firstOp->NextSiblingElement();
					}
				}
			}
		}
	}
}

void ALNS_Parameters::loadParameters(string path)
{
	ifstream comingFlow(path.data());
	string line;
	while(getline(comingFlow, line))
	{
		cout << "[DBG] " << line << endl;
		if(line.size() > 0 && line.at(0) != '#')
		{
			stringstream str;
			str << line;
			string id;
			string eq;
			str >> id;
			str >> eq;    //! for the title line
			if(id == "maxNbIterations")
			{
				str >> maxNbIterations;
			}
			if(id == "minNbIterations")
			{
				str >> minNbIterations;
			}
			else if(id == "lookBackIterations")
			{
				str >> lookBackIterations;
			}
			else if(id == "objImpThreshold")
			{
				str >> objImpThreshold;
			}
			else if(id == "maxRunningTime")
			{
				str >> maxRunningTime;
			}
			else if(id == "maxNbIterationsNoImp")
			{
				str >> maxNbIterationsNoImp;
			}
			else if(id == "stoppingCriterion")
			{
				string temp;
				str >> temp;
				if(temp == "MAX_IT")
				{
					stopCrit = MAX_IT;
				}
				else if(temp == "MAX_RT")
				{
					stopCrit = MAX_RT;
				}
				else if(temp == "MAX_IT_NO_IMP")
				{
					stopCrit = MAX_IT_NO_IMP;
				}
				else if(temp == "ALL")
				{
					stopCrit = ALL;
				}
				else
				{
					assert(false);
				}
			}
			else if(id == "noise")
			{
				string temp;
				str >> temp;
				if(temp == "true")
				{
					globalNoise = true;
				}
				else
				{
					globalNoise = false;
				}
			}
			else if(id == "timeSegmentIt")
			{
				str >> timeSegmentsIt;
			}
			// else if(id == "nbItBeforeReinit")
			// {
			// 	str >> nbItBeforeReinit;
			// }
			else if(id == "sigma1")
			{
				str >> sigma1;
			}
			else if(id == "sigma2")
			{
				str >> sigma2;
			}
			else if(id == "sigma3")
			{
				str >> sigma3;
			}
			else if(id == "rho")
			{
				str >> rho;
			}
			else if(id == "minimumWeight")
			{
				str >> minimumWeight;
			}
			else if(id == "maximumWeight")
			{
				str >> maximumWeight;
			}
			else if(id == "probabilityOfNoise")
			{
				str >> probabilityOfNoise;
			}
			else if(id == "reloadFrequency")
			{
				str >> reloadFrequency;
			}
			else if(id == "acKind")
			{
				string temp;
				str >> temp;
				if(temp == "SA")
				{
					acKind = SA;
				}
				else if(temp == "DM")
				{
					acKind = DM;
				}
				else
				{
					assert(false);
				}
			}
			// else if(id == "acPath")
			// {
			// 	str >> acPath;
			// }
			else if(id == "statsGlobPath")
			{
				str >> statsGlobPath;
			}
			else if(id == "statsOpPath")
			{
				str >> statsOpPath;
			}
			else if(id == "logFrequency")
			{
				str >> logFrequency;
			}
			else if(id == "performLocalSearch")
			{
				string temp;
				str >> temp;
				if(temp == "true")
				{
					performLocalSearch=true;
				}
				else
				{
					performLocalSearch=false;
				}
			}
			else if(id == "forbiddenOperator")
			{
				int posEq = line.find('=',0);
				string ss = line.substr(posEq+2,line.length());
				addForbiddenOperator(ss);
			}
			else if(id == "forbiddenLsOperator")
			{
				int posEq = line.find('=',0);
				string ss = line.substr(posEq+2,line.length());
				addForbiddenLsOperator(ss);
			}
			else
			{
				assert(false);
			}
		}
	}
}

void ALNS_Parameters::sanityChecks()
{

}

ALNS_Parameters::ALNS_Parameters(ALNS_Parameters& p)
{
	maxNbIterations = p.maxNbIterations;

	minNbIterations = p.minNbIterations;

	lookBackIterations = p.lookBackIterations;

	objImpThreshold = p.objImpThreshold;

	maxRunningTime = p.maxRunningTime;

	maxNbIterationsNoImp = p.maxNbIterationsNoImp;

	stopCrit = p.stopCrit;

	globalNoise = p.globalNoise;

	timeSegmentsIt = p.timeSegmentsIt;

	// nbItBeforeReinit = p.nbItBeforeReinit;

	sigma1 = p.sigma1;

	sigma2 = p.sigma2;

	sigma3 = p.sigma3;

	rho = p.rho;

	minimumWeight = p.minimumWeight;

	maximumWeight = p.maximumWeight;

	acKind = p.acKind;

	// acPath = p.acPath;

	logFrequency = p.logFrequency;

	reloadFrequency = p.reloadFrequency;

	probabilityOfNoise = p.probabilityOfNoise;

	performLocalSearch = p.performLocalSearch;

	// statsGlobPath = p.statsGlobPath;

	// statsOpPath = p.statsOpPath;

	// forbidenOperators = p.forbidenOperators;

	// forbidenLsOperators = p.forbidenLsOperators;

	lock = false;
}
