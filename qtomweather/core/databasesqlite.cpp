#include "databasesqlite.h"

namespace Core {

DatabaseSqlite::DatabaseSqlite(const std::string& filename)
{
    db = NULL;
    databasename = new std::string;
    databasename->assign(filename);
}

DatabaseSqlite::~DatabaseSqlite()
{
    std::cerr << "destructor = " << *databasename << std::endl;
    delete databasename;
    if (!db) sqlite3_close(db);
}
void
DatabaseSqlite::set_databasename(const std::string& filename)
{
    databasename->assign(filename);

}

bool
DatabaseSqlite::open_database()
{
    std::string lang = std::string();
    int rc;
    char * msg;
    std::string key ("gismeteo.ru.db");
    std::cerr << (databasename->length() - databasename->rfind(key)) << " " << key.length() << std::endl;
    if (sqlite3_open(databasename->c_str(), &db)){
        std::cerr << "error open " << *databasename << std::endl;
        return false;
    }
    if (databasename->rfind(key) != std::string::npos && (databasename->length() - databasename->rfind(key)) == key.length()) {
        getenv("LC_ALL");
        if(!lang.empty()){
            lang = getenv("LC_MESSAGES");
            if(!lang.empty())
                lang = getenv("LANG");
        }

        if (!lang.empty() && !lang.compare("ru_RU"))
            rc = sqlite3_exec(db, "CREATE TEMP VIEW nstations AS SELECT russian_name as name, id, region_id, longititude, latitude, code, id_gismeteo_new, id_gismeteo_old  FROM stations",
                  NULL, NULL, &msg);
        else
            rc = sqlite3_exec(db, "CREATE TEMP VIEW nstations AS SELECT * FROM stations where russian_name != name",
                  NULL, NULL, &msg);
    }else {
        rc = sqlite3_exec(db, "CREATE TEMP VIEW nstations AS SELECT * FROM stations",
                  NULL, NULL, &msg);
    }
    if(rc != SQLITE_OK){
        std::cerr << "Problem in sqlite3_exec " << rc << " - " << msg << std::endl;
        return false;
    }
    return true;
}
listdata*
DatabaseSqlite::create_countries_list()
{
    listdata *list = NULL;
    int rc;
    char *errMsg = NULL;
    char **result;
    int nrow, ncol;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    if(!db)
        return NULL;    /* database doesn't open */
    list = new listdata;
    rc = sqlite3_get_table(db,
                           "SELECT id, name FROM countries where (select count(name) from nstations where nstations.region_id = (select distinct regions.id from regions where regions.country_id=countries.id )) >0 ORDER BY name",
                           &result,
                           &nrow,
                           &ncol,
                           &errMsg);
    if(rc != SQLITE_OK){
#ifndef RELEASE
        std::cerr << errMsg << std::endl;
#endif
        sqlite3_free(errMsg);
        return NULL;
    }
    for (int i=0; i<ncol*nrow; i=i+2)
        list->push_back(std::make_pair(result[ncol+i], result[ncol+i+1]));

    sqlite3_free_table(result);

#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
    return list;
}

listdata*
DatabaseSqlite::create_region_list(int country_id)
{
    listdata *list = NULL;
    int rc;
    char *errMsg = NULL;
    char **result;
    int nrow, ncol;
    //std::string sql;
    char sql[256];
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    list = new listdata;
    if(!db){
        return NULL;
    }

    if (country_id == 0 || country_id == -1) return list;

    if(country_id == 0) /* for GPS */
        snprintf(sql, sizeof(sql) - 1,
                 "SELECT id, name FROM regions");
    else {

        snprintf(sql,
                 sizeof(sql) - 1,
                 "SELECT id, name FROM regions WHERE country_id = %d ORDER BY name",
                 country_id);
    }
    std::cerr << sql << std::endl;
    rc = sqlite3_get_table(db,
                           sql,
                           &result,
                           &nrow,
                           &ncol,
                           &errMsg);
    if(rc != SQLITE_OK){
#ifndef RELEASE
        std::cerr << errMsg << std::endl;
#endif
        sqlite3_free(errMsg);
        return NULL;
    }
    for (int i=0; i<ncol*nrow; i=i+2)
        list->push_back(std::make_pair(result[ncol+i], result[ncol+i+1]));

    sqlite3_free_table(result);

#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
    return list;
}

listdata*
DatabaseSqlite::create_stations_list(int region_id)
{
    listdata *list = NULL;
    int rc;
    char *errMsg = NULL;
    char **result;
    int nrow, ncol;
    //std::string sql;
    char sql[256];
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    std::cerr << "region id = " << region_id << std::endl;
    list = new listdata;
    if(!db || region_id == 0 || region_id == -1)
        return list;    /* database doesn't open */


    snprintf(sql, sizeof(sql) - 1,
        "SELECT code, name FROM nstations WHERE \
        region_id = %d ORDER BY name", region_id);
    std::cerr << sql << std::endl;
    rc = sqlite3_get_table(db,
                           sql,
                           &result,
                           &nrow,
                           &ncol,
                           &errMsg);
    if(rc != SQLITE_OK){
#ifndef RELEASE
        std::cerr << errMsg << std::endl;
#endif
        sqlite3_free(errMsg);
        return NULL;
    }
    for (int i=0; i<ncol*nrow; i=i+2){
        list->push_back(std::make_pair(result[ncol+i], result[ncol+i+1]));
        //std::cerr << result[ncol+i] << " - " << result[ncol+i+1] << std::endl;
    }
    sqlite3_free_table(result);

#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
    return list;
}

} // namespace Core
