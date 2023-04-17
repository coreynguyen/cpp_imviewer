#include "database.h"


#if defined(FUCKSQL)
int sqlite3_prepare_v2( sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail ) {return 0;}
int exec_callback( void* udp, int c_num, char** c_vals, char** c_names ) {return 0;}
int sqlite3_exec( sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **errmsg ) {return 0;}
const char *sqlite3_errmsg(sqlite3*) {return "";}
int sqlite3_finalize(sqlite3_stmt *pStmt) {return 0;}
int sqlite3_step(sqlite3_stmt*) {return 0;}
int sqlite3_column_int(sqlite3_stmt*, int iCol) {return 0;}
double sqlite3_column_double(sqlite3_stmt*, int iCol) {return 0.0;}
const unsigned char *sqlite3_column_text(sqlite3_stmt*, int iCol) {return 0;}
int sqlite3_open( const char *filename, sqlite3 **ppDb ) {return 0;}
int sqlite3_close(sqlite3*) {return 0;}
#endif


region_data::region_data::region_data (signed int img_width, signed int img_height, signed int img_x, signed int img_y, std::string name, bool addtag) {
    w = img_width; h = img_height; x = img_x; y = img_y; tag = name; add_tag_to_image = addtag;

    //std::cout << "Region(" << w << ", " << h << ", " << x << ", " << y << ")" << std::endl;
    }

category_data::category_data::category_data (std::string class_name, std::string class_desc) {
    name = class_name; description = class_desc;
    }

tag_data::tag_data::tag_data (std::string tag_name, std::string tag_desc) {
    name = tag_name; description = tag_desc;
    }

image_data::image_data (std::string m, std::string f, std::string c, unsigned int w, unsigned int h) {
    md5 = m; fullpath = f; comment = c; width = w; height = h;
    }




database::database () {
#if !defined(FUCKSQL)
    sqlite3* db = nullptr;
#endif
    cache_command = false;
    cache = "";
    }

database::~database () {
    if (db != nullptr) {sqlite3_close(db);}
    }

std::string database::arrayToSequence (std::vector<std::string> &arr, unsigned int count_div, unsigned int offset) {
    std::string seq = "";
    if (count_div < 1) {count_div = 1;}
    if (offset > count_div) {offset = count_div;}
    unsigned int num = (unsigned int)(arr.size() / count_div);
    std::vector<signed int> tmp;
    signed int val;
    for (unsigned int i = 0; i < num; i++) {
        val = convert_to<signed int>(arr.at((i * count_div) + offset));
        if (val == 0) {continue;}
        if (std::find(tmp.begin(), tmp.end(), val) == tmp.end()) {
            tmp.push_back(val);
            }
        }
    std::sort(tmp.begin(), tmp.end());
    num = (unsigned int)tmp.size();
    for (unsigned int i = 0; i < num; i++) {
        if (seq.size() > 0) {seq += ", ";}
        seq += to_string(tmp.at(i));
        }
    return seq;
    }

unsigned int database::countSequence (std::string &str) {
    return (unsigned int)(std::count_if( str.begin(), str.end(), []( char c ){return c ==',';}) + 1);
    }

int database::query (std::string sql, bool verbose) {
    /*
        I thought i could just pass whatever text
        into sqlite3_exec and it would execute it
        ... nope
    */
    if (db == nullptr) {std::cout << "no db found" << std::endl; return -1;}
    int rc = 0;
    if (!cache_command) {
        char *zErrMsg = 0; // Save any error messages
        //sqlite3_exec(db, "BEGIN", NULL, NULL, NULL);
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
        //sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
        if (verbose) {

            std::cout << "Query: \n" << sql << std::endl;
            //std::cout << "Result(" << rc << "): " << std::endl << zErrMsg << std::endl << std::endl; // breaks the program

            if (rc > 0) {
                std::string errStr = "";
                switch (rc) {
                    case 0: {errStr = "OK"; break;}
                    case 1: {errStr = "SYNTAX ERROR"; break;}
                    case 2: {errStr = "INTERNAL"; break;}
                    case 3: {errStr = "PERM"; break;}
                    case 4: {errStr = "ABORT"; break;}
                    case 5: {errStr = "BUSY"; break;}
                    case 6: {errStr = "LOCKED"; break;}
                    case 7: {errStr = "NOMEM"; break;}
                    case 8: {errStr = "READONLY"; break;}
                    case 9: {errStr = "INTERRUPT"; break;}
                    case 10: {errStr = "IOERR"; break;}
                    case 11: {errStr = "CORRUPT"; break;}
                    case 12: {errStr = "NOTFOUND"; break;}
                    case 13: {errStr = "FULL"; break;}
                    case 14: {errStr = "CANTOPEN"; break;}
                    case 15: {errStr = "PROTOCOL"; break;}
                    case 16: {errStr = "EMPTY"; break;}
                    case 17: {errStr = "SCHEMA"; break;}
                    case 18: {errStr = "TOOBIG"; break;}
                    case 19: {errStr = "CONSTRAINT"; break;}
                    case 20: {errStr = "MISMATCH"; break;}
                    case 21: {errStr = "MISUSE"; break;}
                    case 22: {errStr = "NOLFS"; break;}
                    case 23: {errStr = "AUTH"; break;}
                    case 24: {errStr = "FORMAT"; break;}
                    case 25: {errStr = "RANGE"; break;}
                    case 26: {errStr = "NOTADB"; break;}
                    case 27: {errStr = "NOTICE"; break;}
                    case 28: {errStr = "WARNING"; break;}
                    case 100: {errStr = "ROW"; break;}
                    case 101: {errStr = "DONE"; break;}
                    case 256: {errStr = "OK_LOAD_PERMANENTLY"; break;}
                    case 257: {errStr = "ERROR_MISSING_COLLSEQ"; break;}
                    case 261: {errStr = "BUSY_RECOVERY"; break;}
                    case 262: {errStr = "LOCKED_SHAREDCACHE"; break;}
                    case 264: {errStr = "READONLY_RECOVERY"; break;}
                    case 266: {errStr = "IOERR_READ"; break;}
                    case 267: {errStr = "CORRUPT_VTAB"; break;}
                    case 270: {errStr = "CANTOPEN_NOTEMPDIR"; break;}
                    case 275: {errStr = "CONSTRAINT_CHECK"; break;}
                    case 279: {errStr = "AUTH_USER"; break;}
                    case 283: {errStr = "NOTICE_RECOVER_WAL"; break;}
                    case 284: {errStr = "WARNING_AUTOINDEX"; break;}
                    case 513: {errStr = "ERROR_RETRY"; break;}
                    case 516: {errStr = "ABORT_ROLLBACK"; break;}
                    case 517: {errStr = "BUSY_SNAPSHOT"; break;}
                    case 518: {errStr = "LOCKED_VTAB"; break;}
                    case 520: {errStr = "READONLY_CANTLOCK"; break;}
                    case 522: {errStr = "IOERR_SHORT_READ"; break;}
                    case 523: {errStr = "CORRUPT_SEQUENCE"; break;}
                    case 526: {errStr = "CANTOPEN_ISDIR"; break;}
                    case 531: {errStr = "CONSTRAINT_COMMITHOOK"; break;}
                    case 539: {errStr = "NOTICE_RECOVER_ROLLBACK"; break;}
                    case 769: {errStr = "ERROR_SNAPSHOT"; break;}
                    case 773: {errStr = "BUSY_TIMEOUT"; break;}
                    case 776: {errStr = "READONLY_ROLLBACK"; break;}
                    case 778: {errStr = "IOERR_WRITE"; break;}
                    case 779: {errStr = "CORRUPT_INDEX"; break;}
                    case 782: {errStr = "CANTOPEN_FULLPATH"; break;}
                    case 787: {errStr = "CONSTRAINT_FOREIGNKEY"; break;}
                    case 1032: {errStr = "READONLY_DBMOVED"; break;}
                    case 1034: {errStr = "IOERR_FSYNC"; break;}
                    case 1038: {errStr = "CANTOPEN_CONVPATH"; break;}
                    case 1043: {errStr = "CONSTRAINT_FUNCTION"; break;}
                    case 1288: {errStr = "READONLY_CANTINIT"; break;}
                    case 1290: {errStr = "IOERR_DIR_FSYNC"; break;}
                    case 1294: {errStr = "CANTOPEN_DIRTYWAL"; break;}
                    case 1299: {errStr = "CONSTRAINT_NOTNULL"; break;}
                    case 1544: {errStr = "READONLY_DIRECTORY"; break;}
                    case 1546: {errStr = "IOERR_TRUNCATE"; break;}
                    case 1550: {errStr = "CANTOPEN_SYMLINK"; break;}
                    case 1555: {errStr = "CONSTRAINT_PRIMARYKEY"; break;}
                    case 1802: {errStr = "IOERR_FSTAT"; break;}
                    case 1811: {errStr = "CONSTRAINT_TRIGGER"; break;}
                    case 2058: {errStr = "IOERR_UNLOCK"; break;}
                    case 2067: {errStr = "CONSTRAINT_UNIQUE"; break;}
                    case 2314: {errStr = "IOERR_RDLOCK"; break;}
                    case 2323: {errStr = "CONSTRAINT_VTAB"; break;}
                    case 2570: {errStr = "IOERR_DELETE"; break;}
                    case 2579: {errStr = "CONSTRAINT_ROWID"; break;}
                    case 2826: {errStr = "IOERR_BLOCKED"; break;}
                    case 2835: {errStr = "CONSTRAINT_PINNED"; break;}
                    case 3082: {errStr = "IOERR_NOMEM"; break;}
                    case 3091: {errStr = "CONSTRAINT_DATATYPE"; break;}
                    case 3338: {errStr = "IOERR_ACCESS"; break;}
                    case 3594: {errStr = "IOERR_CHECKRESERVEDLOCK"; break;}
                    case 3850: {errStr = "IOERR_LOCK"; break;}
                    case 4106: {errStr = "IOERR_CLOSE"; break;}
                    case 4362: {errStr = "IOERR_DIR_CLOSE"; break;}
                    case 4618: {errStr = "IOERR_SHMOPEN"; break;}
                    case 4874: {errStr = "IOERR_SHMSIZE"; break;}
                    case 5130: {errStr = "IOERR_SHMLOCK"; break;}
                    case 5386: {errStr = "IOERR_SHMMAP"; break;}
                    case 5642: {errStr = "IOERR_SEEK"; break;}
                    case 5898: {errStr = "IOERR_DELETE_NOENT"; break;}
                    case 6154: {errStr = "IOERR_MMAP"; break;}
                    case 6410: {errStr = "IOERR_GETTEMPPATH"; break;}
                    case 6666: {errStr = "IOERR_CONVPATH"; break;}
                    case 6922: {errStr = "IOERR_VNODE"; break;}
                    case 7178: {errStr = "IOERR_AUTH"; break;}
                    case 7434: {errStr = "IOERR_BEGIN_ATOMIC"; break;}
                    case 7690: {errStr = "IOERR_COMMIT_ATOMIC"; break;}
                    case 7946: {errStr = "IOERR_ROLLBACK_ATOMIC"; break;}
                    case 8202: {errStr = "IOERR_DATA"; break;}
                    case 8458: {errStr = "IOERR_CORRUPTFS"; break;}
                    }
                std::cout << "[ERROR " << rc << " " << errStr << "] " << zErrMsg << std::endl;
                }

            }
        } else {cache += sql + "\n";}

    return rc;
    }

void database::begin () {
    if (cache.size() > 0) {
        std::cout << "Error: Cache wasn't cleared, unexcuted commands:\n\t" << cache << std::endl;
        }
    cache = "";
    cache_command = true;
    }

void database::end () {
    if (cache_command) {
        cache_command = false;
        if (cache.size() > 0) {
            query("BEGIN;\n" + cache + "COMMIT;\n");
            cache = "";
            } else {std::cout << "error: \t No cached SQL command to execute\n";}
        }
    }

void database::insert (std::string table, std::vector<std::string> columns, std::vector<std::string> values) {
    /*
        add entries to an existing table
    */
    if (table == "" || columns.size() == 0 || values.size() == 0) {
        std::cout << "Error: \tfailed to Insert, invalid inputs" << std::endl;
        return;
        }
    std::string c = "";
    std::string v = "";
    std::string ignore = "";
    if (cache_command) {ignore = " or IGNORE";}
    size_t num_values = values.size();
    if (num_values == columns.size()) {
        for (unsigned int i = 0; i < num_values; i++) {
            //if (values.at(i).size() == 0) {continue;}
            c += "'" + columns.at(i) + (i + 1 < num_values ? "', " : "'");
            v += "'" + values.at(i) + (i + 1 < num_values ? "', " : "'");
            }

        query("INSERT" + ignore + " INTO '" + table + "' ( " + c + " ) VALUES ( " + v + " );");

        } else {std::cout << "Error: \t Failed to Insert, Mismatched Values to Columns" << std::endl; return;}

    }

unsigned int database::lastrow (std::string table, std::string key, bool verbose) {

    if (db == nullptr) {std::cout << "no db found" << std::endl; return 0;}

    std::string sql = "SELECT MAX(" + key + ") FROM " + table + " LIMIT 1;";
    if (verbose) {std::cout << "QUERY: \t" << sql << std::endl;}

    // binary statement
    sqlite3_stmt* stmt;

    // compile sql statement to binary
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cout << "ERROR: while compiling sql: \t" << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return 0;
        }

    // execute sql statement, and while there are rows returned, print ID
    int rc = 0;
    //char *zErrMsg = 0; // Save any error messages
    //int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
    unsigned int data = 0;
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) { // 100
        if (verbose) {std::cout << "TEST: ID = " << sqlite3_column_int(stmt, 0) << std::endl;}
        int id = sqlite3_column_int(stmt, 0);
        if (verbose) {std::cout << "INTEGER: \t" << id << std::endl;}
        data = id;
        }
    sqlite3_finalize(stmt);
    return data;
    }

std::vector<std::string> database::fetch (std::string table, std::string keyfield, std::string key, std::vector<std::string> columns, std::string dataStruct, unsigned int count, bool verbose) {
    /*
        reads entries from a table, all values are converted to strings

        sqlite3_exec does not retrieve data as explained here:
        https://stackoverflow.com/a/27386276

    */
    if (db == nullptr) {std::cout << "no db found" << std::endl; return (std::vector<std::string>{});}




    std::string keyquote = "";
    if (key.find("'") == std::string::npos) {
        keyquote = "'";
        }
    std::string v = "";
    for (unsigned int i = 0; i < columns.size(); i++) {
        if (v.size() > 0) { v += ", "; }
        v += columns.at(i);
        }
    std::string limit = "";
    if (count > 0) {limit = " LIMIT " + to_string(count);}
    std::string sql = "";
    if (key.find(",") != std::string::npos) {
        sql = "SELECT " + v + " FROM " + table + " WHERE " + keyfield + " IN (" + key + ")" + limit +";";
        }
    else {

        sql = "SELECT " + v + " FROM " + table + " WHERE " + keyfield + " = " + keyquote + key + keyquote + limit +";";
        }
    if (verbose) {std::cout << "FETCH QUERY: \t" << sql << std::endl;}

    // binary statement
    sqlite3_stmt* stmt;

    // compile sql statement to binary

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cout << "ERROR in Fetch Fn.: \t" << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return (std::vector<std::string>{});
        }

    // execute sql statement, and while there are rows returned, print ID
    int rc = 0;
    //char *zErrMsg = 0; // Save any error messages
    //int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
    size_t num_cols = dataStruct.size();
    unsigned char type;
    std::vector<std::string> data;
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) { // 100
        if (verbose) {std::cout << "TEST: ID = " << sqlite3_column_int(stmt, 0) << std::endl;}
        for (unsigned int c = 0; c < num_cols; c++) {

            type = (unsigned char)(int)dataStruct[c];
            switch (type) {
                case 'N': { // NULL. The value is a NULL value.
                    break;
                    }

                case 'I': { // NTEGER. The value is a signed integer, stored in 0, 1, 2, 3, 4, 6, or 8 bytes depending on the magnitude of the value.
                    int id = sqlite3_column_int(stmt, c);
                    if (verbose) {std::cout << "INTEGER: \t" << id << std::endl;}
                    data.push_back(to_string(id));
                    break;
                    }

                case 'R': { // REAL. The value is a floating point value, stored as an 8-byte IEEE floating point number.

                    double value = sqlite3_column_double(stmt, c);
                    if (verbose) {std::cout << "REAL: \t" << value << std::endl;}
                    data.push_back(to_string(value));

                    break;
                    }

                case 'T': { // TEXT. The value is a text string, stored using the database encoding (UTF-8, UTF-16BE or UTF-16LE).
                    const char* name = (const char*)sqlite3_column_text(stmt, c);
                    if (verbose) {std::cout << "TEXT: \t" << name << std::endl;}
                    data.push_back(std::string(name));
                    break;
                    }

                case 'B': { // BLOB.
                    break;
                    }
                }
            }


        }
    sqlite3_finalize(stmt);
    return data;
    }

void database::modify (std::string table, std::string key_field, std::string key, std::vector<std::string> columns, std::vector<std::string> values, unsigned int count) {
    /*
        replace row value in the table
    */
    if (table == "" || columns.size() == 0 || values.size() == 0) {return;}

    std::string c = "";
    for (unsigned int i = 0; i < columns.size(); i++) {
        c += columns.at(i) + " = '" + values.at(i) + (i + 1 < columns.size() ? "', " : "'");
        }

    if (count > 0) {
        query(
            "UPDATE " + table + " SET " + c + " WHERE " +
            key_field + " IN (SELECT " +
            key_field + " FROM " + table + " WHERE " +
            key_field + " = '" + key + "' LIMIT " + to_string(count) + ");"
            );
        }
    else {
        query("UPDATE " + table + " SET " + c + " WHERE " + key_field + " = '" + key + ";");
        }
    }

unsigned int database::rows (std::string table, bool verbose) {
    /*
        https://stackoverflow.com/a/51951734
        tanius:
            "Should be faster, but I did not test that: SELECT MAX(rowid) FROM your_table_name"
    */

    if (db == nullptr) {std::cout << "no db found" << std::endl; return 0;}


//    std::string sql =  "SELECT '" + column + "' FROM '" + table + "';";

    std::string sql = "SELECT MAX(rowid) FROM " + table + ";";

    if (verbose) {std::cout << "QUERY: \t" << sql << std::endl;}

    // binary statement
    sqlite3_stmt* stmt;

    // compile sql statement to binary
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cout << "ERROR: while compiling sql: \t" << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return 0;
        }

    // execute sql statement, and while there are rows returned, print ID
    int rc = 0;
    //char *zErrMsg = 0; // Save any error messages
    //int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);


    unsigned int cnt=0;
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) { // 100
                    cnt = sqlite3_column_int(stmt, 0);
        //            if (verbose) {std::cout << "TEST: ID = " << data << std::endl;}
        //
//        cnt++;
        }
    sqlite3_finalize(stmt);
    return cnt;
    }

std::vector<unsigned int> database::rowid (std::string table, std::string keyfield, std::string key, std::string rowid, unsigned int count, bool verbose) {

    if (db == nullptr) {std::cout << "no db found" << std::endl; return (std::vector<unsigned int>{});}

    std::string limit = "";
    if (count > 0) {limit = " LIMIT " + to_string(count);}
    std::string sql = "SELECT " + rowid + " FROM " + table + " WHERE " + keyfield + " = '" + key + "'" + limit + ";";
    if (verbose) {std::cout << "QUERY: \t" << sql << std::endl;}

    // binary statement
    sqlite3_stmt* stmt;

    // compile sql statement to binary
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cout << "ERROR: while compiling sql: \t" << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return (std::vector<unsigned int>{});
        }

    // execute sql statement, and while there are rows returned, print ID
    int rc = 0;
    //char *zErrMsg = 0; // Save any error messages
    //int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
    std::vector<unsigned int> id;
    //unsigned char type;
    std::vector<std::string> data;
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (verbose) {std::cout << "TEST: ID = " << sqlite3_column_int(stmt, 0) << std::endl;}
        id.push_back(sqlite3_column_int(stmt, 0));
        //std::cout << "INTEGER: \t" << id << std::endl;
        }
    sqlite3_finalize(stmt);
    return id;
    }



/*
    specific functions for the app to access and stuff
*/



bool database::open (const char* dbfile, bool verbose) {
    /*
        not working quite right, from the examples I reviewed
        seems I'm creating the database each time I open it...

    */


#if !defined(FUCKSQL)
    // check if db has already been assigned to the pointer
    if (db != nullptr) {
        std::cout << "database already open?" << std::endl;
        return false;
        }

    // open database
    int rc = 0;

    rc = sqlite3_open(dbfile, &db);

    // create new database
    if( !(rc = sqlite3_open(dbfile, &db)) ) {
        if (verbose ) {std::cout << "creating new db.." << std::endl;}

        // sql table creation query
        std::string sql;
        char *zErrMsg = 0; // Save any error messages
        /*
            I was using the row index 'rowid'
            but looks like when I delete a row
            the entries assume the displaced row
            index as it's rowid..

            instead i've added an id with auto
            increment, this seems to suit my
            needs to create a content id for the
            row

        */
        sql = (
            "CREATE TABLE \"recents\" ("
            "	\"id\"	            INTEGER NOT NULL UNIQUE,"
            "	\"filename\"	    TEXT,"
            "	                    PRIMARY KEY(\"id\" AUTOINCREMENT)"
            "    );"//"    ) WITHOUT ROWID;"
               ); if ( (rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg)) ) {if (verbose ) {std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;}}
//        sql = (
//            "CREATE TABLE \"category\" ("
//            "    \"id\"             INTEGER NOT NULL UNIQUE,"
//            "    \"name\"           TEXT NOT NULL UNIQUE,"
//            "    \"description\"    INTEGER,"
//            "    \"color\"          INTEGER,"
//            "                       PRIMARY KEY(\"id\"  AUTOINCREMENT)"
//            "    );"
//            ); if ( (rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg)) ) {std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;}
        sql = (
            "CREATE TABLE \"tag\" ("
            "    \"id\"             INTEGER NOT NULL UNIQUE,"
            "    \"name\"           TEXT NOT NULL UNIQUE,"
            "    \"description\"    INTEGER,"
//            "    \"category\"       INTEGER,"
            "                       PRIMARY KEY(\"id\" AUTOINCREMENT)"
            //"                       PRIMARY KEY(\"name\")"
            //"                       FOREIGN KEY(\"category\") REFERENCES \"tag\""
            "    );"
            ); if ( (rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg)) ) {if (verbose ) {std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;}}
        sql = (
            "CREATE TABLE \"region\" ("
            "    \"image\"          INTEGER NOT NULL," // ID of the image that the region is associated with
            "    \"tag\"            INTEGER," // ID of a tag associated with the region
            "    \"flag\"           INTEGER," // any settings, currently just 1 to include in tag pool
            "    \"posx\"           INTEGER,"
            "    \"posy\"           INTEGER,"
            "    \"width\"          INTEGER,"
            "    \"height\"         INTEGER"
            //"                       PRIMARY KEY(\"id\" AUTOINCREMENT)"
            "    );"
            ); if ( (rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg)) ) {if (verbose ) {std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;}}
        //sql = (
        //    "CREATE TABLE \"field\" ("
        //    "    \"id\"             INTEGER NOT NULL UNIQUE,"
        //    "    \"name\"	        TEXT     NOT NULL UNIQUE,"
        //    //"                       PRIMARY KEY(\"name\")"
        //    "                       PRIMARY KEY(\"id\" AUTOINCREMENT)"
        //    "    );"
        //    ); if ( (rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg)) ) {std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;}
        sql = (
            "CREATE TABLE \"image\" ("
            "    \"id\"             INTEGER NOT NULL UNIQUE,"
            "    \"hash\"	        TEXT NOT NULL UNIQUE,"
            "    \"fullpath\"       TEXT,"
            "    \"comment\"        TEXT,"
            "    \"width\"          INTEGER,"
            "    \"height\"         INTEGER,"
            "    \"num_tags\"       INTEGER," // number of tags associated with the image
            "    \"num_regions\"    INTEGER," // number of regions associated with the image
            //"                       PRIMARY KEY(\"hash\")"
            "                       PRIMARY KEY(\"id\" AUTOINCREMENT)"
            "    );"
            ); if ( (rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg)) ) {if (verbose ) {std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;}}
        sql = (
            /*
                ties in tags and regions to an image
                there will be multiple entries if an image has
                multiple regions or tags
            */
            "CREATE TABLE \"meta\" (" // connects tags to the image
            "    \"image\"          INTEGER," // ID of the image
            "    \"tag\"            INTEGER" //  ID of the tag associated with the image ID
//            "    \"region\"         INTEGER"
            //"    \"field\"          INTEGER"
            //"                        FOREIGN KEY(\"field\")  REFERENCES \"field\"(\"name\") ON DELETE CASCADE ON UPDATE NO ACTION,"
            //"                        FOREIGN KEY(\"image\")  REFERENCES \"image\"(\"hash\") ON DELETE CASCADE ON UPDATE NO ACTION,"
            //"                        FOREIGN KEY(\"tag\")    REFERENCES \"tag\"(\"name\")   ON DELETE CASCADE ON UPDATE NO ACTION,"
            //"                        FOREIGN KEY(\"region\") REFERENCES \"region\"          ON DELETE CASCADE ON UPDATE NO ACTION"
            "    );"

            ); if ( (rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg)) ) {if (verbose ) {std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;}}

        // get recent files
        recents = std::vector<std::string>(10, "");
        std::vector<std::string> tmp = fetch("recents", "id", "1, 2, 3, 4, 5, 6, 7, 8, 9, 10", std::vector<std::string>{"filename"}, "T", 0);

        if (verbose ) {std::cout << "Files From Query: \t" << tmp.size() << std::endl;}

        for (unsigned int i = 0; i < 10; i++) {
            recents.at(i) = "";
            if (i < tmp.size()) {
                recents.at(i) = tmp.at(i);
                if (verbose ) {std::cout << "Read" << i << ": \t" << tmp.at(i) << std::endl;}
                }
            }
        if (verbose ) {
            std::cout << "Files:" << std::endl;
            for (unsigned int i = 0; i < 10; i++) {
                std::cout << "File" << i << ": \t" << recents.at(i) << std::endl;
                }
            }


        return true;
        }

    if (verbose ) {std::cout << "DB Open Error: " << sqlite3_errmsg(db) << std::endl;}

#else

    scriptini ini;
    recents.clear();
    const unsigned char num_recents = 10;
    recents = std::vector<std::string>(num_recents, "");
    if (ini.open(L"config.ini")) {
        if (ini.find_section("recents")) {
            std::string str;
            std::string key = os::machineName();
            unsigned int strlen;
            
            for (unsigned char i = 0; i < num_recents; i++) {
                str = ini.get_string("recents", "file" + to_string((int)i));
                strlen = str.size();
                if (strlen > 0) {
                    bytestream f;
                    f.base64_decode(str);
                    f.apply_xor_key(key.c_str(), key.size());
                    str = f.readline();
                    f.close();
                    str = trim(str, " ");
                    if (str.size() > 0) {
                        recents.at(i) = str;
                        }
                    } else {std::cout << "no go on file" << to_string((int)i) << std::endl;}
                }
            } else {std::cout << "no recents category\n";}
         } else {std::cout << "failed to read recents files from ini config\n";}


#endif



    return false;
    }

void database::save_recents_as_ini () {
    
    std::string ini = "[recents]\r\n";
    unsigned int inilen = ini.size();
    std::string str  = "";
    
    std::string key = os::machineName();
    //std::cout << "Key: \t" << key << std::endl;
    unsigned int num_rencets = recents.size();
    bytestream s;
    for (unsigned int i = 0; i < num_rencets; i++) {
        if (recents.at(i).size() == 0) {continue;}
        
        s.resize(recents.at(i).size());
        s.seek(0);
        s.writestring(recents.at(i), recents.at(i).size());
        s.apply_xor_key(key.c_str(), key.size());
        str = s.base64_encode(0, recents.at(i).size());
        if (str.size() > 0) {
            ini += "file" + to_string(i) + "=" + str + "\r\n";
            }
        
        }
    s.close();
    if (ini.size() > inilen) {
        bytestream t;
        t.writeFile("config.ini", 0, ini.size(), (char*)ini.c_str());
        t.close();
        }
    
    }

void database::close () {

    if (db != nullptr) {
        sqlite3_close(db);
        //delete db;
        db = nullptr;
        }
#if !defined(FUCKSQL)

    save_recents_as_ini();

#endif
    }

/*
    copies data from database
*/
void database::load (image_data &img) {

    if (db == nullptr) {
        std::cout << "db isn't open?" << std::endl;
        return;
        }

    // check that the hash isn't a dummy / empty
    if (img.md5 == "" || img.md5 == "3B5D3C7D207E37DCEEEDD301E35E2E58") {
        std::cout << "invalid hash id" << std::endl;
        return;
        }

    // get Image ID and Meta Count
    std::vector<std::string> image_id_tmp = fetch(
       "image", "hash", img.md5, std::vector<std::string>{"id", "num_tags", "num_regions", "comment"}, "IIT"
       );

//    std::cout << "image_id_tmp: \t" << image_id_tmp.size() << std::endl;
//    std::cout << "image_id_tmp: \t" << image_id_tmp.at(0) << std::endl;
//    std::cout << "image_id_tmp: \t" << image_id_tmp.at(1) << std::endl;
//    std::cout << "image_id_tmp: \t" << image_id_tmp.at(2) << std::endl;
    if (image_id_tmp.size() < 3) {
        std::cout << "no entry in database" << std::endl;
        return;
        }

    // check that the image id isn't null
    std::string image_id_str = image_id_tmp.at(0);
    if (image_id_str == "0") {
        std::cout << "image_id invalid" << std::endl;
        return;
        }


    // Update Comment
    //img.comment  = image_id_tmp.at(2);
    unsigned int num_tags = convert_to<unsigned int>(image_id_tmp.at(1));
    unsigned int num_regions = convert_to<unsigned int>(image_id_tmp.at(2));

    // Get tags that match the image
    std::vector<std::string> tagdb = fetch( // holds tag ids, that are used to get the actual tags
        "meta",
        "image",
        image_id_str,
        std::vector<std::string>{"tag"},
        "I",
        num_tags
        );

    // Get regions that match the image
    std::vector<std::string> regiondb = fetch(
        "region",
        "image",
        image_id_str,
        std::vector<std::string>{"tag", "flag", "posx", "posy", "width", "height"},
        "TIIIII",
        num_regions
        );

    // create a array of the tag ids, ids must be unique in the array
    std::vector<std::string> unique_tagids;
    unsigned int num_tags_in_db =  tagdb.size();
    for (unsigned int i = 0; i < num_tags_in_db; i++) {
        appendIfUnique<std::string>(unique_tagids, tagdb.at(i));
        }

    num_tags_in_db = regiondb.size() / 6;
    for (unsigned int i = 0; i < num_tags_in_db; i++) {
        appendIfUnique<std::string>(unique_tagids, regiondb.at(i * 6));
        }

    // create a string of the tag ids from the array
    std::string tag_ids = arrayToSequence(unique_tagids);

    // create a map from the db tag ids to the unique array
    std::vector<unsigned int> regiontag_indexmap;
    if (num_tags_in_db > 0) {
        regiontag_indexmap = std::vector<unsigned int>(num_tags_in_db);
        for (unsigned int i = 0; i < num_tags_in_db; i++) {
            regiontag_indexmap.at(i) = findItem<std::string>(unique_tagids, regiondb.at(i * 6));
            }
        }

    // count number of tag id's
    unsigned int num_tag_id = unique_tagids.size(); //(unsigned int)std::count_if( tag_ids.begin(), tag_ids.end(), []( char c ){return c ==',';}) + 1; // get count from counting commas in tagid string

    //std::cout << "tagID num_tag_id: \t" << num_tag_id << std::endl;

    //std::cout << "tag_ids   (" << num_tag_id << "): \t" << tag_ids << std::endl;
    //std::cout << "region_ids(" << num_region_id << "): \t" << region_ids << std::endl;

    // Grab Tags from DB
    std::vector<std::string> tag_db_data;
    std::vector<std::string> tags_from_db;
    if (tag_ids.size() > 0) {
        tag_db_data = fetch("tag", "id", tag_ids, std::vector<std::string>{"name"}, "T", num_tag_id);
        unsigned int num_tag_db_data = (unsigned int)tag_db_data.size();

        // copy tag data to local array
        //std::cout << "tag components: \t" << num_tag_db_data << std::endl;
        for (unsigned int i = 0; i < num_tag_db_data; i++) {
            tags_from_db.push_back(tag_db_data.at(i));
            }
        }

    // add regions to local database
    if (regiondb.size() > 0) {

        unsigned int num_region_db_data = regiondb.size() / 6;

        unsigned int region_index;
        std::string region_tag;
        for (unsigned int i = 0; i < num_region_db_data; i++) {

            region_tag = "";
            region_index = i * 6;
            if (regiondb.at(region_index) != "0") {
                region_tag = tags_from_db.at(regiontag_indexmap.at(i));
                }
            img.region.push_back(
                region_data(
                    convert_to<signed int>(regiondb.at(region_index + 2)),
                    convert_to<signed int>(regiondb.at(region_index + 3)),
                    convert_to<signed int>(regiondb.at(region_index + 4)),
                    convert_to<signed int>(regiondb.at(region_index + 5)),
                    region_tag
                    )
                );
            }
        }


    // add the image
    image.push_back(img);

    }



/*
    dumps data back into the database
*/
void database::unload () {
    // IMAGE->META->REGION->TAG

    // dump recents files
    unsigned int num_recents = recents.size();
    if (num_recents > 0) {
        unsigned int num_dbrecents = rows("recents");
        begin();
        if (num_dbrecents < 10) {
            unsigned int num_dbrecents_to_add = 10 - num_dbrecents;
            for (unsigned int i = 0; i < num_dbrecents_to_add; i++) {
                insert("recents", std::vector<std::string>{"filename"}, std::vector<std::string>{""});
                }
            }

        for (unsigned int i = 0; i < 10; i++) {
            if (num_recents > 0) {
                modify("recents", "id", to_string(i + 1), std::vector<std::string>{"filename"}, std::vector<std::string>{recents.at(--num_recents)});
                } else {break;}
            }

        end();
        }

    unsigned int num_images = (unsigned int)image.size();
    if (num_images > 0) {

        // Build Hash Request
        std::string hash_request = "";
        for (unsigned int i = 0; i < num_images; i++) {
            if (hash_request.size() > 0) {hash_request += ", ";}
            hash_request += "'" + image.at(i).md5 + "'";
            }

        // Fetch Image details
        std::vector<std::string> image_db = fetch("image", "hash", hash_request, std::vector<std::string>{"hash", "id", "num_regions", "num_tags"}, "TIII", num_images);

        // Delete previous region and tag data
        unsigned int num_tags = 0;
        unsigned int num_regions = 0;
        std::string imgid_request = "";
        unsigned int num_dbimages = 0;
        unsigned char image_db_id = 1;
        unsigned char image_db_num_reg = 2;
        unsigned char image_db_num_tag = 3;
        unsigned char image_db_div = 4;
        if (image_db.size() > 0) {
            num_dbimages = image_db.size() / image_db_div;

            // build image id request
            for (unsigned int i = 0; i < num_dbimages; i++) {
                if (imgid_request.size() > 0) {imgid_request += ", ";}
                imgid_request += image_db.at((i * image_db_div) + image_db_id);
                num_regions += convert_to<unsigned int>(image_db.at((i * image_db_div) + image_db_num_reg));
                num_tags += convert_to<unsigned int>(image_db.at((i * image_db_div) + image_db_num_tag));
                }

            // delete previous regions and tags for the given image id's
            if (imgid_request.size() > 0) {
                begin();
                if (imgid_request.find(",")) {

                    }
                query(
                    "DELETE FROM region WHERE image IN (SELECT image FROM region WHERE image = '" +
                    imgid_request + "' LIMIT " + to_string(num_regions) + ");"
                    );
                query(
                    "DELETE FROM meta WHERE image IN (SELECT image FROM meta WHERE image = '" +
                    imgid_request + "' LIMIT " + to_string(num_tags) + ");"
                    );

                end();
                }

            }

        // Add Images
        begin();
        bool image_to_be_added;
        for (unsigned int i = 0; i < num_images; i++) {
            image_to_be_added = true;
            for (unsigned int j = 0; j < num_dbimages; j++) {
                if (image.at(i).md5 == image_db.at(j * image_db_div)) {
                    // Update Counts
                    modify(
                        "image",
                        "hash",
                        image.at(i).md5, std::vector<std::string>{"num_regions", "num_tags"},
                        std::vector<std::string>{to_string(image.at(i).region.size()), to_string(image.at(i).tag.size())}
                        );
                    image_to_be_added = false;
                    break;
                    }
                }
            if (image_to_be_added) {
                if (image.at(i).md5.size() > 0 || image.at(i).md5 != "3B5D3C7D207E37DCEEEDD301E35E2E58") {
                    insert(
                        "image",
                        std::vector<std::string> {"hash", "fullpath", "comment", "width", "height", "num_regions", "num_tags"},
                        std::vector<std::string> {
                            image.at(i).md5,
                            image.at(i).fullpath,
                            image.at(i).comment,
                            to_string(image.at(i).width),
                            to_string(image.at(i).height),
                            to_string(image.at(i).region.size()),
                            to_string(image.at(i).tag.size())
                            }
                        );
                    }
                if (hash_request.size() > 0) {hash_request += ", ";}
                hash_request += "'" + image.at(i).md5 + "'";
                }
            }
        end();

        // Collect Image ID's again
        image_db = fetch("image", "hash", hash_request, std::vector<std::string>{"id"}, "I", num_images);

        // collect tags to add
        std::vector<std::string> tag_array;
        for (unsigned int i = 0; i < num_images; i++) {
            num_tags = image.at(i).tag.size();
            num_regions = image.at(i).region.size();
            for (unsigned int j = 0; j < num_tags; j++) {
                appendIfUnique<std::string>(tag_array, image.at(i).tag.at(j).name);
                }
            for (unsigned int j = 0; j < num_regions; j++) {
                appendIfUnique<std::string>(tag_array, image.at(i).region.at(j).tag);
                }
            }

        // add tags
        num_tags = tag_array.size();
        std::string tagid_request = "";
        if (num_tags > 0) {
            begin();
            for (unsigned int i = 0; i < num_tags; i++) {
                if (tag_array.at(i).size() > 0) {
                    insert(
                        "tag",
                        std::vector<std::string> {"name", "description"},
                        std::vector<std::string> {tag_array.at(i), ""}
                        );
                    }
                if (tagid_request.size() > 0) {tagid_request += ", ";}
                tagid_request += "'" + tag_array.at(i) + "'";
                }
            end();
            }

        // get tag id's
        std::vector<std::string> tags_db;
        if (tagid_request.size() > 0) {
            tags_db = fetch("tag", "name", tagid_request, std::vector<std::string>{"id"}, "I", num_tags);
            }

        // add new regions
        signed int tag_index;
        begin();
        for (unsigned int i = 0; i < num_images; i++) {
            num_regions = image.at(i).region.size();
            for (unsigned int j = 0; j < num_regions; j++) {
                tag_index = findItem<std::string>(tag_array, image.at(i).region.at(j).tag);

                insert(
                    "region",
                    std::vector<std::string> {"posx", "posy", "width", "height", "image", "tag", "flag"},
                    std::vector<std::string> {
                        to_string(image.at(i).region.at(j).x),
                        to_string(image.at(i).region.at(j).y),
                        to_string(image.at(i).region.at(j).w),
                        to_string(image.at(i).region.at(j).h),
                        image_db.at(i), // image id
                        (tag_index > -1) ? tags_db.at((unsigned int)tag_index) : "0", //tag id
                        (image.at(i).region.at(j).add_tag_to_image) ? "1" : "0"
                        }
                    );
                }
            }

        // Link Tags
        std::string tag_link;
        for (unsigned int i = 0; i < num_images; i++) {
            num_tags = image.at(i).tag.size();
            num_regions = image.at(i).region.size();

            for (unsigned int j = 0; j < num_tags; j++) {

                tag_link = "0";
                tag_index = findItem(tag_array, image.at(i).tag.at(j).name);
                if (tag_index > -1) {
                    tag_link = tags_db.at((unsigned int)tag_index);
                    }

                insert("meta", std::vector<std::string>{"image", "tag"}, std::vector<std::string>{image_db.at(i), tag_link});
                }
            for (unsigned int j = 0; j < num_regions; j++) {
                appendIfUnique<std::string>(tag_array, image.at(i).region.at(j).tag);
                }
            }
        end();

        // flush local arrays
        for (unsigned int i = 0; i < num_images; i++) {
            image.at(i).tag.clear();
            image.at(i).region.clear();
            }
        image.clear();
        }





    }







void database::set (image_data img) {

    // kill selection
    selected = -1;

    // check that the hash isn't a dummy / empty
    if (img.md5 == "" || img.md5 == "3B5D3C7D207E37DCEEEDD301E35E2E58") {
        std::cout << "invalid hash id" << std::endl;
        return;
        }

    // retrieve image data from local database
    size_t c = image.size();
    for (unsigned int i = 0; i < c; i++) {
        if (img.md5 == image.at(i).md5) {
            selected = i;
            break;
            }
        }
    std::cout << "MD5 LOOK UP (" << selected << "): \t" << img.md5<< std::endl;

    // wasn't in local database
    if (selected < 0) {

        // check external database
        if (db == nullptr) {
            std::cout << "OPEN DB" << std::endl;
            open();
            }

        // add new entry, fill from external db if present
        selected = (signed int)image.size();
        load(img);
        image.push_back(img);
        }
    }

bool database::isSelected () {
    return (image.size() > 0 && selected > -1 && (unsigned int)selected < image.size());
    }

void database::tag (std::string tag_name, signed int index) {

    if (tag_name.size() == 0) {return;}

    tag_name = tolower(tag_name);

    // set image to load to the first element, decide later
    unsigned int image_index = 0;

    // check if a specified index was provided
    if (index > -1) {image_index = (unsigned int)index;}
    else {image_index = selected;}

    // check if a specified index was provided
    if (image.size() > 0 && image_index < image.size()) {

        size_t c = image.at(image_index).tag.size();
        if (c > 0) {
            bool found = false;
            for (unsigned int i = 0; i < c; i++) {
                if (image.at(image_index).tag.at(i).name == tag_name) {
                    found = true;
                    break;
                    }
                }
            if (!found) {image.at(image_index).tag.push_back(tag_data(tag_name));}
            } else {image.at(image_index).tag.push_back(tag_data(tag_name));}
        }
    }

void database::add (region_data reg, signed int index) {

    // set image to load to the first element, decide later
    unsigned int image_index = 0;

    // check if a specified index was provided
    if (index > -1) {image_index = (unsigned int)index;}
    else {image_index = selected;}

    // check if a specified index was provided
    if (image.size() > 0 && image_index < image.size()) {
        image.at(image_index).region.push_back(reg);
        tag(reg.tag, image_index);
        }
    }

void database::remove (unsigned int region_index, signed int index) {

    // set image to load to the first element, decide later
    unsigned int image_index = 0;

    // check if a specified index was provided
    if (index > -1) {image_index = (unsigned int)index;}
    else {image_index = selected;}

    // check if a specified index was provided
    if (image.size() > 0 && image_index < image.size()) {

        if (image.at(image_index).region.size() > 0 && region_index < image.at(image_index).region.size()) {
            image.at(image_index).region.erase(image.at(image_index).region.begin() + region_index);

            //deleteItem<std::vector<region_data>>(image.at(image_index).region, region_index);
            }
        }
    }

void database::edit (unsigned int region_index, unsigned int posx, unsigned int posy, unsigned int width, unsigned int height, signed int index) {

    // set image to load to the first element, decide later
    unsigned int image_index = 0;

    // check if a specified index was provided
    if (index > -1) {image_index = (unsigned int)index;}
    else {image_index = selected;}

    // check if a specified index was provided
    if (image.size() > 0 && image_index < image.size()) {
        if (image.at(image_index).region.size() > 0 && region_index < image.at(image_index).region.size()) {
            //if (tag.size() > 0) {image.at(image_index).region.at(region_index).tag = tag;}
            if (posx > 0)       {image.at(image_index).region.at(region_index).x = posx;}
            if (posy > 0)       {image.at(image_index).region.at(region_index).y = posy;}
            if (width > 0)      {image.at(image_index).region.at(region_index).w = width;}
            if (height > 0)     {image.at(image_index).region.at(region_index).h = height;}
            }
        }
    }

std::string database::name (unsigned int index) {
    if (isSelected() && image.at(selected).region.size() > 0 && index < image.at(selected).region.size()) {
        return image.at(selected).region.at(index).tag;
        }
    return "";
    }

signed int database::posx (unsigned int index) {
    if (isSelected() && image.at(selected).region.size() > 0 && index < image.at(selected).region.size()) {
        return image.at(selected).region.at(index).x;
        }
    return 0;
    }

signed int database::posy (unsigned int index) {
    if (isSelected() && image.at(selected).region.size() > 0 && index < image.at(selected).region.size()) {
        return image.at(selected).region.at(index).y;
        }
    return 0;
    }

signed int database::width (unsigned int index) {
    if (isSelected() && image.at(selected).region.size() > 0 && index < image.at(selected).region.size()) {
        return image.at(selected).region.at(index).w;
        }
    return 0;
    }

signed int database::height (unsigned int index) {
    if (isSelected() && image.at(selected).region.size() > 0 && index < image.at(selected).region.size()) {
        return image.at(selected).region.at(index).h;
        }
    return 0;
    }

unsigned int database::count () {
    if (isSelected()) {
        return (unsigned int)image.at(selected).region.size();
        }
    return 0;
    }

void database::reset () {
    if (!isSelected()) {return;}
    image.at(selected).region.clear();
    }

bool database::loadCSV (std::string file, signed int index, bool verbose) {

    // set return status
    bool result = false;

    // set image to load to the first element, decide later
    unsigned int image_index = 0;

    // check if a specified index was provided
    if (index > -1) {image_index = (unsigned int)index;}
    else {image_index = selected;}

    // checked image index is valid
    if ((unsigned int)image_index < image.size()) {

        // open file
        bytestream f;
        if (f.openFileW(utf8_2_unicode(file))) {

            // read each line of the text
            std::vector<std::string> txt;
            std::string str;
            while (!f.eos()) {
                str = trim(f.readline());
                if (str.size() > 0) {
                    txt.push_back(str);
                    }
                }

            // check that txt isn't empty
            unsigned int num_rows = txt.size();
            if (num_rows > 0 && txt.at(0).size() > 0) {

                // check for header
                std::string delimiter = ",";
                std::vector<std::string> header = split(txt.at(0), delimiter);

                // if no header, check the file isn't using a different seperator character
                unsigned int num_cols = header.size();
                if (num_cols == 0) {
                    delimiter = "\t";
                    header = split(txt.at(0), delimiter);
                    num_cols = header.size();
                    }

                // correct for header if present
                unsigned int name_col = 0;
                unsigned int posx_col = 1;
                unsigned int posy_col = 2;
                unsigned int dimx_col = 3;
                unsigned int dimy_col = 4;
                bool hasHeader = false;
                for (unsigned int i = 0; i < num_cols; i++) {
                    // lowercase
                    header.at(i) = trim(tolower(header.at(i)), " ");

                    // search and collect index for each col
                    if      (header.at(i).find("name")   == 0) {name_col = i; hasHeader = true;}
                    else if (header.at(i).find("posx")   == 0) {posx_col = i; hasHeader = true;}
                    else if (header.at(i).find("posy")   == 0) {posy_col = i; hasHeader = true;}
                    else if (header.at(i).find("width")  == 0) {dimx_col = i; hasHeader = true;}
                    else if (header.at(i).find("height") == 0) {dimy_col = i; hasHeader = true;}
                    }

                // set starting row
                unsigned int row_start = 0;
                if (hasHeader) {row_start = 1;}

                // check that theres rows to read
                unsigned int num_regions = num_rows - row_start;
                if (num_regions > 0) {

                    // set return status to true
                    result = true;

                    // clear existing region data
                    image.at(image_index).region.clear();
                    image.at(image_index).region = std::vector<region_data>(num_regions);

                    // parse each line of the csv
                    std::vector<std::string> tmp;

                    unsigned int x;
                    for (unsigned int i = 0; i < num_regions; i++) {

                        // copy string
                        str = txt.at(row_start + i);


                        // parse out values
                        while (str.size() > 0) {


                            // check if there is a quoted value
                            if (str.substr(0, 1) == "\"") {
                                // trim out the first character
                                str = str.substr(1, str.size() - 1);

                                // check for trailing quote
                                x = str.find("\"");
                                if (x != std::string::npos && x > 0) {

                                    // add value to the temp array
                                    tmp.push_back(str.substr(0, x));

                                    // remove from string
                                    str.erase(0, x);
                                    x = str.find(delimiter);
                                    if (x != std::string::npos) {
                                        str.erase(0, x + 1);
                                        } else {str.erase(0, str.size());}
                                    }
                                else {
                                    x = str.find(delimiter);
                                    if (x != std::string::npos) {
                                        str.erase(0, x + 1);
                                        } else {str.erase(0, str.size());}
                                    }
                                }

                            // check if there is a delimiter character
                            else if ((x = str.find(delimiter)) != std::string::npos) {

                                if (x > 0) {
                                    // add value to the temp array



                                    tmp.push_back(str.substr(0, x));
                                    str.erase(0, x + 1);

                                    }
                                else {
                                    // empty
                                    tmp.push_back("");
                                    str.erase(0, 1);

                                    }

                                }

                            // just copy to the end of the string
                            else {

                                tmp.push_back(str.substr(0, str.size()));
                                str.erase(0, str.size());
                                str.clear();
                                }

                            // trim whitespace
                            str = trim(str);

                            }


                        // check that temp array is valid
                        x = tmp.size();
                        if (verbose) {
                            std::cout << "elements on row: \t" << x << std::endl;
                            std::cout << "Entry: \t{";
                            for (unsigned int j = 0; j < x; j++) {
                                std::cout << "(" << tmp.at(j) + ")";
                                if (j + 1 < x) {std::cout << ", ";}
                                }
                            std::cout << "}" << std::endl;
                            }

                        if (x > 0) {
                            // update region data
                            if (name_col < x) {image.at(image_index).region.at(i).tag = tmp.at(name_col);}
                            if (posx_col < x) {image.at(image_index).region.at(i).x   = convert_to<signed int>(tmp.at(posx_col));}
                            if (posy_col < x) {image.at(image_index).region.at(i).y   = convert_to<signed int>(tmp.at(posy_col));}
                            if (dimx_col < x) {image.at(image_index).region.at(i).w   = convert_to<signed int>(tmp.at(dimx_col));}
                            if (dimy_col < x) {image.at(image_index).region.at(i).h   = convert_to<signed int>(tmp.at(dimy_col));}
                            } else {if (verbose) {std::cout << "Error: \tRow" << i << " was empty\n";}}

                        // clear temp array
                        tmp.clear();
                        }
                    }
                }

            // close file
            f.close();
            }
        }

    return result;
    }

bool database::writeCSV (std::string file, signed int index) {

    // set image to load to the first element, decide later
    unsigned int image_index = 0;

    // check if a specified index was provided
    if (index > -1) {image_index = (unsigned int)index;}
    else {image_index = selected;}


    // std::cout << "image_index: \t" << image_index << std::endl;

    // checked image index is valid
    if (image_index < image.size()) {

        // compile csv text
        unsigned int num_regions = image.at(image_index).region.size();
        //std::cout << "num_regions: \t" << num_regions << std::endl;
        if (num_regions > 0) {
            std::string csv = "name,posx,posy,width,height\r\n";
            for (unsigned int i = 0; i < num_regions; i++) {
                csv += "\"" + image.at(image_index).region.at(i).tag + "\",";
                csv += to_string(image.at(image_index).region.at(i).x) + ",";
                csv += to_string(image.at(image_index).region.at(i).y) + ",";
                csv += to_string(image.at(image_index).region.at(i).w) + ",";
                csv += to_string(image.at(image_index).region.at(i).h) + "\r\n";
                }

            //std::cout << "csv: \t" << csv << std::endl;
            // write file
            bytestream s;
            //std::cout << "file: \t" << file << std::endl;
            s.writeFileW(utf8_2_unicode(file), 0, csv.size(), (char*)csv.c_str());
            s.close();
            // send pass status
            return true;
            }
        }
    return false;
    }
