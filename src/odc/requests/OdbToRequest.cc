/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/requests/OdbToRequest.h"

#include "eckit/types/Types.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"
#include "odc/GribCodes.h"
#include "odc/core/TablesReader.h"
#include "odc/ODAHandle.h"

#include <algorithm>
#include <string>
#include <memory>
#include <ostream>

using namespace eckit;
using namespace odc::core;

namespace odc {
namespace requests {

//----------------------------------------------------------------------------------------------------------------------

//OdbToRequest::OdbToRequest(bool mergeSimilar, bool constant) :
//    mergeSimilarBlocks_(mergeSimilar),
//    onlyConstantColumns_(constant) {}
//
//void OdbToRequest::parseConfig(const std::string& s) {
//
//    Tokenizer tokenizer(",= ':\n", false);
//
//    std::vector<std::string> tokens;
//    tokenizer(s, tokens);
//
//    ASSERT(tokens.size() > 1);
//    ASSERT(tokens.size() % 2 == 0);
//
//    for (size_t i = 0; i < tokens.size(); i += 2) {
//        Log::debug<LibOdc>() << "parseConfig: " << tokens[i] << "=" << tokens[i+1] << std::endl;
//        addColumn(tokens[i], tokens[i+1]);
//    }
//}
//
//void OdbToRequest::addColumn(const std::string& keyword, const std::string& columnName)
//{
//	keywords_.push_back(keyword);
//	columnNames_.push_back(columnName);
//}
//
//bool OdbToRequest::scanFile(const PathName& fileName) {
//    std::vector<ODBSpan> spans;
//    return scanFile(fileName, spans);
//}
//
//bool OdbToRequest::scanFile(const PathName& fileName, std::vector<ODBSpan>& spans)
//{
//    Log::debug<LibOdc> << "Iterating over headers of '" << fileName << "'" <<  std::endl;
//
//    core::TablesReader reader(fileName);
//    auto it = reader.begin();
//    auto end = reader.end();
//
//    const MetaData* currentMD = &it->columns();
//    size_t rowCount = currentMD->rowsNumber();
//
//	unsigned long int mds = 0;
//	for ( ; it != end; ++it)
//	{
//        const MetaData& md (it->columns());
//		++mds;
//
//        Offset startOffset = it->startPosition();
//        Offset endOffset = it->nextPosition();
//        Length blockSize = endOffset - startOffset;
//
//        if (!offsets.size() || !mergeSimilarBlocks_ || !currentMD->equalsIncludingConstants(md, columnNames_)) {
//
//            Log::debug<LibOdc>() << "OdbToRequest@" << this << "::scanFile: new handle for <" << startOffset << "," << endOffset << ">" << std::endl;
//
//            currentMD = &md;
//            ASSERT(currentMD->equalsIncludingConstants(md, columnNames_));
//            spans.emplace_back(ODBSpan {startOffset, blockSize});
//        } else {
//            spans.back().extend(blockSize);
//        }
//
//        if (!collectValues(*it, spans.back())) {
//            Log::debug<LibOdc>() << "OdbToRequest@" << this << "::scanFile: collectValues returned false" << std::endl;
//            return false;
//        }
//
//        rowCount += md.rowsNumber();
//	}
//
//	return true;
//}
//
//bool OdbToRequest::collectValues(const Table& tbl, ODAHandle& odaHandle) {
//
//    const MetaData& md (tbl.columns());
//
//    std::vector<size_t> nonConstantColumns;
//
//    for (size_t i = 0; i < columnNames_.size(); ++i) {
//
//        const std::string& keyword(keywords[i]);
//        const std::string& columnName(columnNames_[i]);
//
//        Column* column = md.columnByName(columnName);
//        if (!column) throw UserError("Column '" + columnName + "' not found and required to build request", Here());
//
//        if (!column->isConstant()) {
//            if (onlyConstantColumns_) throw UserError("Column '" + columnName + "' is not constant", Here());
//            nonConstantColumns.push_back(i);
//        }
//    }
//
//    if (!nonConstantColumns.empty()) {
//        ASSERT(!onlyConstantColumns_);
//
//        // Look at the actual data to determine what we need.
//
//
//
//    }
//
//
//
//    ASSERT(columnNames_.size());
//	std::vector<std::string> currentValues;
//
//    for (size_t i = 0; i < columnNames_.size(); ++i) {
//
//        const std::string& keyword(keywords[i]);
//        const std::string& columnName(columnNames_[i]);
//
//		Column* column = md.hasColumn(columnName) ? md.columnByName(columnName) : 0;
//		std::string v = ! column ? columnNotFound(columnName)
//				: ! column->isConstant() ? columnIsNotConstant(*column)
//                : column->type() == odc::api::STRING ? StringTool::double_as_string(column->min())
//                : column->type() == odc::api::INTEGER ? StringTool::int_as_double2string(column->min())
//                : Translator<double, std::string>()(column->min());
//		values_[i].insert(v);
//		currentValues.push_back(v);
//		double dv = !column ? odc::MDI::realMDI() : column->min();
//
//        Log::debug<LibOdc>() << "OdbToRequest@" << this << "::collectValues: columnName: " << columnName << ": " << v << "(" << dv << ")" << std::endl;
//
//		odaHandle.addValue(columnNames_[i], dv);
//		doubleValues_[keywords_[i]].insert(dv);
//	}
//
//	if (columnNames_.size())
//	{
//		if (valuesSeen_.find(currentValues) == valuesSeen_.end())
//            valuesSeen_[currentValues] = std::make_pair<Offset,Offset>(odaHandle.start(), odaHandle.end());
//		else {
//			std::pair<Offset,Offset> p = valuesSeen_[currentValues];
//			std::vector<std::string> vs = columnNames_;
//			for (size_t i = 0; i < vs.size(); ++i)
//				vs[i] += std::string("=") + currentValues[i];
//            std::stringstream s;
//			s << "Values " << vs << " found in blocks <" << p.first << "," << p.second << ">"
//				<< " and <" << odaHandle.start() << "," << odaHandle.end() << ">";
//			if (! duplicateCombination(s.str()))
//				return false;
//		}
//	}
//	return true;
//}
//
//std::string OdbToRequest::genRequest() const
//{
//    std::stringstream request;
//
//	for (size_t i = 0; i < columnNames_.size(); ++i)
//	{
//		const std::string& key = keywords_[i];
//        std::string k = StringTools::upper(key);
//		std::string valuesList;
//        const std::set<std::string>& vs = values_[i];
//        for (std::set<std::string>::const_iterator vi = vs.begin(); vi != vs.end(); ++vi)
//			valuesList += std::string(vi != vs.begin() ? "/" : "") + patchValue(k, *vi);
//		if (i > 0)
//			request << ",\n";
//		request << key << " = " << valuesList;
//	}
//
//    Log::debug<LibOdc> << "OdbToRequest@" << this << "::genRequest() => " << std::endl << request.str() << std::endl;
//
//	return request.str();
//}
//
//std::string OdbToRequest::patchValue(const std::string& k, const std::string& value) const
//{
//    std::ostream& L (Log::debug<LibOdc>());
//
//	std::string v = StringTools::trim(value);
//    L << "OdbToRequest@" << this << "::patchValue: v = '" << v  << "', key = " << k << std::endl;
//	if (k == "TIME")
//		v = StringTool::patchTimeForMars(v);
//	else if (k == "CLASS" || k == "TYPE" || k == "STREAM" || k == "OBSGROUP")
//	{
//        L << "OdbToRequest@" << this << "::genRequest: checking if '" << v << "' is numeric" << std::endl;
//		if (StringTool::check(v, isdigit))
//		{
//            L << "OdbToRequest@" << this << "::genRequest: replacing " << v << " with ";
//			v = GribCodes::alphanumeric(StringTools::lower(k), v);
//			L << v << std::endl;
//		}
//		v = StringTools::upper(v);
//	}
//	return v;
//}
//
//const std::set<std::string>& OdbToRequest::getValues(const std::string& keyword)
//{
//	for (size_t i = 0; i < keywords_.size(); ++i)
//		if (keywords_[i] == keyword)
//			return values_[i];
//    throw UserError(std::string("Keyword '") + keyword + "' not found");
//	// This is to keep the compiler happy:
//	return values_[-1];
//}
//
//std::map<std::string, std::vector<double> > OdbToRequest::getValues()
//{
//	std::map<std::string, std::vector<double> > r;
//
//	for (std::map<std::string, std::set<double> >::const_iterator it = doubleValues_.begin(); it != doubleValues_.end(); ++it)
//	{
//		const std::string &k = it->first;
//		const std::set<double>& values = it->second;
//
//		std::vector<double>& vs = r[k] = std::vector<double>();
//
//		for (std::set<double>::const_iterator vi = values.begin(); vi != values.end(); ++vi)
//			vs.push_back(*vi);
//	}
//	return r;
//}
//
//std::map<std::string, double> OdbToRequest::getUniqueValues()
//{
//	std::map<std::string, double> r;
//	for (size_t i (0); i < keywords_.size(); ++i)
//	{
//		std::string kw (keywords_[i]);
//		if ( doubleValues_[kw].size() != 1)
//		{
//            std::stringstream s;
//            std::set<double>& values (doubleValues_[kw]);
//			s << "Data should contain only one '" << kw << "' value, found: " << values.size() << ". ";
//            for (std::set<double>::iterator it (values.begin()); it != values.end(); ++it)
//                s << *it << ", ";
//            throw UserError(s.str());
//		}
//		r[kw] = *doubleValues_[kw].begin();
//	}
//	return r;
//}

//----------------------------------------------------------------------------------------------------------------------

} // namespace requests
} // namespace odc
