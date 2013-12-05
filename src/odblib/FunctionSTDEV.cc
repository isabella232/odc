/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <math.h>

#include "odblib/FunctionSTDEV.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionSTDEV::FunctionSTDEV(const std::string& name,const expression::Expressions& args)
: FunctionVAR(name, args)
{}

FunctionSTDEV::FunctionSTDEV(const FunctionSTDEV& other)
: FunctionVAR(static_cast<const FunctionVAR&>(other))
{}

FunctionSTDEV::~FunctionSTDEV() {}

SQLExpression* FunctionSTDEV::clone() const { return new FunctionSTDEV(*this); }

const type::SQLType* FunctionSTDEV::type() const { return &type::SQLType::lookup("double"); }

double FunctionSTDEV::eval(bool& missing) const
{
	double x = FunctionVAR::eval(missing);
	if (x < 0) x = 0;
	return sqrt(x);
}

} // namespace function
} // namespace expression
} // namespace sql 
} // namespace odb

