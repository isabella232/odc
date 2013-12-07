/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestTextSelect3.h
///
/// @author Piotr Kuchta, ECMWF, Oct 2010

#ifndef TestTextSelect3_H
#define TestTextSelect3_H

#include "odblib/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestTextSelect3 : public TestCase {
public:
	TestTextSelect3(int argc, char **argv);
	~TestTextSelect3();

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
	void selectStarOneColumn();
	void selectSumOneColumn();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif
