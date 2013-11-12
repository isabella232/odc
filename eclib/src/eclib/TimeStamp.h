/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TimeStamp.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eclib_TimeStamp_h
#define eclib_TimeStamp_h

#include "eclib/machine.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class TimeStamp {
public:

// -- Contructors

	TimeStamp(const string& = defaultFormat_);
	TimeStamp(time_t,const string& = defaultFormat_);

// -- Operators

	operator string() const;

	friend ostream& operator<<(ostream&,const TimeStamp&);

private:

// There is no private copy constructor as this will confuse g++ 4.x.x

// -- Members

	time_t        time_;
	const string& format_;

// -- Class members

	static const char* defaultFormat_;
};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif