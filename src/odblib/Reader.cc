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
/// \file ODA.cc
///
/// @author Piotr Kuchta, Feb 2009


#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <climits>
#include <cfloat>

using namespace std;

#include "eclib/Exceptions.h"
#include "eclib/PathName.h"
#include "eclib/DataHandle.h"
#include "eclib/FileHandle.h"

#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/FixedSizeWriterIterator.h"
#include "odblib/HashTable.h"
#include "odblib/Header.h"
#include "odblib/IteratorProxy.h"
#include "odblib/MemoryBlock.h"
#include "odblib/MetaData.h"
#include "odblib/Reader.h"
#include "odblib/ReaderIterator.h"
#include "odblib/RowsIterator.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLInteractiveSession.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLTable.h"
#include "odblib/SQLType.h"
#include "odblib/SelectIterator.h"
#include "odblib/Writer.h"
#include "odblib/WriterBufferingIterator.h"

using namespace std;

namespace odb {
#define MEGA(x) (x*1024*1024)

Reader::Reader(DataHandle &dh)
: dataHandle_(&dh),
  deleteDataHandle_(false)
{}

Reader::Reader()
: dataHandle_(0),
  deleteDataHandle_(true),
  path_("")
{}

Reader::Reader(const string& path)
: dataHandle_(new FileHandle(path)),
  deleteDataHandle_(true),
  path_(path)
{
        dataHandle_->openForRead();
}

Reader::~Reader()
{
        if (dataHandle_ && deleteDataHandle_)
        {
                dataHandle_->close();
                delete dataHandle_;
        }
}

ReaderIterator* Reader::createReadIterator(const PathName& pathName)
{
        return new ReaderIterator(*this, pathName);
}

ReaderIterator* Reader::createReadIterator()
{
	return createReadIterator(path_);
}

Reader::iterator Reader::begin()
{
        ReaderIterator * it = new ReaderIterator(*this);
        it->next();
        return iterator(it);
}

const Reader::iterator Reader::end() { return iterator(0); }

} // namespace odb
