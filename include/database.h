/*
    sending and receiving data from the SQL is surprisingly SLOW
    my db has 5 entries and is 15kbs and its taking 2mins to
    read from it (O-o)

    if its still stupid slow i'll have to not use a SQL database

    update April 9 2023
    I decided to move away from using SQL, I will instead just write
    my image data to a text file and load the text file as the
    person loads an image of the same name. Possibly could be
    that i'm on a NVMe SSD but loading files is faster then SQL

    Since I spent two weeks on this SQL garbage I'll still use it
    to store the last 10 files that were opened.

    but I by no means need SQL, i've included my class for reading
    and writting INI text files which I can substitute when required

    update april 13 2023
    uh yeah moved the project to microsoft visual studios community
    and was having issues compiling sqlite3, so i've dummied out
    the sqlite3 functions going to entirely avoid using it sadly

    i made a define called FUCKSQL although I never use directives
    I should be able to switch the use of SQL on and off using it?
*/
#ifndef DATABASE_H
#define DATABASE_H


#include <iostream>
#include <string>
#include <vector>

#include <stdio.h> // for printf()
#include "stringext.h"
#include "stringenc.h"
#include "vectorext.h"
#include "filesystem.h"
#include "bytestream.h"


#if !defined(FUCKSQL)
#include <sqlite3.h>
#else
#include "scriptini.h"
#ifndef SQLITE_ABORT
#define SQLITE_ABORT 4
#endif
#ifndef SQLITE_AUTH
#define SQLITE_AUTH 23
#endif
#ifndef SQLITE_BUSY
#define SQLITE_BUSY 5
#endif
#ifndef SQLITE_CANTOPEN
#define SQLITE_CANTOPEN 14
#endif
#ifndef SQLITE_CONSTRAINT
#define SQLITE_CONSTRAINT 19
#endif
#ifndef SQLITE_CORRUPT
#define SQLITE_CORRUPT 11
#endif
#ifndef SQLITE_DONE
#define SQLITE_DONE 101
#endif
#ifndef SQLITE_EMPTY
#define SQLITE_EMPTY 16
#endif
#ifndef SQLITE_ERROR
#define SQLITE_ERROR 1
#endif
#ifndef SQLITE_FORMAT
#define SQLITE_FORMAT 24
#endif
#ifndef SQLITE_FULL
#define SQLITE_FULL 13
#endif
#ifndef SQLITE_INTERNAL
#define SQLITE_INTERNAL 2
#endif
#ifndef SQLITE_INTERRUPT
#define SQLITE_INTERRUPT 9
#endif
#ifndef SQLITE_IOERR
#define SQLITE_IOERR 10
#endif
#ifndef SQLITE_LOCKED
#define SQLITE_LOCKED 6
#endif
#ifndef SQLITE_MISMATCH
#define SQLITE_MISMATCH 20
#endif
#ifndef SQLITE_MISUSE
#define SQLITE_MISUSE 21
#endif
#ifndef SQLITE_NOLFS
#define SQLITE_NOLFS 22
#endif
#ifndef SQLITE_NOMEM
#define SQLITE_NOMEM 7
#endif
#ifndef SQLITE_NOTADB
#define SQLITE_NOTADB 26
#endif
#ifndef SQLITE_NOTFOUND
#define SQLITE_NOTFOUND 12
#endif
#ifndef SQLITE_NOTICE
#define SQLITE_NOTICE 27
#endif
#ifndef SQLITE_OK
#define SQLITE_OK 0
#endif
#ifndef SQLITE_PERM
#define SQLITE_PERM 3
#endif
#ifndef SQLITE_PROTOCOL
#define SQLITE_PROTOCOL 15
#endif
#ifndef SQLITE_RANGE
#define SQLITE_RANGE 25
#endif
#ifndef SQLITE_READONLY
#define SQLITE_READONLY 8
#endif
#ifndef SQLITE_ROW
#define SQLITE_ROW 100
#endif
#ifndef SQLITE_SCHEMA
#define SQLITE_SCHEMA 17
#endif
#ifndef SQLITE_TOOBIG
#define SQLITE_TOOBIG 18
#endif
#ifndef SQLITE_WARNING
#define SQLITE_WARNING 28
#endif

class sqlite3 {
    sqlite3 ();
    ~sqlite3 ();
    };
class sqlite3_stmt {
    sqlite3_stmt ();
    ~sqlite3_stmt ();
    };
int sqlite3_prepare_v2( sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail );
int exec_callback( void* udp, int c_num, char** c_vals, char** c_names );
int sqlite3_exec( sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **errmsg );
const char *sqlite3_errmsg(sqlite3*);
int sqlite3_finalize(sqlite3_stmt *pStmt);
int sqlite3_step(sqlite3_stmt*);
int sqlite3_column_int(sqlite3_stmt*, int iCol);
double sqlite3_column_double(sqlite3_stmt*, int iCol);
const unsigned char *sqlite3_column_text(sqlite3_stmt*, int iCol);
int sqlite3_open( const char *filename, sqlite3 **ppDb );
int sqlite3_close(sqlite3*);
#endif


struct region_data {
    std::string tag;
    bool add_tag_to_image;
    signed int x;
    signed int y;
    signed int w;
    signed int h;
    region_data (signed int img_width = 0, signed int img_height = 0, signed int img_x = 0, signed int img_y = 0, std::string name = "", bool addtag = false);
    };

struct category_data {
    std::string name; // name, object, place, etc
    std::string description;
    category_data (std::string class_name = "", std::string class_desc = "");
    };

struct tag_data {
//    category_data* category;
    std::string name;
    std::string description;
    tag_data (std::string tag_name = "", std::string tag_desc = "");
    };

struct image_data {
    std::string md5;
    std::string fullpath;
    std::string comment;
    unsigned int width;
    unsigned int height;

    std::vector<tag_data> tag;
    std::vector<region_data> region;

    image_data (std::string m = "", std::string f = "", std::string c = "", unsigned int w = 0, unsigned int h = 0);
    };

class database {
public:
    signed int selected;
    std::vector<image_data> image;
    std::vector<std::string> recents; // utf8

    database ();
    virtual ~database ();

    /**
     * Opens a connection to the database, also creates new one if one not found
     * @param Name or fullpath to database
     * @return Bool: True if Successful Connection, False otherwise
     */
    bool open (const char* dbfile = "imvdb.db", bool verbose = false);

    void save_recents_as_ini ();

    /**
     * Closes the connection to the database
     * @return Void
     */
    void close ();

    /**
     * Retrieves settings from the database for a given image
     * @param String: MD5 Hash of the Image that the settings are for
     * @param Struct: object to write to
     * @return Void
     */
    void load (image_data &img);

    /**
     * dumps everything from local database back into the external sql database
     * @return Void
     */
    void unload ();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions to Manage Local Database

    void set (image_data img);

    bool isSelected ();

    void tag (std::string tag_name, signed int index = -1);

    void add (region_data reg, signed int index = -1);

    void remove (unsigned int region_index, signed int index = -1);

    void edit (unsigned int region_index, unsigned int posx = 0, unsigned int posy = 0, unsigned int width = 0, unsigned int height = 0, signed int index = -1);

    std::string name (unsigned int index);

    signed int posx (unsigned int index);

    signed int posy (unsigned int index);

    signed int width (unsigned int index);

    signed int height (unsigned int index);

    unsigned int count ();

    void reset ();

    bool loadCSV (std::string file, signed int index = -1, bool verbose = false);

    bool writeCSV (std::string file, signed int index = -1);

private:


    sqlite3* db;


    // caches the query command for sending delete or write commands, use begin to start cache and end to finish and execute
    bool cache_command;
    std::string cache;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions to Manage External Database

    std::string arrayToSequence (std::vector<std::string> &arr, unsigned int count_div = 1, unsigned int offset = 0);
    unsigned int countSequence (std::string &str);

    /**
     * Executes Sql commands that don't require a return
     * @param String: SQL Command to execute
     * @param Bool: (Default:false) Prints additional to console
     * @return Int: Error Code, 1 = OK
     */
    int query (std::string sql, bool verbose = true);

    /**
     * Caches any queries sent to query() instead of excuting them, use end() to executed cached commands
     * @return Void
     */
    void begin ();

    /**
     * Executes anything SQL commands that were cached using begin()
     * @return Void
     */
    void end ();

    /**
     * Adds new Row to table with the given fields
     * @param String: Table name
     * @param String[]: column's to add data in
     * @param String[]: values to add into the column's fields
     * @return Void
     */
    void insert (std::string table, std::vector<std::string> columns, std::vector<std::string> values);

    /**
     * The Number of rows or effectively the last row in the given table
     * @param String: Table name
     * @param String: (Default:rowid) Column name to Search
     * @param Bool: (Default:false) Prints additional to console
     * @return Int: Number of Rows in a given table
     */
    unsigned int lastrow (std::string table, std::string key = "rowid", bool verbose = false);

    /**
     * Retrieve fields from specified columns in a given table
     * @param String: Table name
     * @param String: Column name to Search
     * @param String: Keyword within the column, if matches gets retrieved
     * @param String[]: Names of Column's to collected fields from
     * @param String: Data types of each column field. ex for columns (name, age) set 'TI' for (TEXT, INT)
     * @param Int: set to 0 to read ALL rows, else count will terminate after the key value has been found N times
     * @return String[]: fields retrieved from the given columns based on the search criteria
     */
    std::vector<std::string> fetch (std::string table, std::string keyfield, std::string key, std::vector<std::string> columns, std::string dataStruct, unsigned int count = 1, bool verbose = false);

    /**
     * Overwrites a row entry in a given table
     * @param String: Table name
     * @param String: Column name to Search
     * @param String: Keyword within the column, if matches gets overwritten
     * @param String[]: Names of Column's to batch replace data in
     * @param String[]: Values to substitute in each column
     * @param Int: set to 0 to modify ALL rows, else count will terminate after the key value has been found N times
     * @return Void
     */
    void modify (std::string table, std::string key_field, std::string key, std::vector<std::string> columns, std::vector<std::string> values, unsigned int count = 1);

    /**
     * Counts the number of rows in a given table
     * @param String: Table name
     * @param String: Column name
     * @return Int: Number of Rows
     */
    unsigned int rows (std::string table, bool verbose = false);

    /**
     * Retrieves rowids in an array based on a fields present in a column
     * @param String: Table name
     * @param String: Column name
     * @param String: Search Key Word
     * @param String: (Default:rowid) Change which column field to return
     * @param Int: set to 0 to modify ALL rows, else count will terminate after the key value has been found N times
     * @param Bool: (Default:false) Prints additional to console
     * @return Int[]: Array of collected rowids
     */
    std::vector<unsigned int> rowid (std::string table, std::string keyfield, std::string key, std::string rowid = "rowid", unsigned int count = 0, bool verbose = true);

    };


#endif // DATABASE_H
