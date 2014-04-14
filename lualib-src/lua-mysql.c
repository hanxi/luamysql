#include <lua.h>
#include <lauxlib.h>

#include <string.h>
#include <stdlib.h>

#include "mysql/mysql.h"

// return MYSQL * to lua
int _connect(lua_State* L) {
	if (lua_gettop(L) != 5) {
        return luaL_error(L, "argument error");
	}

	// init mysql
	MYSQL* mysql = mysql_init(NULL);
	if (!mysql) {
        return luaL_error(L, "init mysql error");
	}

	const char* db_addr = lua_tostring(L,1);
	const char* db_user = lua_tostring(L,2);
	const char* db_pass = lua_tostring(L,3);
	const char* db_name = lua_tostring(L,4);
	unsigned int db_port = (unsigned int)lua_tonumber(L,5);
	
	// connect mysql
	if (!mysql_real_connect(mysql,db_addr,db_user,db_pass,db_name,db_port, NULL, 0)) {
        return luaL_error(L, "failed to connect to database. mysql_err : %s\n", mysql_error(mysql));
	}

    // set auto reconnect
    char value = 1;
    mysql_options(mysql, MYSQL_OPT_RECONNECT, &value);
    mysql_set_character_set(mysql, "utf8");
	
    lua_pushlightuserdata(L,mysql);
	return 1;
}

int _disconnect(lua_State* L) {
	if (lua_gettop(L) > 0) {
		MYSQL* mysql = (MYSQL*)lua_touserdata(L,1);
		mysql_close(mysql);
	}
	
	return 0;
}

// return double dimesional lua table
int _query(lua_State* L)
{
	if (lua_gettop(L) != 2) {
        return luaL_error(L, "argument error");
	}

	MYSQL* mysql = (MYSQL*)lua_touserdata(L,1);
	const char* query_str = lua_tostring(L,2);

	if (mysql_ping(mysql) != 0) {
        return luaL_error(L, "mysql ping error");
	}

	// mysql query
	if ( mysql_real_query(mysql, query_str, strlen(query_str)) ) {
        return luaL_error(L, "mysql query error. mysql_err=%s\n",mysql_error(mysql));
	}

	// result of query
	MYSQL_RES* res = mysql_store_result(mysql);

	// fieldcount
	int fieldnum = mysql_num_fields(res);
	MYSQL_FIELD *fields = mysql_fetch_fields(res);

	MYSQL_ROW row;
	int rowno = 1;
	
	// set result to lua table
	lua_newtable(L);
	while ( (row = mysql_fetch_row(res)) ) {
		lua_pushnumber(L,rowno++);
        
		lua_newtable(L);
		for (int i = 0; i<fieldnum; i++) {
			lua_pushstring(L,fields[i].name);
            switch (fields[i].type) {
                case MYSQL_TYPE_TINY:
                case MYSQL_TYPE_SHORT:
                case MYSQL_TYPE_INT24:
                case MYSQL_TYPE_LONG:
                case MYSQL_TYPE_LONGLONG:
                    if (row[i]) {
                        lua_pushnumber(L,atol(row[i]));
                    }
                    else {
                        lua_pushnumber(L,0);
                    }
                    break;
                default:
                    lua_pushstring(L,row[i]);
                    break;
            }
			lua_settable(L,-3);
		}

		lua_settable(L,-3);
	}

	// free result
	mysql_free_result(res);

	return 1;
}

// retrun insert_id
int _insert(lua_State* L)
{
	if (lua_gettop(L) != 2) {
        return luaL_error(L, "argument error");
	}

	MYSQL* mysql = (MYSQL*)lua_touserdata(L,1);
	const char* insert_str = lua_tostring(L,2);

	if (mysql_ping(mysql) != 0) {
        return luaL_error(L, "mysql ping error");
	}

	// insert mysql
	int result = mysql_real_query(mysql, insert_str, (unsigned int)strlen(insert_str));
	if (result) {
        return luaL_error(L, "mysql insert error. mysql_err=%s\n",mysql_error(mysql));
	}

	unsigned long long insert_id = mysql_insert_id(mysql);
    lua_pushnumber(L,insert_id);
    return 1;
}

// update
int _update(lua_State* L) {
	if (lua_gettop(L) != 2) {
        return luaL_error(L, "argument error");
	}

	MYSQL* mysql = (MYSQL*)lua_touserdata(L,1);
	const char* update_str = lua_tostring(L,2);

	if (mysql_ping(mysql) != 0) {
        return luaL_error(L, "mysql ping error");
	}

	// update mysql
	int result = mysql_real_query(mysql, update_str, (unsigned int)strlen(update_str));

    if (result) {
        return luaL_error(L, "mysql update error. mysql_err=%s\n",mysql_error(mysql));
	}

    lua_pushnumber(L,0);
	return 1;
}

int _character_set(lua_State* L) {
	if (lua_gettop(L) != 2) {
        return luaL_error(L, "argument error");
	}

	MYSQL* mysql = (MYSQL*)lua_touserdata(L,1);
	const char* character_type = lua_tostring(L,2);

    mysql_set_character_set(mysql, character_type);

    lua_pushnumber(L,0);
    return 1;
}

int luaopen_mysql_driver(lua_State *L) {
    luaL_checkversion(L);
    luaL_Reg l[] ={
        { "connect", _connect },
        { "disconnect", _disconnect },
        { "query", _query },    
        { "insert", _insert },    
        { "update", _update },   
        { "character_set", _character_set },   
        { NULL, NULL },
    };

    luaL_newlib(L,l);
    return 1;
}

