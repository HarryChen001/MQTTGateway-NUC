#pragma once
#ifndef _SOFT_SQLITE3PP_H_
#define _SOFT_SQLITE3PP_H_

#include "sqlite3pp/sqlite3pp.h"

class MySqlite
{
public:
	MySqlite(char* dbname);
	~MySqlite();
	int GetCountFromTable(char* tablename);
	int selectfrom(char* tablename, char* format, ...);
	int GetAllInfo();
private:
	sqlite3pp::database db;
};

#endif
