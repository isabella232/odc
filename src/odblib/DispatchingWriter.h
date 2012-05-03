/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file DispatchingWriter.h
///
/// @author Piotr Kuchta, June 2009

#ifndef DispatchingWriter_H
#define DispatchingWriter_H

#include "odblib/WriterDispatchingIterator.h"
#include "odblib/WriterBufferingIterator.h"

class PathName;

namespace odb {

class DispatchingWriter
{
public:
	typedef WriterDispatchingIterator<WriterBufferingIterator, DispatchingWriter> iterator_class;
	typedef IteratorProxy<iterator_class, DispatchingWriter>  iterator;

	DispatchingWriter(const string &outputFileTemplate, int maxOpenFiles = 0);
	virtual ~DispatchingWriter();

	const string outputFileTemplate() { return outputFileTemplate_; }

	iterator begin();

protected:
	iterator_class* writer();

private:
// No copy allowed
    DispatchingWriter(const DispatchingWriter&);
    DispatchingWriter& operator=(const DispatchingWriter&);

	const string outputFileTemplate_;
	int maxOpenFiles_; 
};

} // namespace odb 

#endif
