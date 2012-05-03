/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/StrStream.h"
#include "eclib/Tokenizer.h"

#include "odblib/SQLAST.h"
#include "odblib/SQLBitColumn.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLColumn.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLTable.h"
#include "odblib/SQLType.h"
#include "odblib/SchemaAnalyzer.h"

namespace odb {
namespace sql {

SQLTable::SQLTable(SQLDatabase& owner,const PathName& path,const string& name):
	path_(path),
	name_(name),
	owner_(owner),
	master_(0)
{
	//Log::info() << "SQLTable[path=" << path_ << ",name=" << name << ",no_rows=" << no_rows_ << ",index=" << index_ << "]" << endl;
}

SQLTable::~SQLTable() { clearColumns(); }

void SQLTable::clearColumns()
{
	// Don't loop on names, as we have all bitmap entries pointing to the same
	for(map<int,SQLColumn*>::iterator m = columnsByIndex_.begin(); m != columnsByIndex_.end(); ++m)
	{
		SQLColumn* p = (*m).second;
		delete p;
	}
	columnsByName_.clear();
	columnsByIndex_.clear();
}

vector<string> SQLTable::columnNames() const
{
	vector<string> results;
	for(map<int,SQLColumn*>::const_iterator j = columnsByIndex_.begin(); j != columnsByIndex_.end(); ++j)
		results.push_back((*j).second->name());
	return results;
}

FieldNames SQLTable::bitColumnNames(const string& name) const
{
	typedef map<string, FieldNames>::const_iterator I;
	I i = bitColumnNames_.find(name);
	if (i != bitColumnNames_.end())
		return (*i).second;

	ASSERT("name not found" && name.find("@") == string::npos);


	string columnName;
	FieldNames fieldNames;
	size_t counter = 0;
	for (I i = bitColumnNames_.begin(); i != bitColumnNames_.end(); ++i)
	{
		if (i->first.find(name + "@") == 0)
		{
			columnName = i->first;
			fieldNames = i->second;
			++counter;
		}
	}
	if (counter == 0) throw UserError(string("Column '") + name + "' not found.");
	if (counter != 1) throw UserError(string("Ambiguous column name: '") + name + "'");

	return fieldNames;
}


//void SQLTable::addColumn(const string& name, int index, const type::SQLType& type, const FieldNames& bitmap)
void SQLTable::addColumn(const string& name, int index, const type::SQLType& type, bool hasMissingValue, double missingValue, bool
isBitfield, const BitfieldDef& bitfieldDef)
{
	const FieldNames& bitmap = bitfieldDef.first;
	SQLColumn *col = createSQLColumn(type, name, index, hasMissingValue, missingValue, isBitfield, bitfieldDef);

	columnsByName_[name]   = col;
	columnsByIndex_[index] = col;

	bitColumnNames_[name] = bitmap;

	vector<string> tokens;
	Tokenizer("@")(name, tokens);

	ASSERT(tokens.size() == 1 || tokens.size() == 2);

	// TODO: clean up, probably no need to do this parsing as we have the whole bitfieldDef now
	string tableName = (tokens.size() == 2) ? tokens[1] : "";
	string columnName = tokens[0];

	for(FieldNames::const_iterator j = bitmap.begin(); j != bitmap.end(); ++j)
	{
		string fieldName = *j;
		string n = columnName + "." + fieldName + "@" + tableName;
		columnsByName_[n] = col;

		//Log::info() << "SQLTable::addColumn: columnsByName_[" << n << "] = " << *col << endl;
	}
}

void SQLTable::addColumn(SQLColumn *col, const string& name, int index)
{
	columnsByName_[name]   = col;	
	columnsByIndex_[index] = col;	
}

//bool SQLTable::hasColumn(const string& name, string* fullName, bool *hasMissingValue, double *missingValue, BitfieldDef* bitfieldDef)
bool SQLTable::hasColumn(const string& name, string* fullName)
{
	map<string,SQLColumn*>::iterator j = columnsByName_.find(name);
	return j != columnsByName_.end();
}

unsigned long long SQLTable::noRows() const
{
	map<string,SQLColumn*>::const_iterator j = columnsByName_.begin();
	if(j != columnsByName_.end())
		return(*j).second->noRows();
	return 0;
}


SQLColumn* SQLTable::column(const string& name)
{
	map<string,SQLColumn*>::iterator j = columnsByName_.find(name);
	if(j != columnsByName_.end())
		return (*j).second;

	vector<string> v;
	Tokenizer(".")(name, v);

	if(v.size() > 1)
	{
		SQLColumn* col = column(v[0]);
		columnsByName_[name] = new SQLBitColumn(*col,v[1]);
		return columnsByName_[name];
	}

	throw UserError("Column not found",name);

}

SQLPool* SQLTable::pool(int index)
{
	map<int,SQLPool*>::iterator j = pools_.find(index);
	if(j == pools_.end())
		return 0;
	else
		return (*j).second;
}

void SQLTable::addLinkFrom(const SQLTable* from)
{
	linksFrom_.insert(from);
}

bool SQLTable::hasLinkFrom(const SQLTable& from) const
{
	return linksFrom_.find(&from) != linksFrom_.end();
}

void SQLTable::addLinkTo(const SQLTable* to)
{
	linksTo_.insert(to);
}

bool SQLTable::hasLinkTo(const SQLTable& to) const
{
	return linksTo_.find(&to) != linksTo_.end();
}

bool SQLTable::isParentOf(const SQLTable& other) const
{
	if(hasLinkTo(other))
		return true;

	for(set<const SQLTable*>::const_iterator j = linksTo_.begin();
		j != linksTo_.end(); ++j)
			if((*j)->isParentOf(other))
				return true;

	return false;
}

SQLTable* SQLTable::master() const      
{ 
	return master_? master_ : const_cast<SQLTable*>(this); 
}

void SQLTable::master(SQLTable* master) 
{ 
//	cout << "MASTER of " << name() << " " << master->name() << endl;
	master_ = master;            
}

string SQLTable::fullName() const
{
	return owner_.name() + "." + name_;
}

bool SQLTable::sameAs(const SQLTable& other) const
{
	return owner_.sameAs(other.owner_) && (name_ == other.name_);
}

bool SQLTable::sameDatabase(const SQLTable& other) const
{
	return &owner_ == &other.owner_;
}

void SQLTable::print(ostream& s) const
{
	s << "CREATE TABLE " << fullName() << " AS (" << endl;
	for(map<int,SQLColumn*>::const_iterator j = columnsByIndex_.begin(); j != columnsByIndex_.end(); ++j)
	{
		SQLColumn *c = j->second;
		s << "	" << c->name() << " " << c->type() << "," << endl;
	}
	s << ")" << endl;
}

} // namespace sql 
} // namespace odb 

