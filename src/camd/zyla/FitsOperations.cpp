#include"fitsio.h"
#include"atcore.h"
#include<time.h>
#include<stdio.h>
#include<iostream>
#include<chrono>
#include<string>
#include <sstream> // stringstream
#include <iomanip> // put_time
#include"FitsOperations.h"
int createFits(unsigned short *arr, double d_newExposure)
{
	fitsfile *fptr=NULL;
	int status, ii, jj;
	long  fpixel = 1, naxis = 2, nelements, exposure;
	long naxes[2] = { 2560, 2160 };
	
	status = 0;

	
	std::string str = return_current_time_and_date();
	
	
	std::string total = std::string(str) + std::string(".fits");
	
	fits_create_file(&fptr,total.c_str(), &status);

	fits_create_img(fptr, ULONG_IMG, naxis, naxes, &status);

	
	fits_update_key(fptr, TDOUBLE, "EXPOSURE", &d_newExposure,
		"Total Exposure Time", &status);


	nelements = naxes[0] * naxes[1];


	fits_write_img(fptr, TUSHORT, fpixel, nelements, arr, &status);

	fits_close_file(fptr, &status);

	fits_report_error(stderr, status);

	return status;
}


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
void currentDateTime()
{



/*
	typedef std::chrono::system_clock Clock;

	std::chrono::system_clock::time_point now = Clock::now();
	std::chrono::seconds seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	std::chrono::fraction fraction = now - seconds;
	time_t cnow = Clock::to_time_t(now);
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
	//std::cout << milliseconds.count() << '\n';
	//return &milliseconds.count ;

	auto t = std::chrono::high_resolution_clock::now();
	std::cout << t.time_since_epoch().count ();

	//std::stringstream str_frac;
	//str_frac << fractional_seconds;
	//return str_frac.str();

*/
}

std::string return_current_time_and_date()
{



	std::chrono::high_resolution_clock::time_point p = std::chrono::high_resolution_clock::now();

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch());

	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
	std::time_t t = s.count();
	std::size_t fractional_seconds = ms.count() % 1000;

	
	//char str[26];
	//ctime_s(str, sizeof str, &t);
	//printf("%s", str);
	//std::cout << std::ctime(&t) << std::endl;
	//std::cout << fractional_seconds << std::endl;
	std::stringstream str_frac;
	str_frac<< t<<"_"<<fractional_seconds;
	//std::cout << "fonksiyondan donen" << str_frac.str().c_str() << std::endl;
	return str_frac.str();

}
