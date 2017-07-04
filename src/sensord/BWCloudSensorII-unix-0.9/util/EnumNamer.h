/*
    Copyright (c) 2007,2008 MyTelescope.com
    All rights reserved.

    This file is part of the BWCloudSensorII-unix package.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this package; if not, write to the
    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA  02111-1307  USA.
 */
#ifndef ENUM_NAMER_H
# define ENUM_NAMER_H

# include <utility>
# include <string>

# define ENUM_NAMER_ENTRY_STR(value, str)	{ value, str }
# define ENUM_NAMER_BIT_ENTRY(ident)		{ (1 << (ident)), #ident }
# define ENUM_NAMER_ENTRY(ident)		{ ident, #ident }

/*
 * Used to make putting names on enum values easier.
 * Usage:
    const char *getName(SomeEnum val)
    {
	const static EnumNamer::entry_t names[] = {
		ENUM_NAMER_ENTRY(SS__UNINITIALIZED),
		ENUM_NAMER_ENTRY(SS_INIT),
		ENUM_NAMER_ENTRY(SS_FOO),
	    };
	static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
	return namer.getName(val);
    }
 * Works fastest if names[] values start at 0, are contigeous and are sorted.
 */
class EnumNamer
{
  public:
    typedef unsigned int enum_t;
    typedef struct {
	enum_t value;
	const char *name;
    } entry_t;
    typedef unsigned int size_t;

    EnumNamer(const entry_t *entries, size_t nentries)
	: entries(entries), nentries(nentries), bufIndex(0)
    {
    }
    const char *getName(enum_t val) const;
    /* Assume each value in entries[] is a single bit, and return
     * a comma separated list of the names that have bits set in val.
     */
    std::string getBitSetNames(enum_t val) const;
    /* Assume each value in entries[] is sequential number (1,2,3,..)
     * and that "val" is a mask of the indicated bits (1<<1, 1<<2, 1,<3, ...).
     * Return a comma separated list of the names that have bits set in val.
     */
    std::string getBitVectorNames(unsigned int val) const;

    template <class E>
    bool getValue(const char *name, E &val) const {
	enum_t v;
	bool rval = getValue_(name, v);
	val = E(v);
	return rval;
    }

  private:
    bool getValue_(const char *name, enum_t &val) const;

  private:
    const entry_t *entries;
    size_t nentries;
    enum { NBUFS = 4 };	/* allow multiple calls before buffers get re-used. */
    mutable char bufs[NBUFS][17 + (sizeof(enum_t) * 8 + 2) / 3];
    mutable int bufIndex;
};

#endif /* ENUM_NAMER_H */
