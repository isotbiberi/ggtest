#ifndef FITSOPERATIONS_H
#define FITSOPERATIONS_H
#include"atcore.h"
#include<string.h>
extern int createFits(unsigned short *arr, double d_newExposure);
void currentDateTime();
std::string return_current_time_and_date();
std::string dateTimeMs();
struct FitsMetaData {
	double expTime;
	std::string expStartDate;
	std::string expStopDate;
};
#endif