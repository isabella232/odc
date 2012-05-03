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
/// \file MetaDataReaderIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include <arpa/inet.h>

#include "odblib/oda.h"
#include "odblib/Codec.h"
#include "odblib/DataStream.h"
#include "odblib/Header.h"
#include "odblib/MetaDataReader.h"
#include "odblib/MetaDataReaderIterator.h"

namespace odb {

MetaDataReaderIterator::MetaDataReaderIterator(Owner &owner, bool skipData)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  codecs_(0),
  nrows_(0),
  f(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  headerCounter_(0),
  skipData_(skipData),
  encodedData_(0),
  sizeOfEncodedData_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
	f = owner.dataHandle();
	ASSERT(f);
}

MetaDataReaderIterator::MetaDataReaderIterator(Owner &owner, const PathName& pathName, bool skipData)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  codecs_(0),
  nrows_(0),
  f(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  headerCounter_(0),
  skipData_(skipData),
  encodedData_(0),
  sizeOfEncodedData_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
    f = new FileHandle( pathName );
	ASSERT(f);
	ownsF_ = true;
	f->openForRead();
}


void MetaDataReaderIterator::loadHeaderAndBufferData()
{
	Header<MetaDataReaderIterator> header(*this);
	header.load();
	byteOrder_ = header.byteOrder();
	++headerCounter_;

	initRowBuffer();

	size_t dataSize = header.dataSize();
	if (dataSize && !skip(dataSize))
		ASSERT(0 && "Could not read the amount of data indicated by file's header");
	blockEndOffset_ = f->position();
}

MetaDataReaderIterator::~MetaDataReaderIterator ()
{
	Log::debug() << "MetaDataReaderIterator::~MetaDataReaderIterator: headers read: " << headerCounter_ << " rows:" << nrows_ << endl;

	close();
	delete [] lastValues_;
	delete [] codecs_;
}


bool MetaDataReaderIterator::operator!=(const MetaDataReaderIterator& other)
{
	ASSERT(&other == 0);
	return noMore_;
}

void MetaDataReaderIterator::initRowBuffer()
{
	size_t nCols = columns().size();

	delete [] lastValues_;
	lastValues_ = new double [nCols];

	delete [] codecs_;
	codecs_ = new odb::codec::Codec* [nCols];

	for(size_t i = 0; i < nCols; i++)
	{
		codecs_[i] = &columns()[i]->coder();
		lastValues_[i] = codecs_[i]->missingValue(); 
		codecs_[i]->dataHandle(&memDataHandle_);
	}
}

bool MetaDataReaderIterator::skip(size_t dataSize)
{
	Log::debug() << "MetaDataReaderIterator::skip: skipData_=" << skipData_ << endl;

	if (skipData_)
	{
		Log::debug() << "MetaDataReaderIterator::readBuffer: fseeko64(" << dataSize << ")" << endl;

        // TODO: check this code and refactor dur to changes in return condition 
        
        f->skip(dataSize); // will throw if fails
        
        return true;
	}

	Log::debug() << "MetaDataReaderIterator::skip: sizeOfEncodedData_=" << sizeOfEncodedData_ << endl;
	if (sizeOfEncodedData_ < dataSize)
	{
		Log::debug() << "MetaDataReaderIterator::skip: allocating " << dataSize << " bytes." << endl;
		delete [] encodedData_;
		encodedData_ = new char[dataSize];
	}
	
	size_t actualNumberOfBytes = 0;
	if ((actualNumberOfBytes = f->read(encodedData_, dataSize)) != dataSize)
	{
		Log::warning() << "MetaDataReaderIteratorReadingData::skip: expected " << dataSize 
						<< " could read only " << actualNumberOfBytes << endl;
		return false;
	}

	sizeOfEncodedData_ = dataSize;
	return true;
}

bool MetaDataReaderIterator::next()
{
	newDataset_ = false;
	if (noMore_)
		return false; 

	uint16_t c = 0;
	unsigned long bytesRead = 0;

	blockStartOffset_ = f->position();

	if ( (bytesRead = memDataHandle_.read(&c, 2)) == 0)
	{
		if ( (bytesRead = f->read(&c, 2)) == 0)
			return ! (noMore_ = true);
		ASSERT(bytesRead == 2);

		if (c == ODA_MAGIC_NUMBER) 
		{
			DataStream<SameByteOrder> ds(f);

			unsigned char cc;
			ds.readUChar(cc); ASSERT(cc == 'O');
			ds.readUChar(cc); ASSERT(cc == 'D');
			ds.readUChar(cc); ASSERT(cc == 'A');

			Header<MetaDataReaderIterator> header(*this);
			header.loadAfterMagic();
			byteOrder_ = header.byteOrder();
			++headerCounter_;
			nrows_ += columns().rowsNumber();
			initRowBuffer();

			newDataset_ = true;

			size_t dataSize = header.dataSize();
			if (! skip(dataSize)) {
				blockEndOffset_ = f->position();
				return ! (noMore_ = true);
			} else {
				blockEndOffset_ = f->position();
				return true;
			}
		}
	}
	c = ntohs(c);

	size_t nCols = columns().size();
	for(size_t i = c; i < nCols; i++)
		lastValues_[i] = codecs_[i]->decode();

	return nCols;
}

bool MetaDataReaderIterator::isNewDataset() { return newDataset_; }

const double* MetaDataReaderIterator::data() { return lastValues_; }

int MetaDataReaderIterator::close()
{
	if (ownsF_ && f)
	{
		f->close();
		delete f;
		f = 0;
	}

	return 0;
}


void MetaDataReaderIterator::property(string key, string value)
{
	properties_[key] = value;
}

string MetaDataReaderIterator::property(string key)
{
	return properties_[key];
}


ColumnType MetaDataReaderIterator::columnType(unsigned long index) { return columns_[index]->type(); }
const std::string& MetaDataReaderIterator::columnName(unsigned long index) const { return columns_[index]->name(); }
const std::string& MetaDataReaderIterator::codecName(unsigned long index) const { return columns_[index]->coder().name(); }
double MetaDataReaderIterator::columnMissingValue(unsigned long index) { return columns_[index]->missingValue(); }
const BitfieldDef& MetaDataReaderIterator::bitfieldDef(unsigned long index) { return columns_[index]->bitfieldDef(); }





} // namespace odb

