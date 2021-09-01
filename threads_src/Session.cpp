
#include <vlc/vlc.h>
#include <vlcpp/vlc.hpp>

#include "Session.hpp"
#include "Playlist.hpp"

extern Playlist current_playlist;

void Session::set_credentials(
    int         id, 
    std::string login, 
    std::string password)
{
    this->id = id;
    this->login = login;
    this->password = password;
    this->valid = true;
}


int Session::login_callback(
    void*  used, 
    int    argc, 
    char** argv, 
    char** azColName)
{ 
    std::cout << "Arguments count: " << argc << "\n";
    int id = atoi(argv[0]);

    std::string login = std::string(argv[1]);

    std::string password = std::string(argv[2]);

    Session* session = static_cast<Session *>(used);
    session->set_credentials(id, login, password);
    std::cout << "Logged in successfuly.\n";
    session->print();

    return 0;
}


int Session::validation_callback(
    void*  used,
    int    argc,
    char** argv,
    char** azColName
    )
{
    Session* session = static_cast<Session *>(used);
    
    int id = atoi(argv[0]);

    std::string login = std::string(argv[1]);

    std::string password = std::string(argv[2]);

    if (
        id == session->id &&
        login.compare(session->login) == 0 &&
        password.compare(session->password) == 0
    )
        session->valid = true;

    return 0;
}


bool Session::validate()
{
    std::string sql =
        "SELECT UserID, Login, Password FROM User WHERE Login=" + 
        quote_sql(this->login) + " AND Password=" + quote_sql(this->password);

    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), validation_callback, this, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "Invalid SQL request during login validation.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        this->valid = false;
    }
    
    return this->valid;
}


bool Session::permited(
    std::string table_name, 
    int         item_id
    )
{
    std::string id_name = table_name + "ID";
    std::string sql =
        "SELECT * FROM " + table_name + " WHERE " + id_name + "=" + std::to_string(item_id) 
        + " AND UserID=" + std::to_string(this->id);

    bool status = false;
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), status_callback, &status, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    }
    
    return status;
}


int Session::value_retrieval_callback(
    void*  used,
    int    argc,
    char** argv,
    char** azColName
    )
{
    std::string* value = static_cast<std::string*>(used);
    *value = std::string(argv[0]);
    
    std::cout << "Retrieved value: " + *value + "\n";
    
    return 0;
}


Session::Session(
    sqlite3* db
    )
{
    this->db = db;
}

void Session::login_func()
{
    std::string login;
    std::cout << "Input existing login: ";
    std::cin >> login;

    std::string password;
    std::cout << "Input the password: "; 
    std::cin >> password;

    std::string sql =
        "SELECT UserID, Login, Password FROM User WHERE Login=" + 
        quote_sql(login) + " AND Password=" + quote_sql(password);

    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), login_callback, this, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    }

}


void Session::logout()
{
    this->id = -1;
    this->login = "";
    this->password = "";
    this->valid = false;
    std::cout << "Logging out...\n";
}


void Session::print()
{
    std::cout << 
        this->id       << " " << 
        this->login    << " " << 
        this->password << " " << 
        this->valid    << "\n";
}


void Session::add_song()
{
    std::string mrl = "";
    std::cout << "Input song's mrl in youtu.be type format: ";
    std::cin >> mrl;

    if (mrl.length() > 30)
    {
        std::cout << "MRL is to long.\n";
        std::cout << "No songs were added.\n";
        return;
    }

    std::string sql = 
        "INSERT INTO Song (UserID, MRL) VALUES (" 
        + std::to_string(this->id) + "," + quote_sql(mrl) + ");";

    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;

    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "Song addition failed.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    else 
        std::cout << "Song added successfuly: " << mrl << "\n"; 

    // TODO: Add song names to db as well.
}


int Session::snatch_song_internal(
    int song_id
    )
{
    if (!permited("Song", song_id)) // reusing function permited() to check wether song was added by this user
    {
        std::string copy_song_sql = 
            "INSERT INTO Song (UserID, MRL) VALUES (" + std::to_string(this->id) + "," 
            + "(SELECT MRL FROM Song WHERE (SongID=" + std::to_string(song_id) + ")) )";
        
        char* zErrMsg;
        int rc = sqlite3_exec(this->db, copy_song_sql.c_str(), 0, 0, &zErrMsg);
        
        if (rc != SQLITE_OK)
        {
            std::cout << "SQL error: " << zErrMsg << "\n";
            sqlite3_free(zErrMsg);
            return 0;
        }
        
        std::cout << "Copy of song was created with you as an owner.\n";
        
        std::string get_song_id = 
            "SELECT SongID FROM Song WHERE ( MRL=( SELECT MRL FROM Song WHERE SongID=" 
            + std::to_string(song_id) + ") AND UserId=" + std::to_string(this->id) + ")";
        std::string song_id_str = "";
        rc = sqlite3_exec(this->db, get_song_id.c_str(), value_retrieval_callback, &song_id_str, &zErrMsg);
        
        std::cout << "Snatched song ID: " + song_id_str + "\n";
        return std::stoi(song_id_str);
    }
    else
        std::cout << "You already own this song.\n";
    
    return song_id;
}


void Session::snatch_song()
{
    int song_id;
    std::cout << "Input the ID of a song you'd like to snatch: ";
    std::cin >> song_id;
    
    if(!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    
    snatch_song_internal(song_id);
    
    this->valid = false;
}


void Session::del_song()
{
    int song_id;
    std::cout << "Input ID of the song you'd like to delete: ";
    std::cin >> song_id;
    std::string sql = 
        "DELETE FROM Song WHERE SongID=" + std::to_string(song_id) + " AND UserID=" + std::to_string(this->id); 
        
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    if (!permited("Song", song_id))
    {
        std::cout << "You do not have a permission to delete this song.\n";
        return;
    }
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    else
        std::cout << "Song deleted successfuly.\n";
}



void Session::print_songs()
{
    std::string sql = 
        "SELECT SongID, MRL FROM Song WHERE UserID=" + std::to_string(this->id);
        
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), default_callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
}


void Session::print_all_songs()
{
    std::string sql = 
        "SELECT SongID, MRL FROM Song";
        
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), default_callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
}


void Session::make_playlist()
{
    std::string new_playlist_name;
    std::cout << "Input name of your playlist: "; std::cin >> new_playlist_name;
    
    std::string sql =
        "INSERT INTO Playlist (UserID, Name) VALUES (" 
        + std::to_string(this->id) + ", " + quote_sql(new_playlist_name) + ")";
    
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    else
        std::cout << "New playlist created successfuly.\n";
}


void Session::del_playlist()
{
    int playlist_id;
    std::cout << "Input ID of the playlist you'd like to delete: ";
    std::cin >> playlist_id;
    std::string sql = 
        "DELETE FROM Playlist WHERE PlaylistID=" + std::to_string(playlist_id) 
        + " AND UserID=" + std::to_string(this->id); 
        
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    if (!permited("Playlist", playlist_id))
    {
        std::cout << "You do not have a permission to delete this playlist.\n";
        return;
    }
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Failed to delete playlist from Playlist table.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    sql = "DELETE FROM PlaylistToSong WHERE PlaylistID=" + std::to_string(playlist_id);
    rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Failed to delete playlist mentions from PlaylistToSong table.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    std::cout << "Playlist deleted successfuly.\n";
}


void Session::print_playlist()
{
    int playlist_id;
    std::cout << "Input the ID of a playlist you'd like to print: ";
    std::cin >> playlist_id;
    
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    if (!permited("Playlist", playlist_id))
    {
        std::cout << "You do not have permission to view this playlist.\n";
        return;
    }
    
    std::string sql = 
        "SELECT Song.SongID, Song.MRL FROM (Song INNER JOIN PlaylistToSong ON PlaylistToSong.SongID=Song.SongID) WHERE PlaylistToSong.PlaylistID=" 
        + std::to_string(playlist_id) + "";
        
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), default_callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
}


void Session::print_playlists()
{
    std::string sql = 
        "SELECT PlaylistID, Name FROM Playlist WHERE UserID=" + std::to_string(this->id);
        
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), default_callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
}


void Session::add_to_playlist()
{
    int playlist_id;
    std::cout << "Input ID of the playlist you'd like to add song to: ";
    std::cin >> playlist_id;
    
    int song_id;
    std::cout << "Input the ID of the song you'd like to add to playlist: ";
    std::cin >> song_id;
    
    song_id = snatch_song_internal(song_id);
    
    std::string sql = 
        "INSERT INTO PlaylistToSong (PlaylistID, SongID) VALUES ((SELECT PlaylistID FROM Playlist WHERE PlaylistID=" 
        + std::to_string(playlist_id) + ")," + std::to_string(song_id) + ")";
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "There is no such song or playlist with given ID.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    std::cout << "Song was successfuly added to playlist.\n";
}


void Session::del_from_playlist()
{
    int playlist_id;
    std::cout << "Input ID of the playlist you'd like to delete song from: ";
    std::cin >> playlist_id;
    
    int song_id;
    std::cout << "Input ID of the song you'd like to delete: ";
    std::cin >> song_id;
    
    if(!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    if (!permited("Playlist", playlist_id))
    {
        std::cout << "You do not have permission to view this playlist.\n";
        return;
    }
    
    std::string sql = "DELETE FROM PlaylistToSong WHERE PlaylistID=" + std::to_string(playlist_id) 
        + " AND SongID=" + std::to_string(song_id);
    
    char* zErrMsg;
    int rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    std::cout << "Song was succesfuly removed from the playlist.\n";
}


int Session::playlist_retrieval_callback(
    void*  used,
    int    argc,
    char** argv,
    char** azColName
    )
{
    std::vector<std::string>* mrls = static_cast< std::vector<std::string>* >(used);
    
    std::cout << "\n";
    
    for (int i = 0; i < argc; i++)
    {
        mrls->push_back( std::string(argv[i]) );
        std::cout << argv[i] << "\n";
    }
    
    return 0;
}


void Session::set_playlist()
{
    std::vector<std::string> result = {};
    
    int playlist_id;
    std::cout << "Input the ID of a playlist you'd like to retrieve: ";
    std::cin >> playlist_id;
    
    std::string sql = 
        "SELECT Song.MRL FROM Song INNER JOIN PlaylistToSong WHERE (PlaylistToSong.PlaylistID=" 
        + std::to_string(playlist_id) + " AND PlaylistToSong.SongID=Song.SongID)";
        
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    char* zErrMsg; 
    int rc = sqlite3_exec(this->db, sql.c_str(), playlist_retrieval_callback, &result, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Failed to retrieve a playlist.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    }
    
    current_playlist = Playlist();
    current_playlist.ser_url_list(result);
}


void Session::del_user()
{
    if (!validate())
    {
        std::cout << "Invalid login. Action abborted.\n";
        return;
    }
    this->valid = false;
    
    std::string confirmation = "";
    std::cout << "This action cannot be undone. Input this user's login to confirm: ";
    std::cin >> confirmation;
    
    if (confirmation.compare(this->login) != 0)
    {
        std::cout << "Incorrect login. Action aborted.\n";
    }
    
    std::string sql_del_song_to_playlist = 
        "DELETE FROM PlaylistToSong WHERE (SELECT SongID FROM Song WHERE UserID=" + std::to_string(this->id) + ")";
        
    std::string sql_del_playlists = 
        "DELETE FROM Playlist WHERE UserID=" + std::to_string(this->id);
        
    std::string sql_del_songs =
        "DELETE FROM Song WHERE UserID=" + std::to_string(this->id);
        
    std::string sql_del_usr_cred = 
        "DELETE FROM User WHERE UserID=" + std::to_string(this->id);
        
    int rc; char* zErrMsg;
    
    rc = sqlite3_exec(this->db, sql_del_song_to_playlist.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Failed to delete entries in PlaylistToSong table.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    rc = sqlite3_exec(this->db, sql_del_playlists.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Failed to delete playlists.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    rc = sqlite3_exec(this->db, sql_del_songs.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Failed to delete songs.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    rc = sqlite3_exec(this->db, sql_del_usr_cred.c_str(), 0, 0, &zErrMsg);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Failed to delete user credentials.\n";
        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        return;
    }
    
    std::cout << "This user was deleted succesfuly.\n";
    
    this->logout();
}
