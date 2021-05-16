
#pragma once

#include <iostream>
#include <string>
#include <map>

#include <sqlite3.h>

static std::string quote_sql(
    const std::string str
    )
{
    std::string quote = "'";
    std::string result = "'" + str + "'";
    return result;
}

static int default_callback(
    void*  useless,
    int    argc,
    char** argv,
    char** azColName
    )
{
    std::cout << "\n";
    
    for(int i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    std::cout << "\n";
    return 0;
}

static int status_callback(
    void*  status,
    int    argc,
    char** argv,
    char** azColName
    )
{
    bool* stat = static_cast<bool *>(status);
    *stat = true;
    return 0;
}
