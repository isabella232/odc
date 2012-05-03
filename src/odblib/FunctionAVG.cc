/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/FunctionAVG.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionAVG::FunctionAVG(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  count_(0),
  value_(0)
{}

FunctionAVG::FunctionAVG(const FunctionAVG& other)
: FunctionExpression(other.name_, other.args_),
  count_(other.count_),
  value_(other.value_)
{}


SQLExpression* FunctionAVG::clone() const { return new FunctionAVG(*this); }


FunctionAVG::~FunctionAVG() {}

double FunctionAVG::eval(bool& missing) const
{
	if(!count_)
	{
		missing = true;
		return 0;
	}

	return value_ / count_;
}

void FunctionAVG::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = 0;
	count_ = 0;
}

void FunctionAVG::cleanup(SQLSelect& sql)
{
//cout << "Cleanup  FunctionAVG " << count_ << " " << value_ << endl;
	FunctionExpression::cleanup(sql);
	value_ = 0;
	count_ = 0;
}

void FunctionAVG::partialResult() 
{
	bool missing = false;
	double value = args_[0]->eval(missing);
	if(!missing)
	{
		value_ += value;
		count_++;
	}
//	else cout << "missing" << endl;
}

static FunctionMaker<FunctionAVG> make_AVG("avg",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

