/* 
 * Image manipulation program.
 * Copyright (C) 2006-2009 Petr Kubanek <petr@kubanek.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <config.h>
#include <strings.h>

#ifdef HAVE_PGSQL
#include "../../lib/rts2fits/rts2appdbimage.h"
#else
#include "../../lib/rts2fits/appimage.h"
#endif							 /* HAVE_PGSQL */
#include "rts2config.h"
#include "rts2format.h"

#include <iostream>
#include <iomanip>

#include <list>

#ifdef HAVE_LIBJPEG
#include <Magick++.h>
#endif // HAVE_LIBJPEG

#define IMAGEOP_NOOP      0x0000
#define IMAGEOP_ADDDATE   0x0001
#ifdef HAVE_PGSQL
#define IMAGEOP_INSERT    0x0002
#endif							 /* HAVE_PGSQL */
#define IMAGEOP_TEST      0x0004
#define IMAGEOP_PRINT     0x0008
#define IMAGEOP_FPRINT    0x0010
#define IMAGEOP_COPY      0x0020
#define IMAGEOP_SYMLINK	  0x0040
#define IMAGEOP_MOVE      0x0080
#define IMAGEOP_EVAL      0x0100
#define IMAGEOP_CREATEWCS 0x0200
#define IMAGEOP_ADDHELIO  0x0400
#define IMAGEOP_MODEL     0x0800
#define IMAGEOP_JPEG      0x1000
#define IMAGEOP_STAT      0x2000
#define IMAGEOP_DISTANCE  0x4000

#define OPT_ADDDATE   OPT_LOCAL + 5
#define OPT_ADDHELIO  OPT_LOCAL + 6
#define OPT_OBSID     OPT_LOCAL + 7
#define OPT_IMGID     OPT_LOCAL + 8
#define OPT_CAMNAME   OPT_LOCAL + 9
#define OPT_MOUNTNAME OPT_LOCAL + 10
#define OPT_LABEL     OPT_LOCAL + 11
#define OPT_ZOOM      OPT_LOCAL + 12
#define OPT_ERR_RA    OPT_LOCAL + 13
#define OPT_ERR_DEC   OPT_LOCAL + 14
#define OPT_ERR       OPT_LOCAL + 15

namespace rts2image
{

#ifdef HAVE_PGSQL
class AppImage:public Rts2AppDbImage
#else
class AppImage:public rts2image::AppImageCore
#endif							 /* HAVE_PGSQL */
{
	public:
		AppImage (int in_argc, char **in_argv, bool in_readOnly);
		virtual ~AppImage ();
	protected:
		virtual int processOption (int in_opt);
#ifdef HAVE_LIBJPEG
		virtual int init ();
#endif
#ifdef HAVE_PGSQL
		virtual bool doInitDB ();

		virtual int processImage (rts2image::ImageDb * image);
#else
		virtual int processImage (rts2image::Image * image);
#endif						 /* HAVE_PGSQL */

		virtual void usage ();
	private:
		int operation;

		void printOffset (double x, double y, rts2image::Image * image);

		int addDate (rts2image::Image * image);
#ifdef HAVE_PGSQL
		int insert (rts2image::ImageDb * image);
#endif
		void testImage (rts2image::Image * image);
		void pointDistance (rts2image::Image * image);
		void testEval (rts2image::Image * image);
		void createWCS (rts2image::Image * image);
		void printModel (rts2image::Image * image);
		void printStat (rts2image::Image * image);

		double off_x, off_y;
		double d_x1, d_y1, d_x2, d_y2;

		const char* print_expr;
		const char* copy_expr;
		const char* distance_expr;
		const char* link_expr;
		const char* move_expr;
#ifdef HAVE_LIBJPEG
		const char* jpeg_expr;
		const char* label;
		double zoom;
#endif

		int obsid;
		int imgid;

		const char *cameraName;
		const char *mountName;

		double err_ra;
		double err_dec;
		double err;
};

}

using namespace rts2image;

void AppImage::printOffset (double x, double y, Image * image)
{
	double sep;
	double x_out;
	double y_out;
	double ra, dec;

	image->getOffset (x, y, x_out, y_out, sep);
	image->getRaDec (x, y, ra, dec);

	std::ios_base::fmtflags old_settings =
		std::cout.setf (std::ios_base::fixed, std::ios_base::floatfield);

	int old_p = std::cout.precision (2);

	std::cout << "Image::getOffset ("
		<< std::setw (10) << x << ", "
		<< std::setw (10) << y << "): "
		<< "RA " << LibnovaRa (ra) << " "
		<< LibnovaDegArcMin (x_out)
		<< " DEC " << LibnovaDec (dec) << " "
		<< LibnovaDegArcMin (y_out) << " ("
		<< LibnovaDegArcMin (sep) << ")" << std::endl;

	std::cout.precision (old_p);
	std::cout.setf (old_settings);
}

int AppImage::addDate (Image * image)
{
	int ret;
	time_t t;
	std::cout << "Adding date " << image->getFileName () << "..";
	t = image->getExposureSec ();
	image->setValue ("DATE-OBS", &t, image->getExposureUsec (),"date of observation");
	ret = image->saveImage ();
	std::cout << (ret ? "failed" : "OK") << std::endl;
	return ret;
}

#ifdef HAVE_PGSQL
int AppImage::insert (ImageDb * image)
{
  	if (obsid > 0)
	  	image->setObsId (obsid);
	if (imgid > 0)
	  	image->setImgId (imgid);
	if (cameraName)
	  	image->setCameraName (cameraName);
	if (mountName)
	  	image->setMountName (mountName);
	image->setErrors (err_ra, err_dec, err);	

	return image->saveImage ();
}
#endif							 /* HAVE_PGSQL */

void AppImage::testImage (Image * image)
{
	double ra, dec; //, x, y;
	std::cout
		<< image << std::endl
		<< "average " << image->getAverage () << std::endl
		<< "stdev " << image->getStdDev () << std::endl
		<< "bg_stdev " << image->getBgStdDev () << std::endl
		<< "Image XoA and Yoa: [" << image->getXoA ()
		<< ":" << image->getYoA () << "]" << std::endl
		<< "[XoA:YoA] RA: " << image->getCenterRa ()
		<< " DEC: " << image->getCenterDec () << std::endl
		<< "FLIP: " << image->getFlip () << std::endl
		<< image->getRaDec (image->getXoA (), image->getYoA (), ra, dec)
		<< "ROTANG: " << ln_rad_to_deg (image->getRotang ())
		<< " (deg) XPLATE: " << image->getXPlate ()
		<< " YPLATE: " << image->getYPlate () << std::endl
		<< "RA and DEC of [XoA:YoA]: " << ra << ", " << dec << std::endl
		<< image->getRaDec (0, 0, ra, dec) << "RA and DEC of [0:0]: " << ra << ", " << dec << std::endl
		<< image->getRaDec (image->getChannelWidth (0), 0, ra, dec) << "RA and DEC of [W:0]: " << ra << ", " << dec << std::endl
		<< image->getRaDec (0, image->getChannelHeight (0), ra, dec) << "RA and DEC of [0:H]: " << ra << ", " << dec << std::endl
		<< image->getRaDec (image->getChannelWidth (0), image->getChannelHeight (0), ra, dec) << "RA and DEC of [W:H]: " << ra << ", " << dec << std::endl
		// << "Image::getCenterRow " << image->getCenter (x, y, 3) << " " << x << ":" << y << std::endl
		<< "Expression %b/%t/%i/%c/%f '" << image->expandPath (std::string ("%b/%t/%i/%c/%f")) << '\'' << std::endl
		<< "Expression $DATE-OBS$/%b/%e/%E/%f/%F/%t/%i/%y/%m/%d/%D/%H/%M/%S/%s.fits '" << image->expandPath (std::string ("$DATE-OBS$/%b/%e/%E/%f/%F/%t/%i/%y/%m/%d/%D/%H/%M/%S/%s.fits")) << '\'' << std::endl
		<< "Target Type " << image->getTargetType () << std::endl;

	printOffset (image->getXoA () + 50, image->getYoA (), image);
	printOffset (image->getXoA (), image->getYoA () + 50, image);
	printOffset (image->getXoA () - 50, image->getYoA (), image);
	printOffset (image->getXoA (), image->getYoA () - 50, image);

	printOffset (152, 150, image);
}

void AppImage::pointDistance (Image * image)
{
	double ra, dec, sep;
	int ret;

	ret = image->getOffset (d_x1, d_y1, d_x2, d_y2, ra, dec, sep);
	if (ret)
		return;
	std::cout << LibnovaDegArcMin (ra) << " " << LibnovaDegArcMin (dec) << std::endl;
}

void AppImage::testEval (Image * image)
{
	float value, error;

	image->evalAF (&value, &error);

	std::cout << "value: " << value << " error: " << error << std::endl;
}

void AppImage::createWCS (Image * image)
{
	int ret = image->createWCS (off_x, off_y);

	if (ret)
		std::cerr << "create WCS returned with error " << ret << std::endl;
}

void AppImage::printModel (Image *image)
{
	try
	{
  		std::cout << image->getCoord ("OBJ") << " " << image->getCoord ("TAR")
			<< '\t' << LibnovaDegDist (image->getValue ("RA_ERR"))
			<< '\t' << LibnovaDegDist (image->getValue ("DEC_ERR"))
			<< '\t' << LibnovaDegDist (image->getValue ("POS_ERR"))
			<< '\t' << LibnovaDegDist (image->getValue ("CORR_RA"))
			<< '\t' << LibnovaDegDist (image->getValue ("CORR_DEC"))
			<< std::endl;
	}
	catch (KeyNotFound er)
	{
		logStream (MESSAGE_ERROR) << er << sendLog;
	}
}

void AppImage::printStat (Image *image)
{
	image->computeStatistics ();
	std::cout << image->getFileName ()
		<< " " << image->getAverage ()
		<< " " << image->getStdDev () << std::endl;
}

int AppImage::processOption (int in_opt)
{
	char *off_sep;
	switch (in_opt)
	{
		case 'p':
			if (print_expr)
				return -1;
			operation |= IMAGEOP_PRINT;
			print_expr = optarg;
			break;
		case 'P':
			if (print_expr)
				return -1;
			operation |= IMAGEOP_FPRINT;
			print_expr = optarg;
			break;
		case 'r':
			operation |= IMAGEOP_MODEL;
			break;
		case 's':
			operation |= IMAGEOP_STAT;
			break;
		case 'n':
			std::cout << pureNumbers;
			break;
		case 'c':
			if (copy_expr)
				return -1;
			operation |= IMAGEOP_COPY;
			copy_expr = optarg;
			break;
		case 'd':
			if (distance_expr)
				return -1;
			operation |= IMAGEOP_DISTANCE;
			distance_expr = optarg;
			if (sscanf (distance_expr, "%lf:%lf-%lf:%lf", &d_x1, &d_y1, &d_x2, &d_y2) != 4)
			{
				std::cerr << "cannot parse distancei argument: " << distance_expr << std::endl;
				return -1;
			}
			break;
		case OPT_ADDDATE:
			operation |= IMAGEOP_ADDDATE;
			readOnly = false;
			break;
		case OPT_ADDHELIO:
			operation |= IMAGEOP_ADDHELIO;
			readOnly = false;
			break;
#ifdef HAVE_PGSQL
		case 'i':
			operation |= IMAGEOP_INSERT;
			break;
#endif					 /* HAVE_PGSQL */
		case OPT_OBSID:
			obsid = atoi (optarg);
			break;
		case OPT_IMGID:
			imgid = atoi (optarg);
			break;
		case OPT_CAMNAME:
			cameraName = optarg;
			break;
		case OPT_MOUNTNAME:
			mountName = optarg;
			break;
		case OPT_ERR_RA:
			err_ra = atof (optarg);
			break;
		case OPT_ERR_DEC:
			err_dec = atof (optarg);
			break;
		case OPT_ERR:
			err = atof (optarg);
			break;	
		case 'm':
			if (move_expr)
				return -1;
			operation |= IMAGEOP_MOVE;
			move_expr = optarg;
			break;
		case 'l':
			if (link_expr)
				return -1;
			operation |= IMAGEOP_SYMLINK;
			link_expr = optarg;
			break;
		case 't':
			operation |= IMAGEOP_TEST;
			break;
		case 'e':
			operation |= IMAGEOP_EVAL;
			break;
		case 'w':
			operation |= IMAGEOP_CREATEWCS;
			readOnly = false;
			break;
		case 'o':
			off_sep = index (optarg, ':');
			if (off_sep)
			{
				*off_sep = '\0';
				off_sep++;
				off_x = atof (optarg);
				off_y = atof (off_sep);
			}
			else
			{
				off_x = atof (optarg);
				off_y = off_x;
			}
			break;
		#ifdef HAVE_LIBJPEG
		case 'j':
			if (jpeg_expr)
				return -1;
			operation |= IMAGEOP_JPEG;
			jpeg_expr = optarg;
			break;
		case OPT_LABEL:
			label = optarg;
			break;
		case OPT_ZOOM:
			zoom = atof (optarg);
			break;
		#endif /* HAVE_LIBJPEG */
		default:

		#ifdef HAVE_PGSQL
			return Rts2AppDbImage::processOption (in_opt);
		#else
			return rts2image::AppImageCore::processOption (in_opt);
		#endif /* HAVE_PGSQL */
	}
	return 0;
}

#ifdef HAVE_LIBJPEG
int AppImage::init ()
{
#ifdef HAVE_PGSQL
	int ret = Rts2AppDbImage::init ();
#else
	int ret = rts2image::AppImageCore::init ();
#endif /* HAVE_PGSQL */
	if (ret)
		return ret;
	Magick::InitializeMagick (".");
	return 0;
}
#endif /* HAVE_LIBJPEG */

#ifdef HAVE_PGSQL
bool AppImage::doInitDB ()
{
	return (operation & IMAGEOP_MOVE) || (operation & IMAGEOP_INSERT);
}
#endif

#ifdef HAVE_PGSQL
int AppImage::processImage (ImageDb * image)
#else
int AppImage::processImage (Image * image)
#endif							 /* HAVE_PGSQL */
{
	if (operation == IMAGEOP_NOOP)
		help ();
	if (operation & IMAGEOP_ADDDATE)
		addDate (image);
	if (operation & IMAGEOP_ADDHELIO)
	{
		struct ln_equ_posn tel;
		double JD = image->getExposureJD () + image->getExposureLength () / 2.0 / 86400;
		image->getCoordMount (tel);
		image->setValue ("JD_HELIO", JD + ln_get_heliocentric_time_diff (JD, &tel), "heliocentric JD");
	}
	#ifdef HAVE_PGSQL
	if (operation & IMAGEOP_INSERT)
		insert (image);
	#endif
	if (operation & IMAGEOP_TEST)
		testImage (image);
	if (operation & IMAGEOP_PRINT)
		std::cout << image->expandPath (print_expr) << std::endl;
	if (operation & IMAGEOP_FPRINT)
	  	std::cout << image->getFileName () << " " << image->expandPath (print_expr) << std::endl;
	if (operation & IMAGEOP_MODEL)
	  	printModel (image);
	if (operation & IMAGEOP_STAT)
	  	printStat (image);
	if (operation & IMAGEOP_COPY)
		image->copyImageExpand (copy_expr);
	if (operation & IMAGEOP_DISTANCE)
	  	pointDistance (image);
	if (operation & IMAGEOP_MOVE)
		image->renameImageExpand (move_expr);
	if (operation & IMAGEOP_SYMLINK)
	  	image->symlinkImageExpand (link_expr);
	if (operation & IMAGEOP_EVAL)
		testEval (image);
	if (operation & IMAGEOP_CREATEWCS)
		createWCS (image);
#ifdef HAVE_LIBJPEG
	if (operation & IMAGEOP_JPEG)
	  	image->writeAsJPEG (jpeg_expr, zoom, label);
#endif /* HAVE_LIBJPEG */
	return 0;
}

void AppImage::usage ()
{
	std::cout 
		<< "  rts2-image -w 123.fits                     .. write WCS to file 123, based on information stored by RTS2 in the file"	<< std::endl
		<< "  rts2-image -w -o 20.12:10.56 123.fits      .. same as above, but add X offset of 20.12 pixels and Y offset of 10.56 pixels to WCS" << std::endl
		<< "  rts2-image -P @DATE_OBS/@POS_ERR 123.fits  .. prints DATE_OBS and POS_ERR keywords" << std::endl
		<< "  rts2-image -d 10:15-20:25 123.fits         .. prints RA DEC distance between pixel (10,15) and (20,25)" << std::endl
		<< "  rts2-image --label '%H:%M' -j out.jpeg 123.fits  .. creates out.jpeg, add label consisting of exposure hour and minute" << std::endl;
}

AppImage::AppImage (int in_argc, char **in_argv, bool in_readOnly):
#ifdef HAVE_PGSQL
Rts2AppDbImage (in_argc, in_argv, in_readOnly)
#else
rts2image::AppImageCore (in_argc, in_argv, in_readOnly)
#endif
{
	operation = IMAGEOP_NOOP;

	off_x = 0;
	off_y = 0;

	print_expr = NULL;
	copy_expr = NULL;
	distance_expr = NULL;
	link_expr = NULL;
	move_expr = NULL;
#ifdef HAVE_LIBJPEG
	jpeg_expr = NULL;
	label = NULL;
	zoom = 1;
#endif

	obsid = -1;
	imgid = -1;

	cameraName = NULL;
	mountName = NULL;

	err_ra = err_dec = err = rts2_nan ("f");

	addOption ('p', NULL, 1, "print image expression");
	addOption ('P', NULL, 1, "print filename followed by expression");
	addOption ('r', NULL, 0, "print referencig status - usefull for modelling checks");
	addOption ('s', NULL, 0, "print image statistics - average, median, min & max values,...");
	addOption ('n', NULL, 0, "print numbers only - do not pretty print degrees,..");
	addOption ('c', NULL, 1, "copy image(s) to path expression given as argument");
	addOption ('d', NULL, 1, "print distance in RA and DEC degrees between two points, specified as X1:Y1-X2:Y2");
	addOption (OPT_ADDDATE, "add-date", 0, "add DATE-OBS to image header");
	addOption (OPT_ADDHELIO, "add-heliocentric", 0, "add JD_HELIO to image header (contains heliocentrict time)");
	addOption ('i', NULL, 0, "insert/update image(s) in the database");
	addOption (OPT_OBSID, "obsid", 1, "force observation ID for image operations");
	addOption (OPT_IMGID, "imgid", 1, "force image ID for image operations");
	addOption (OPT_CAMNAME, "camera", 1, "force camera name for image operations");
	addOption (OPT_MOUNTNAME, "telescope", 1, "force telescope name for image operations");
	addOption (OPT_ERR_RA, "ra-err", 1, "force image RA error");
	addOption (OPT_ERR_DEC, "dec-err", 1, "force image DEC error");
	addOption (OPT_ERR, "err", 1, "force image position error");
	addOption ('m', NULL, 1, "move image(s) to path expression given as argument");
	addOption ('l', NULL, 1, "soft link images(s) to path expression given as argument");
	addOption ('e', NULL, 0, "image evaluation for AF purpose");
	addOption ('t', NULL, 0, "test various image routines");
	addOption ('w', NULL, 0, "write WCS to FITS file, based on the RTS2 informations recorded in fits header");
	addOption ('o', NULL, 1, "X and Y offsets in pixels aplied to WCS information before WCS is written to the file. X and Y offsets must be separated by ':'");
#ifdef HAVE_LIBJPEG
	addOption ('j', NULL, 1, "export image(s) to JPEGs, specified by expansion string");
	addOption (OPT_LABEL, "label", 1, "label (expansion string) for image(s) JPEGs");
	addOption (OPT_ZOOM, "zoom", 1, "zoom the image before writing its label");
#endif /* HAVE_LIBJPEG */
}

AppImage::~AppImage ()
{
#ifdef HAVE_LIBJPEG
  	MagickLib::DestroyMagick ();
#endif /* HAVE_LIBJPEG */
}

int main (int argc, char **argv)
{
	AppImage app = AppImage (argc, argv, true);
	return app.run ();
}
