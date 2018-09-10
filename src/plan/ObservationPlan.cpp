/*
 * ObservationPlan.cpp
 *
 *  Created on: Oct 11, 2017
 *      Author: ismail
 */

#include "ObservationPlan.h"

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <stdexcept>
#include <utility>

using namespace std;
using namespace xercesc;
using namespace std;
using namespace xercesc_3_1;

#include <fstream>
#include <sstream>

ObservationPlan::ObservationPlan() {
	// TODO Auto-generated constructor stub

}

ObservationPlan::~ObservationPlan() {
	// TODO Auto-generated destructor stub
}

int ObservationPlan::initializeXML() {
	try {
		XMLPlatformUtils::Initialize();
	} catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Error during initialization! :\n" << message << "\n";
		XMLString::release(&message);
		return 1;
	}
}

int ObservationPlan::applyPlan(slot selectedSlot)

{

	for (int i = 0; i < selectedSlot.mounts.size(); i++) {
		// parsedToDateTime (timeParser(selectedSlot.mounts[i][1].second));
		// std::cout<<parsedToDateMilliSec(timeParser(selectedSlot.mounts[i][1].second))<<endl;

		// std::cout<<parsedToDateMilliSec(timeParser(selectedSlot.mounts[i][1].second))-return_current_time_and_date()<<endl;

		cout << "mount time is in epoch ms " << selectedSlot.mounts[i][1].second
				<<std::endl<< "mount time is in microseconds " << selectedSlot.mounts[i][2].second
				<<std::endl<< "mount is moving to azimuth : "
				<< selectedSlot.mounts[i][3].second << " Elevation : "
				<< selectedSlot.mounts[i][4].second << endl;
		        cout << "exp start time time is in epoch ms "<< selectedSlot.images[i][1].second
				<<std::endl		<< "exp start time microseconds is "<< selectedSlot.images[i][2].second
				<<std::endl	<< "Exptime is :"<< selectedSlot.images[i][3].second
				<<std::endl	<< "Number of exp is "<< selectedSlot.images[i][4].second << endl;
		cout << "difference is "
				<< std::stoll(selectedSlot.mounts[i][1].second)- std::stoll(selectedSlot.images[i][1].second)
				<< std::endl;

		//parsedToDateTime (timeParser(selectedSlot.mounts[i][1].second));
		//  cout <<" Exp start time is " << selectedSlot.images[i][1].second <<endl;

	}
	return 0;

}

int ObservationPlan::readPlan() {

	initializeXML();
	XercesDOMParser* parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true); // optional

	ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
	parser->setErrorHandler(errHandler);

	const char * xmlFile = "ObservationPlan.xml";

	try {
		parser->parse(xmlFile);
	} catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n" << message << "\n";
		XMLString::release(&message);
		return -1;
	} catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		cout << "Exception message is: \n" << message << "\n";
		XMLString::release(&message);
		return -1;
	} catch (...) {
		cout << "Unexpected Exception \n";
		return -1;
	}

	cout << xmlFile << ": parse OK" << endl;

	try

	{
		// no need to free this pointer - owned by the parent parser object
		DOMDocument* xmlDoc = parser->getDocument();

		// Get the top-level element: NAme is "root". No attributes for "root"

		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		if (!elementRoot)
			throw(std::runtime_error("empty XML document"));

		// Parse XML file for tags of interest: "ApplicationSettings"
		// Look one level nested within "root". (child of root)

		DOMNodeIterator * walker;
		walker = xmlDoc->createNodeIterator(elementRoot,
				DOMNodeFilter::SHOW_ELEMENT, NULL, true);

		// Some declarations
		DOMNode * current_node = NULL;
		string thisNodeName;
		string parentNodeName;
		bool imageLineParts[7] = { false, false, false, false, false, false,
				false };
		bool mountLineParts[4] = { false, false, false, false };
		bool slotParts[4] = { false, false, false, false };

	for (current_node = walker->nextNode(); current_node != 0;
			current_node = walker->nextNode()) {

		thisNodeName = XMLString::transcode(current_node->getNodeName());
		parentNodeName = XMLString::transcode(
				current_node->getParentNode()->getNodeName());

		//	std::cout<<"this node is "<<thisNodeName<<" parent is " <<parentNodeName<<endl;

		if (parentNodeName == "Slots")

		{

			//   std::cout<<"slot if true"<<endl;

			slotsVect.push_back(slot());
			//slotsVectSet  = true;

			if (thisNodeName == "SlotID") {
				slotParts[0] = true;
				slotsVect[slotNumber].slotID = XMLString::transcode(
						current_node->getFirstChild()->getNodeValue());

			} else if (thisNodeName == "ObjectID") {
				slotParts[1] = true;
				slotsVect[slotNumber].ObjectID = XMLString::transcode(
						current_node->getFirstChild()->getNodeValue());

			} else if (thisNodeName == "StartTime") {
				slotParts[2] = true;
				slotsVect[slotNumber].StartTime = XMLString::transcode(
						current_node->getFirstChild()->getNodeValue());

			}

			else if (thisNodeName == "EndTime") {
				slotParts[3] = true;
				slotsVect[slotNumber].EndTime = XMLString::transcode(
						current_node->getFirstChild()->getNodeValue());

			}
			if (slotParts[3] && slotParts[2] && slotParts[1]
					&& slotParts[0]) {

				slotParts[0] = false;
				slotParts[1] = false;
				slotParts[2] = false;
				slotParts[3] = false;

			}

		}			//end if Slot

		if (parentNodeName == "ImageLine")

		{

			// std::cout<<"imageline if true"<<endl;

			if (thisNodeName == "ImageLineID") {
				imageLineParts[0] = true;

				imageLine.push_back(
						std::make_pair("ImageLineID",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));
			} else if (thisNodeName == "Time") {
				imageLineParts[1] = true;

				string imageTime = XMLString::transcode(
						current_node->getFirstChild()->getNodeValue());

				//	std::cout<<"non parsed is "<<nonParsedToDateMilliSec(imageTime)-return_current_time_and_date()<<std::endl;

				/*
				 imageLine.push_back(
				 std::make_pair("Time",
				 XMLString::transcode(
				 current_node->getFirstChild()->getNodeValue())));
				 */

				/*imageLine.push_back(
						std::make_pair("Time",
								std::to_string(
										nonParsedToDateMilliSec(
												imageTime))));*/

				imageLine.push_back(std::make_pair("Time",std::to_string(parsedToDateMilliSec(timeParser(imageTime)))));
				imageLine.push_back(std::make_pair("MicroSec",std::to_string(nonparsedToDateMicroSec(timeParser(imageTime)[2]))));




			} else if (thisNodeName == "ExposureTime") {
				imageLineParts[2] = true;

				imageLine.push_back(
						std::make_pair("ExposureTime",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));

			}

			else if (thisNodeName == "FrameNumber") {
				imageLineParts[3] = true;
				imageLine.push_back(
						std::make_pair("FrameNumber",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));

			} else if (thisNodeName == "Gain") {
				imageLineParts[4] = true;
				imageLine.push_back(
						std::make_pair("Gain",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));

			} else if (thisNodeName == "Focus") {
				imageLineParts[5] = true;
				imageLine.push_back(
						std::make_pair("Focus",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));

			} else if (thisNodeName == "Binning") {
				imageLineParts[6] = true;
				imageLine.push_back(
						std::make_pair("Binning",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));

			}

			if (imageLineParts[6] && imageLineParts[5] && imageLineParts[4]
					&& imageLineParts[3] && imageLineParts[2]
					&& imageLineParts[1] && imageLineParts[0]) {
				//std::cout<<"imageline parts hepsi true oldu"<<imageLine[0].first<<std::endl;

				imageLines.push_back(imageLine);
				//std::cout<<"imageline parts hepsi true oldu"<<imageLine[0].first<<std::endl;

				imageLine.clear();
				imageLineParts[0] = false;
				imageLineParts[1] = false;
				imageLineParts[2] = false;
				imageLineParts[3] = false;
				imageLineParts[4] = false;
				imageLineParts[5] = false;
				imageLineParts[6] = false;

			}

			//std::cout<<"imageline parts hepsi true muoldu"<<imageLine[0].first<<std::endl;
			// imageLinesSet = true;
		}					//end if ImageLine

		//	std::cout<<"imageline parts hepsi true oldu"<<imageLine[0].first<<std::endl;

		if (parentNodeName == "MountLine")

		{
			// std::cout<<"mountline if true"<<endl;

			if (thisNodeName == "MountLineID") {
				mountLineParts[0] = true;

				mountLine.push_back(
						std::make_pair("MountLineID",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));
			} else if (thisNodeName == "Time") {
				mountLineParts[1] = true;
				string mountTime = XMLString::transcode(
						current_node->getFirstChild()->getNodeValue());
				/*mountLine.push_back(
						std::make_pair("Time",
								std::to_string(
										nonParsedToDateMilliSec(
												mountTime))));*/

				mountLine.push_back(std::make_pair("Time",std::to_string(parsedToDateMilliSec(timeParser(mountTime)))));
				//cout<<" time parser mount time is " <<timeParser(mountTime)[2]<<endl;
				mountLine.push_back(std::make_pair("MicroSec",std::to_string(nonparsedToDateMicroSec(timeParser(mountTime)[2]))));


			} else if (thisNodeName == "Azimuth") {
				mountLineParts[2] = true;

				mountLine.push_back(
						std::make_pair("Azimuth",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));

			}

			else if (thisNodeName == "Elevation") {
				mountLineParts[3] = true;
				mountLine.push_back(
						std::make_pair("Elevation",
								XMLString::transcode(
										current_node->getFirstChild()->getNodeValue())));

			}

			if (mountLineParts[3] && mountLineParts[2] && mountLineParts[1]
					&& mountLineParts[0]) {

				mountLines.push_back(mountLine);

				mountLine.clear();
				mountLineParts[0] = false;
				mountLineParts[1] = false;
				mountLineParts[2] = false;
				mountLineParts[3] = false;

			}

		}					//end if MountLine

	}  //end of for loop

}  //end of try

catch (xercesc::XMLException& e) {
	char* message = xercesc::XMLString::transcode(e.getMessage());
	ostringstream errBuf;
	errBuf << "Error parsing file: " << message << flush;
	XMLString::release(&message);
}

slotsVect[slotNumber].images = imageLines;
slotsVect[slotNumber].mounts = mountLines;
slotNumber++;

/*

 for (int i = 0; i < imageLines.size(); i++)
 {
 imageLine = imageLines[i];
 cout <<"printing image line "<<i+1<<endl;
 for (int i = 0; i < imageLine.size(); i++)
 {
 cout << imageLine[i].first << ", " << imageLine[i].second <<endl;
 }
 }
 */

/*
 for (int i = 0; i < mountLines.size(); i++)
 {
 mountLine = mountLines[i];
 cout <<"printing image line "<<i+1<<endl;
 for (int i = 0; i < mountLine.size(); i++)
 {
 cout << mountLine[i].first << ", " << mountLine[i].second <<endl;
 }
 }
 */

/*

 for (int i = 0; i < slotsVect[0].imageLines.size(); i++)
 {
 cout<<"ismail"<<slotsVect[0].imageLines.size()<<endl;

 //cout << slots[i].first << ", " << slots[i].second <<endl;

 }


 */

delete parser;

std::cout << "plan readed" << std::endl;
}

void ObservationPlan::clearSlots() {
slotsVect.clear();
imageLine.clear();
imageLines.clear();
mountLine.clear();
mountLines.clear();
slotNumber=0;
}



void ObservationPlan::waitMicroSec(long microsec) {
	timeval timeout = { 0 /* seconds */, microsec /* and microseconds */};
	if (select(0, NULL, NULL, NULL, &timeout) != 0) {
		// Need to deal with errors like EINTR
	}

}

void ObservationPlan::waitSec(long sec) {
	timeval timeout = { sec /* seconds */, 0 /* and microseconds */};
	if (select(0, NULL, NULL, NULL, &timeout) != 0) {
		// Need to deal with errors like EINTR
	}

}

void ObservationPlan::printTimeStamp() {

	struct timeval tv;
	struct tm* ptm;
	char time_string[40];
	long milliseconds;

	/* Obtain the time of day, and convert it to a tm struct. */
	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);
	/* Format the date and time, down to a single second. */
	strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);
	/* Compute milliseconds from microseconds. */
	milliseconds = tv.tv_usec / 1000;
	/* Print the formatted time, in seconds, followed by a decimal point
	 and the milliseconds. */
	printf("%s.%03ld\n", time_string, milliseconds);

}

uint64_t ObservationPlan::GetTimeStamp() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec;

}
vector<string> ObservationPlan::timeParser(string planTime) {
	std::string s = planTime;
	std::string milliSecond = "";
	std::string delimiter = "_";
	std::vector<string> all;
	size_t pos = 0;
	std::string token;
	string dateAndTime = "";
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		//  std::cout << token << std::endl;
		dateAndTime += token;
		all.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	//std::cout << s << std::endl;
	milliSecond += s;
	all.push_back(milliSecond);
/*
	for (std::vector<string>::const_iterator i = all.begin(); i != all.end(); ++i)
	 std::cout << *i << ' ';
*/
	return all;

}

void ObservationPlan::parsedToDateTime(vector<string> parsedDateTime) {
	struct tm tm;
	char buf[255];
	string dateString = parsedDateTime[0] + "_" + parsedDateTime[1];
	memset(&tm, 0, sizeof(struct tm));

	strptime(dateString.c_str(), "%Y%m%d_%H%M%S", &tm);
	strftime(buf, sizeof(buf), "%d %b %Y %H:%M:%S", &tm);
	puts(buf);

	//exit(EXIT_SUCCESS);

}

long long ObservationPlan::parsedToDateMilliSec(vector<string> parsedDateTime) {
	/*  struct tm tm;
	 char buf[255];
	 string dateString = parsedDateTime[0]+"_"+ parsedDateTime[1];
	 memset(&tm, 0, sizeof(struct tm));

	 strptime(dateString.c_str(), "%Y%m%d_%H%M%S", &tm);
	 strftime(buf, sizeof(buf), "%d %b %Y %H:%M:%S", &tm);
	 puts(buf);


	 //exit(EXIT_SUCCESS);
	 *
	 */
	std::tm tm = { };
	std::string vectorToString = parsedDateTime[0] + "_" + parsedDateTime[1]
			+ "." + parsedDateTime[2];
	//std::cout << "vector string is " << vectorToString << std::endl;
	const char* snext = ::strptime(vectorToString.c_str(), "%Y%m%d_%H%M%S",
			&tm);
	auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
	long long duration_ms = time_point.time_since_epoch()
			/ std::chrono::milliseconds(1) + std::atof(snext) * 1000.0f;
	return duration_ms;

}


int ObservationPlan::nonparsedToDateMicroSec(string parsedDateTime)
{
	//std::cout<<"input is " <<parsedDateTime<<std::endl;

    if(parsedDateTime.size()>3)
	return std::stoi(parsedDateTime.substr(3,3));

    return 0;
}



long long ObservationPlan::nonParsedToDateMilliSec(string parsedDateTime) {

	std::tm tm = { };
	const char* snext = ::strptime(parsedDateTime.c_str(), "%Y%m%d_%H%M%S",
			&tm);
	auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
	long long duration_ms = time_point.time_since_epoch()
			/ std::chrono::milliseconds(1) + std::atof(snext) * 1000.0f;
	return duration_ms;

}

long long ObservationPlan::return_current_time_and_date() {
	/*
	 std::tm tm = {};
	 std::stringstream ss2("Jan 9 2014 12:35:34.23");
	 ss2 >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
	 auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
	 */

	//std::cout << duration_ms << std::endl;
	//std::cout <<"snext is "<< snext << std::endl;

	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast < std::chrono::milliseconds> (duration).count();
	//std::cout << "now in millisec is  " << millis << std::endl;
	return millis;
	//std::cout <<"difference is   "<< duration_ms-millis<< std::endl;

	//  std::stringstream ss;
	// ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	//return ss.str();

}
