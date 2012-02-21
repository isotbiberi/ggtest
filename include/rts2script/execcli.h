/**
 * Executor client for telescope and camera.
 * Copyright (C) 2005-2007 Petr Kubanek <petr@kubanek.net>
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

#ifndef __RTS2_EXECCLI__
#define __RTS2_EXECCLI__

#include "devscript.h"

#include "event.h"
#include "rts2target.h"

#include "rts2fits/devcliimg.h"
#include "rts2script/script.h"

#define EVENT_SCRIPT_STARTED               RTS2_LOCAL_EVENT+50
#define EVENT_LAST_READOUT                 RTS2_LOCAL_EVENT+51
#define EVENT_SCRIPT_ENDED                 RTS2_LOCAL_EVENT+52
#define EVENT_MOVE_OK                      RTS2_LOCAL_EVENT+53
#define EVENT_MOVE_FAILED                  RTS2_LOCAL_EVENT+54
// to get correct scriptCount..
#define EVENT_KILL_ALL                     RTS2_LOCAL_EVENT+57

#define EVENT_SLEW_TO_TARGET               RTS2_LOCAL_EVENT+58

#define EVENT_TEL_SCRIPT_CHANGE            RTS2_LOCAL_EVENT+59
#define EVENT_TEL_SCRIPT_RESYNC            RTS2_LOCAL_EVENT+60

#define EVENT_ACQUSITION_END               RTS2_LOCAL_EVENT+61

#define EVENT_TEL_START_GUIDING            RTS2_LOCAL_EVENT+62
#define EVENT_TEL_STOP_GUIDING             RTS2_LOCAL_EVENT+63

#define EVENT_QUE_IMAGE                    RTS2_LOCAL_EVENT+64

#define EVENT_STOP_OBSERVATION             RTS2_LOCAL_EVENT+65

#define EVENT_CORRECTING_OK                RTS2_LOCAL_EVENT+66

#define EVENT_SCRIPT_RUNNING_QUESTION      RTS2_LOCAL_EVENT+67

// slew to target, and do not wait for clearing of the block state
#define EVENT_SLEW_TO_TARGET_NOW           RTS2_LOCAL_EVENT+68

namespace rts2script
{

class GuidingParams
{
	public:
		char dir;
		double dist;
		GuidingParams (char in_dir, double in_dist)
		{
			dir = in_dir;
			dist = in_dist;
		}
};

class DevClientCameraExec:public rts2image::DevClientCameraImage, public DevScript
{
	public:
		DevClientCameraExec (rts2core::Connection * in_connection, rts2core::ValueString * in_expandPath = NULL, std::string templateFile = std::string (""));
		virtual ~ DevClientCameraExec (void);
		virtual rts2image::Image *createImage (const struct timeval *expStart);
		virtual void postEvent (rts2core::Event * event);
		virtual void nextCommand ();
		void queImage (rts2image::Image * image, bool run_after = true);
		virtual rts2image::imageProceRes processImage (rts2image::Image * image);
		virtual void stateChanged (rts2core::ServerState * state);
		virtual void exposureFailed (int status);

		/**
		 * Called to report script progress.
		 */
		virtual void scriptProgress (double start, double end) {};

	protected:
		ScriptPtr exposureScript;
		virtual void unblockWait () { rts2image::DevClientCameraImage::unblockWait (); }
		virtual void unsetWait () { rts2image::DevClientCameraImage::unsetWait (); }

		virtual void clearWait () { rts2image::DevClientCameraImage::clearWait (); }

		virtual int isWaitMove () { return rts2image::DevClientCameraImage::isWaitMove (); }

		virtual void setWaitMove () { rts2image::DevClientCameraImage::setWaitMove (); }

		virtual void queCommandFromScript (rts2core::Command * com) { queCommand (com); }

		virtual int getFailedCount () { return rts2core::DevClient::getFailedCount (); }

		virtual void clearFailedCount () { rts2core::DevClient::clearFailedCount (); }

		virtual void idle ();

		virtual void exposureStarted ();
		virtual void exposureEnd ();
		virtual void readoutEnd ();

		int imgCount;

		virtual void startTarget ();

		virtual int getNextCommand ();

		virtual bool canEndScript ();
	private:
		rts2core::ValueString *expandPath;
		bool waitForExposure;
		bool waitMetaData;
};

class DevClientTelescopeExec:public rts2image::DevClientTelescopeImage
{
	private:
		Rts2Target * currentTarget;
		int blockMove;
		rts2core::CommandChange *cmdChng;

		struct ln_equ_posn fixedOffset;

		int syncTarget (bool now = false);
		void checkInterChange ();
	protected:
		virtual void moveEnd ();
	public:
		DevClientTelescopeExec (rts2core::Connection * in_connection);
		virtual void postEvent (rts2core::Event * event);
		virtual void moveFailed (int status);
};

}

#endif							 /*! __RTS2_EXECCLI__ */