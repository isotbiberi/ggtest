/* 
 * Basic sensor class.
 * Copyright (C) 2007-2008 Petr Kubanek <petr@kubanek.net>
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

#include "sensord.h"

Rts2DevSensor::Rts2DevSensor (int argc, char **argv):
Rts2Device (argc, argv, DEVICE_TYPE_SENSOR, "S1")
{
	setIdleInfoInterval (60);
}


Rts2DevSensor::~Rts2DevSensor (void)
{
}


int
SensorWeather::idle ()
{
	// switch weather state back to good..
	if (isGoodWeather () == true)
		setWeatherState (true);
	return Rts2DevSensor::idle ();
}

bool
SensorWeather::isGoodWeather ()
{
	if (getNextGoodWeather () >= getNow ())
		return false;
	return true;
}


SensorWeather::SensorWeather (int argc, char **argv, int _timeout)
:Rts2DevSensor (argc, argv)
{
	createValue (nextGoodWeather, "next_good_weather", "date and time of next good weather");
	setWeatherTimeout (_timeout);
}

void
SensorWeather::setWeatherTimeout (time_t wait_time)
{
	setWeatherState (false);

	time_t next;
	time (&next);
	next += wait_time;
	if (next > nextGoodWeather->getValueInteger ())
		nextGoodWeather->setValueInteger (next);
}
