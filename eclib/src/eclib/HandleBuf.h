/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HandleBuf.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eclib_HandleBuf_h
#define eclib_HandleBuf_h

#include "eclib/DataHandle.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class HandleBuf : public streambuf  {
public:

// -- Contructors

	HandleBuf(DataHandle& handle);

// -- Destructor

	~HandleBuf();

private:

// No copy allowed

	HandleBuf(const HandleBuf&);
	HandleBuf& operator=(const HandleBuf&);

// -- Members

	char in_[1];
	char out_[80];
	DataHandle& handle_;

// -- Overridden methods

	// From streambuf

	virtual int overflow(int c);
	virtual int underflow();
	virtual int sync();
//	virtual int uflow();

};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif