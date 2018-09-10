/*
 * ObservationPlan.h
 *
 *  Created on: Oct 11, 2017
 *      Author: ismail
 */

#ifndef OBSERVATIONPLAN_H_
#define OBSERVATIONPLAN_H_
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sys/time.h>
#include<time.h>
#include <stdint.h>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string

using namespace std;

struct slot {
	string slotID, ObjectID, StartTime, EndTime;
	vector<vector<pair<string, string> > > mounts;
	vector<vector<pair<string, string> > > images;
};

class ObservationPlan {
public:
	ObservationPlan();
	virtual ~ObservationPlan();
	int readPlan();
	int initializeXML();
	int applyPlan(slot);
	void waitMicroSec(long microSec);
	void waitSec(long sec);
	uint64_t GetTimeStamp();
	void printTimeStamp();
	vector<string> timeParser(string planTime);
	void parsedToDateTime(vector<string>);
	long long return_current_time_and_date();
	long long parsedToDateMilliSec(vector<string> parsedDateTime);
	long long nonParsedToDateMilliSec(string parsedDateTime);
	int nonparsedToDateMicroSec(string parsedDateTime);

	vector<pair<string, string> > imageLine;
	vector<vector<pair<string, string> > > imageLines;

	vector<pair<string, string> > mountLine;
	vector<vector<pair<string, string> > > mountLines;
	vector<slot> slotsVect;
        void clearSlots();

private:

	int slotNumber = 0;
};

#endif /* OBSERVATIONPLAN_H_ */
