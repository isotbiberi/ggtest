/* 
 * Telescope control daemon.
 * Copyright (C) 2005-2009 Petr Kubanek <petr@kubanek.net>
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

#include "clicupola.h"
#include "rts2command.h"

#include <math.h>
#include <libnova/libnova.h>

using namespace rts2teld;

ClientCupola::ClientCupola (Rts2Conn * conn):rts2core::Rts2DevClientCupola (conn)
{
}

ClientCupola::~ClientCupola (void)
{
	getMaster ()->postEvent (new Rts2Event (EVENT_CUP_ENDED, this));
}

void ClientCupola::syncEnded ()
{
	getMaster ()->postEvent (new Rts2Event (EVENT_CUP_SYNCED));
	rts2core::Rts2DevClientCupola::syncEnded ();
}

void ClientCupola::syncFailed (int status)
{
	getMaster ()->postEvent (new Rts2Event (EVENT_CUP_SYNCED));
	rts2core::Rts2DevClientCupola::syncFailed (status);
}

void ClientCupola::notMoveFailed (int status)
{
	getMaster ()->postEvent (new Rts2Event (EVENT_CUP_NOT_MOVE));
	rts2core::Rts2DevClientCupola::notMoveFailed (status);
}

void ClientCupola::postEvent (Rts2Event * event)
{
	struct ln_equ_posn *dome_position;
	switch (event->getType ())
	{
		case EVENT_CUP_START_SYNC:
			dome_position = (struct ln_equ_posn *) event->getArg ();
			connection->queCommand (new rts2core::Rts2CommandCupolaMove (this, dome_position->ra, dome_position->dec));
			dome_position->ra = rts2_nan ("f");
			break;
		case EVENT_CUP_NOT_MOVE:
			connection->queCommand (new rts2core::Rts2CommandCupolaNotMove (this));
			break;
	}
	rts2core::Rts2DevClientCupola::postEvent (event);
}