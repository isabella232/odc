/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ODADatabase.h
// Baudouin Raoult - ECMWF Oct 04

#ifndef ODADatabase_H
#define ODADatabase_H

class PathName;

#include "odblib/SQLDatabase.h"

namespace odb {
namespace sql {

class ODADatabase : public SQLDatabase {
public:
	ODADatabase(const PathName&,const string&);
	~ODADatabase(); // Change to virtual if base class

private:
// No copy allowed
	ODADatabase(const ODADatabase&);
	ODADatabase& operator=(const ODADatabase&);

// -- Methods
	// None

// -- Overridden methods
	// From SQLDatabase

	virtual void open();
	virtual void close();
	virtual SQLTable* table(const string&);
	virtual SQLTable* openDataHandle(DataHandle&, DataFormat = ODA);
	virtual SQLTable* openDataStream(istream&, DataFormat = CSV); 

// -- Friends
	//friend ostream& operator<<(ostream& s,const ODADatabase& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
