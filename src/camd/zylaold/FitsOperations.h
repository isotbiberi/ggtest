#ifndef FITSOPERATIONS_H
#define FITSOPERATIONS_H
#include"atcore.h"
#include<string.h>
#include<map>
#include<string>
extern int createFits(unsigned short *arr, std::map<std::string,std::pair<std::string,std::string>> header);
void currentDateTime();
std::string return_current_time_and_date();
std::string dateTimeMs();
struct FitsMetaData {
	double expTime;
	std::string expStartDate;
	std::string expStopDate;
};
#endif
