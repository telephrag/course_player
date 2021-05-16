
#pragma once
#include <vector>

#include "common_sqlite.hpp"

class Session
{
private:
    int id = -1;
    std::string login = "";
    std::string password = "";
    bool valid = false;
    sqlite3* db;

    void set_credentials(
        int         id,
        std::string login, 
        std::string password
        );

    static int login_callback(
        void*  used, 
        int    argc,
        char** argv, 
        char** azColName
        );

    static int validation_callback(
        void*  useless,
        int    argc,
        char** argv,
        char** azColName
        );
    
    
    static int value_retrieval_callback(
        void*  used,
        int    argc,
        char** argv,
        char** azColName
        );
    
    static int playlist_retrieval_callback(
        void*  used,
        int    argc,
        char** argv,
        char** azColName
        );

    bool permited(
        std::string table_name, 
        int         item_id
        );
    
    int snatch_song_internal(
        int song_id
        );

public:
    Session(
        sqlite3* db
        );

    bool validate();
    
    void login_func();

    void logout();

    void print();
    
    void add_song();
    
    void snatch_song();

    void del_song();
    
    void print_songs();
    
    void print_all_songs();
    
    void make_playlist();
    
    void del_playlist();
    
    void print_playlist();
    
    void print_playlists();
    
    void add_to_playlist();
    
    void del_from_playlist();
    
    void set_playlist();
    
    void del_user();
    
};
