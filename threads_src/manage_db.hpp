
#pragma once

#include <map>

#include "Session.hpp"
#include "logged.hpp"

enum CommandsAuth
{
    signup, 
    login, 
    quit
};

std::map<std::string, CommandsAuth> commandMapAuth = {
        { "signup", CommandsAuth::signup },
        { "login",  CommandsAuth::login  },
        { "quit",   CommandsAuth::quit   }
    };
    

void signup_func(
    sqlite3* db
    )
{
    std::string login;
    std::cout << "Input new login: "; 
    std::cin >> login;
    
    std::string password;
    std::cout << "Make up some password: "; 
    std::cin >> password;

    std::string sql = 
        "INSERT INTO User (Login, Password) VALUES (" + quote_sql(login) + "," + quote_sql(password) + ");";

    char* zErrMsg; 
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    }
    else 
    {
        std::cout << "Entry in table created successfuly.\n";
    }
}


int manage_playlists()
{
    sqlite3* db;
    int rc = sqlite3_open("test.db", &db);

    if (rc)
    {
        std::cout << "Error. Can't open database.\n";
        return 0;
    }
    else
        std::cout << "Database was successfuly opened.\n";

    Session session = Session(db);
    for (;;)
    {
        std::string command = "";
        std::cin >> command;

        try {
            commandMapAuth.at(command);
        }
        catch (std::out_of_range)
        {
            std::cout << "No such command.\n";
            continue;
        }

        switch(commandMapAuth[command])
        {
            case signup:
                signup_func(db);
                break;
            
            case login:
            {
                session.login_func();
                logged(session);
                break;
            }

            case quit:
                return 0;

            default:
                break;
        }
    }

    return 0;
}
