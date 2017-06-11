#include "bone-database.h"

namespace BONE {

static bool flag_succeed= false;	// flag for query function know if query is succeed of not. 
// static stats_t stats= {0};			// count the total query command.

// static void command_started (const mongoc_apm_command_started_t *event);	// show start time of command
static void command_excuted (const mongoc_apm_command_succeeded_t *event); // it was called if the query command succeeded
static void command_failed (const mongoc_apm_command_failed_t *event); // it was called if the query command failed

// this function will turn on flag_succeed if "n"/"ok" : 1 exist in string s and 
// if "n"/"ok" : 0 will turn off flag_succeed
// the issue :  command_excuted was static function. and so turnFlag
// must be static function.
static bool turnFlag(char *s) {
	if(s[strrchr(s,'}')-s-2]-'0')	// get value of "ok" : 1
		flag_succeed = true;
	else
		flag_succeed = false;
	return flag_succeed;
}

static void command_excuted (const mongoc_apm_command_succeeded_t *event) {
	char *s;
	char *e = strdup(mongoc_apm_command_succeeded_get_command_name (event));
	s = bson_as_json (mongoc_apm_command_succeeded_get_reply (event), NULL);
	if(turnFlag(s)) {
		// if( strcmp(e,"insert")==0 || strcmp(e,"update")==0 || strcmp(e,"delete")==0 )
			// BONE_VLOG << "Command " << mongoc_apm_command_succeeded_get_command_name (event)
				// << " succeeded\n" << "\n";
		// else
			// BONE_VLOG << "Command " << mongoc_apm_command_succeeded_get_command_name (event)
			// 	<< " succeeded:\n" << "\n";
	} else {
		BONE_WARN << "Command " << mongoc_apm_command_succeeded_get_command_name (event)
			<< " missed:\n" << s << "\n";
	}
   bson_free (s);
   free(e);
}

static void command_failed (const mongoc_apm_command_failed_t *event) {

   bson_error_t error;
   mongoc_apm_command_failed_get_error (event, &error);
   BONE_WARN <<"Command "<< mongoc_apm_command_failed_get_command_name (event)
   				<< " failed:" << error.message << "\n";
}

Database::Database() {}

Database::Database(const char *databaseName) {

	dbName = databaseName;
	bson_t *command, reply;
	bson_error_t error;
	mongoc_apm_callbacks_t *callbacks;

	mongoc_init ();
	// Create a new client
	client = mongoc_client_new ("mongodb://127.0.0.1/");

	mongoc_client_set_error_api (client, 2);
   	callbacks = mongoc_apm_callbacks_new ();

	// mongoc_apm_set_command_started_cb (callbacks, command_started);	// really need more debug.
	mongoc_apm_set_command_succeeded_cb (callbacks, command_excuted);
	mongoc_apm_set_command_failed_cb (callbacks, command_failed);
	mongoc_client_set_apm_callbacks (client, callbacks, NULL);

	dtb = mongoc_client_get_database (client, dbName);
	command = BCON_NEW ("ping", BCON_INT32 (1));
	if (!mongoc_client_command_simple (client, "admin", command, NULL, &reply, &error)) {
		BONE_ERROR << error.message << "\n\n" << "[Suggestion] Lets try turn on mongo: $ sudo service mongod start";
	}

	bson_destroy (command);
	BONE_VLOG << "Database \""<< databaseName << "\" has connected successful !";
}

Database::~Database() {
	mongoc_database_destroy (dtb);
	mongoc_client_destroy (client);
	mongoc_cleanup ();
}

void Database::begin(const char *databaseName) {
	dbName = databaseName;
	bson_t *command, reply;
	bson_error_t error;
	mongoc_apm_callbacks_t *callbacks;

	mongoc_init ();
	// Create a new client
	client = mongoc_client_new ("mongodb://127.0.0.1/");

	mongoc_client_set_error_api (client, 2);
   	callbacks = mongoc_apm_callbacks_new ();

	// mongoc_apm_set_command_started_cb (callbacks, command_started);	// really need more debug.
	mongoc_apm_set_command_succeeded_cb (callbacks, command_excuted);
	mongoc_apm_set_command_failed_cb (callbacks, command_failed);
	mongoc_client_set_apm_callbacks (client, callbacks, NULL);

	dtb = mongoc_client_get_database (client, dbName);
	command = BCON_NEW ("ping", BCON_INT32 (1));
	if (!mongoc_client_command_simple (client, "admin", command, NULL, &reply, &error)) {
		BONE_ERROR << error.message << "\n\n" << "[Suggestion] Lets try turn on mongo: $ sudo service mongod start";
	}

	bson_destroy (command);
	BONE_VLOG << "Database \""<< databaseName << "\" has connected successful !";
}

int Database::insertQuick(const char *COLL_NAME, const bson_t *insert) {

	db_collection *colt = mongoc_client_get_collection (client, dbName, COLL_NAME);
	bson_error_t error;
	if( !mongoc_collection_insert (colt, MONGOC_INSERT_NONE, insert, NULL, &error)) {
		BONE_WARN << error.message;
		return RET_FAILURE;
	} else {
		char *json = bson_as_json(insert, NULL);
		BONE_VLOG << "[insert -> "<< COLL_NAME <<"] " << json;
		bson_free(json);
		return RET_SUCCESS;
	}
}

int Database::insert(const char *COLL_NAME, const bson_t *insert) {
	return insertQuick(COLL_NAME, insert);
}

int Database::getNextSequence(db_collection *colt) {
	int count;
   	bson_error_t error;
	if ( (count = mongoc_collection_count (colt, MONGOC_QUERY_NONE, NULL, 0, 0, NULL, &error)) < 0) {
		BONE_WARN << error.message;
		return -1;
	} else {
		bson_t *query = BCON_NEW ("_id", "ob_id");
		bson_t *update = BCON_NEW ("$set", "{", "total", BCON_INT32(count), "}");
		bool res = mongoc_collection_update (colt, MONGOC_UPDATE_NONE, query, update, NULL, &error);
		bson_destroy(query);
		bson_destroy(update);
		if(res)
			return (count-1); // I dont wanna count the first document (its information for collection).
		else
			return -1;
	}
}

int Database::insertInOrder(const char *COLL_NAME, const bson_t *insert) {

	db_collection *colt = mongoc_client_get_collection (client, dbName, COLL_NAME);
	int total = getNextSequence(colt);  // I dont wanna count the first document (its information for collection).
	if(total==-1) {
		mongoc_collection_destroy(colt);
		return RET_FAILURE;
	}
	bson_t *command = BCON_NEW ("_id", BCON_INT32(total));
	bson_concat(command, insert);
	int res = insertQuick(COLL_NAME, command);
	bson_destroy(command);
	mongoc_collection_destroy(colt);

	return res;
}

int Database::insertInOrderWithDateTime(const char *COLL_NAME, const bson_t *insert) {
	
	bson_t * command = bson_new();
	bson_concat(command, insert);
	time_t rawtime = time(NULL);
	time_t localTime = mktime(localtime(&rawtime)) + GMT*3600;
	bson_append_time_t (command, "date", -1, localTime);
	int res = insertInOrder(COLL_NAME, command);
	bson_destroy(command);
	return res;
}

int Database::update(const char *COLL_NAME, const char *key_select, const char *val_select, const char *key_update, bool val_update) {
	bson_t *update = BCON_NEW ("$set", "{", key_update, BCON_BOOL(val_update), "}");
	int res = updateQuick(COLL_NAME, key_select, val_select, update);
	bson_destroy (update);
	return res;
}

int Database::update(const char *COLL_NAME, const char *key_select, const char *val_select, const char *key_update, int32_t val_update) {
	bson_t *update = BCON_NEW ("$set", "{", key_update, BCON_INT32(val_update), "}");
	int res = updateQuick(COLL_NAME, key_select, val_select, update);
	bson_destroy (update);
	return res;
}

int Database::update(const char *COLL_NAME, const char *key_select, const char *val_select, const char *key_update, int64_t val_update) {
	bson_t *update = BCON_NEW ("$set", "{", key_update, BCON_INT64(val_update), "}");
	int res = updateQuick(COLL_NAME, key_select, val_select, update);
	bson_destroy (update);
	return res;
}

int Database::update(const char *COLL_NAME, const char *key_select, const char *val_select, const char *key_update, double val_update) {
	bson_t *update = BCON_NEW ("$set", "{", key_update, BCON_DOUBLE(val_update), "}");
	int res = updateQuick(COLL_NAME, key_select, val_select, update);
	bson_destroy (update);
	return res;
}

int Database::update(const char *COLL_NAME, const char *key_select, const char *val_select, const char *key_update, int val_update[], unsigned int length_arr) {
	bson_t child;
    bson_t child2;
    bson_t *update = bson_new ();

    BSON_APPEND_DOCUMENT_BEGIN (update, "$set", &child);
    BSON_APPEND_ARRAY_BEGIN (&child, key_update, &child2);
    for (unsigned int i = 0; i < length_arr; ++i)
    	BSON_APPEND_INT32(&child2, "", val_update[i]);
    bson_append_array_end (&child, &child2);
    bson_append_document_end (update, &child);    

    int res = updateQuick(COLL_NAME, key_select, val_select, update);
	bson_destroy(update);
	return res;
}

int Database::update(const char *COLL_NAME, const char *key_select, const char *val_select, const char *key_update, std::vector<int> val_update) {
	bson_t child;
    bson_t child2;
    bson_t *update = bson_new ();

    BSON_APPEND_DOCUMENT_BEGIN (update, "$set", &child);
    BSON_APPEND_ARRAY_BEGIN (&child, key_update, &child2);
    for (unsigned int i = 0; i < val_update.size(); ++i)
    	BSON_APPEND_INT32(&child2, "", val_update[i]);
    bson_append_array_end (&child, &child2);
    bson_append_document_end (update, &child);    

    int res = updateQuick(COLL_NAME, key_select, val_select, update);
	bson_destroy(update);
	return res;
}

int Database::updateQuick(const char *COLL_NAME, const char *key_select, const char *val_select, bson_t *update) {
	bson_t *query = BCON_NEW(key_select, val_select);
	bool res = updateQuick(COLL_NAME, query, update);
	bson_destroy(query);
	return res;
}

int Database::updateQuick(const char *COLL_NAME, bson_t *query, bson_t *update) {
	bson_error_t error;
	db_collection *colt = mongoc_client_get_collection (client, dbName, COLL_NAME);
	if( !mongoc_collection_update (colt, MONGOC_UPDATE_NONE, query, update, NULL, &error)) {	
		mongoc_collection_destroy (colt);
		return RET_FAILURE;
	} else {
		BONE_VLOG << "[update -> "<< COLL_NAME <<"] " << bson_as_json (query, NULL) << "," << bson_as_json(update, NULL);
		mongoc_collection_destroy (colt);
		return RET_SUCCESS;
	}
}

int Database::remove(const char *COLL_NAME, const bson_t *query){

	bson_error_t error;
	db_collection *colt = mongoc_client_get_collection (client, dbName, COLL_NAME);

	bool res = mongoc_collection_remove (colt, MONGOC_REMOVE_SINGLE_REMOVE, query, NULL, &error);
    mongoc_collection_destroy (colt);
    if(res) {
		BONE_VLOG << "[remove -> "<< COLL_NAME <<"] " << bson_as_json(query, NULL);
    	return RET_FAILURE;
    } else {
    	return RET_FAILURE;
    }
}

int32_t Database::totalDocuments (const char *COLL_NAME) {

   	int64_t count;
   	bson_error_t error;
   	db_collection *colt = mongoc_client_get_collection (client, dbName, COLL_NAME);

	if ( (count = mongoc_collection_count (colt, MONGOC_QUERY_NONE, NULL, 0, 0, NULL, &error)) < 0) {
		mongoc_collection_destroy (colt);
		BONE_WARN << error.message;
		return 0;
	} else {
		mongoc_collection_destroy (colt);
		return count;
	}
}

int Database::initOrder(const char *COLL_NAME) {
	bson_t *init = BCON_NEW("_id", "ob_id", "total", BCON_INT32(0));
	bool res = insertQuick(COLL_NAME, init);
	bson_destroy(init);
	return res;
}

std::string Database::readDocument(const char *COLL_NAME, char *jsonSelecColl) {

	bson_t *query;
	if(!(query = bson_new_from_json ((const uint8_t*)jsonSelecColl, -1, NULL))) {
		bson_destroy (query);
		return std::string(); // or return {}
	}
	return readDocument(COLL_NAME, query);
}

std::string Database::readDocument(const char *COLL_NAME, bson_t *query) {

	const bson_t* doc;
	db_collection *colt = mongoc_client_get_collection (client, dbName, COLL_NAME);
	mongoc_cursor_t *cursor = mongoc_collection_find_with_opts (colt, query, NULL, NULL);
	if(mongoc_cursor_next (cursor, &doc)==0) {
		mongoc_cursor_destroy(cursor);
		return std::string(); // or return {}
	}
	std::string resJson(bson_as_json(doc, NULL));
	mongoc_cursor_destroy(cursor);
	mongoc_collection_destroy(colt);
	return resJson;
}

bson_t * Database::readDocument(const char *COLL_NAME, const bson_t *query) {

	const bson_t* doc;
	db_collection *colt = mongoc_client_get_collection (client, dbName, COLL_NAME);
	mongoc_cursor_t *cursor = mongoc_collection_find_with_opts (colt, query, NULL, NULL);
	if(mongoc_cursor_next (cursor, &doc)==0) {
		mongoc_cursor_destroy(cursor);
		return NULL; // or return {}
	}
	mongoc_cursor_destroy(cursor);
	mongoc_collection_destroy(colt);
	return (bson_t *)doc;
}

}