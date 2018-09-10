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
#include<iomanip>
#include <sys/time.h>
int createFits(unsigned short *arr, std::map<std::string,std::pair<std::string, std::string>> header)
{
	fitsfile *fptr=NULL;
	int status, ii, jj;
	long  fpixel = 1, naxis = 2, nelements, exposure;
	long naxes[2] = { 2560, 2160 };
	
	status = 0;

        //std::string folder = "fitssil/";	
	std::string str = return_current_time_and_date();
	
	
	std::string total = std::string("fitssil/") + std::string(str) + std::string(".fits");
	
	fits_create_file(&fptr,total.c_str(), &status);

	fits_create_img(fptr, ULONG_IMG, naxis, naxes, &status);

       // int d_new = 23;
       // std::string newone="xxxs";
       //	fits_update_key(fptr, TSTRING, "test", (void *)newone.c_str(),"TSTRING TEST", &status);

        for(auto& t : header)
	{
               //d_newExposure =t.second;
  //             fits_update_key(fptr, TSHORT, "ALT", &d_new,"Total Exposure Time", &status);
 //fits_update_key(fptr, TSHORT, "AZ", &t.second,"azimuth", &status);
  fits_update_key(fptr, TSTRING, t.first.c_str(), (void *)t.second.first.c_str(),t.second.second.c_str(), &status);
 

	}
 
	//fits_update_key(fptr, TDOUBLE, "EXPOSURE", &d_newExposure,
	//	"Total Exposure Time", &status);


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
void waitMicroSec(long microsec)
{
timeval   timeout = {0 /* seconds */, microsec /* and microseconds */};
if (select(0, NULL, NULL, NULL, &timeout) != 0)
{
      // Need to deal with errors like EINTR
}


}



void   printTimeStamp()
{


 struct timeval tv;
 struct tm* ptm;
 char time_string[40];
 long milliseconds;

 /* Obtain the time of day, and convert it to a tm struct. */
gettimeofday (&tv, NULL);
 ptm = localtime (&tv.tv_sec);
 /* Format the date and time, down to a single second. */
 strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
 /* Compute milliseconds from microseconds. */
 milliseconds = tv.tv_usec / 1000;
 /* Print the formatted time, in seconds, followed by a decimal point
   and the milliseconds. */
 printf ("%s.%03ld\n", time_string, milliseconds);



}


    uint64_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}



