/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/FunctionIN.h"
#include "odblib/FunctionEQ.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionIN::FunctionIN(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args),
  size_(args.size() - 1)
{}

FunctionIN::FunctionIN(const FunctionIN& other)
: FunctionExpression(other.name_, other.args_),
  size_(other.args_.size() - 1)
{}

FunctionIN::~FunctionIN() {}

SQLExpression* FunctionIN::clone() const { return new FunctionIN(*this); }

double FunctionIN::eval(bool& missing) const
{
	const SQLExpression& x = *args_[size_];
	for(size_t i = 0 ; i < size_ ; ++i)
		if (FunctionEQ::equal(x, *args_[i], missing))
			return true;
	return false;
}

static FunctionMaker<FunctionIN> make_IN("in");

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb 
