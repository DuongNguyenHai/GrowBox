// Nguyen Hai Duong
// Date : july 2016

#include <stdio.h>      // for printf() and fprintf()
#include <string.h>
#include "box-data.h"
#include "bone-database.h"
#include "bone-log.h"
#include "bone-config.h"

using namespace BONE;

#define DATABASE_NAME "test"
// #define DATABASE_GROW_BOX "GrowBox"
#define SENSOR "sensor"

char insert[] = "{\"tempt\": 25, \"heartTempt\":35, \"state\":1, \"warning\": \"none\" }";
char *json = insert;


char *jsonSelector = (char *)"{\"heartTempt\":35}";
char *jsonUpdate = (char *)"{\"$set\":{\"heartTempt\":100}}";
char *jsonSelector2 = (char *)"{\"tempt\":25}";

int main (int   argc, char *argv[]) {

	Database dt(DATABASE_NAME);
	dt.initOrder(SENSOR);
	dt.insert(SENSOR, insert);
	dt.insertInOrder(SENSOR, insert);
	dt.update(SENSOR, jsonUpdate, jsonSelector);
	dt.remove(SENSOR, jsonSelector2);
	dt.totalDocuments(SENSOR);
	dt.displayTotalQuery();

	return 0;
}