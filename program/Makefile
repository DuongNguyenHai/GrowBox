CC = gcc
CXX = g++

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

DIRLIB = $(ROOT_DIR)/lib
DIRLIB_BASE = $(DIRLIB)/base
# json-c dynamic library directory
JSON_DIR = $(DIRLIB_BASE)/lib-json/cJSON

CFLAGS += -std=c++11 -Wall -g
LDFLAGS += -lwiringPi -lpthread
INCLUDES += -I$(DIRLIB) -I$(DIRLIB_BASE) `pkg-config --cflags --libs libmongoc-1.0`


# I dont know why that I have to put -lpthread in LDFLAGS but CFLAGS. If -lpthread is in CFLAGS. It will not working
# Oh. The order of command line of compiler is matter. so -lpthread must after $(OBJECT). see my note to learn more
# link dynamic library
# LIBS = -Wl,-rpath='$(JSON_DIR)/lib' -lcJSON
# link static lib.. there is some error when include static ....
# LIBS = $(JSON_DIR)/lib/libcJSON.a
# LIBS = -lcJSON
# source file
SOURCES = box.cc $(DIRLIB)/system.cc $(DIRLIB)/communicate.cc $(DIRLIB_BASE)/bone-database.cc $(DIRLIB_BASE)/TCP-IP.cc $(DIRLIB_BASE)/bone-log.cc $(DIRLIB_BASE)/bone-config.cc $(DIRLIB_BASE)/DHT22.cpp $(DIRLIB_BASE)/DS18B20.cpp $(DIRLIB_BASE)/BH1750.cpp $(DIRLIB_BASE)/OneWire.cpp $(DIRLIB_BASE)/Wire.cpp
SRC_SENSOR = sensor.cc $(DIRLIB_BASE)/DHT22.cpp $(DIRLIB_BASE)/DS18B20.cpp $(DIRLIB_BASE)/BH1750.cpp $(DIRLIB_BASE)/OneWire.cpp $(DIRLIB_BASE)/Wire.cpp
SRC_TEMPT = read-tempt.cc $(DIRLIB_BASE)/DHT22.cpp $(DIRLIB_BASE)/DS18B20.cpp $(DIRLIB_BASE)/OneWire.cpp
SRC_DATA = data.cc $(DIRLIB_BASE)/DHT22.cpp $(DIRLIB_BASE)/DS18B20.cpp $(DIRLIB_BASE)/OneWire.cpp $(DIRLIB_BASE)/Wire.cpp
SRC_DATABASE = test-database.cc $(DIRLIB_BASE)/bone-database.cc $(DIRLIB_BASE)/bone-log.cc $(DIRLIB_BASE)/bone-config.cc 
# convert name.c/name.cc/name.cpp/... to name.o
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))
OBJECTS_d = $(addsuffix .d, $(basename $(SOURCES)))
OBJ_SENSOR = $(addsuffix .o, $(basename $(SRC_SENSOR)))
OBJ_SENSOR_d = $(addsuffix .d, $(basename $(SRC_SENSOR)))
OBJ_TEMPT = $(addsuffix .o, $(basename $(SRC_TEMPT)))
OBJ_TEMPT_d = $(addsuffix .d, $(basename $(SRC_TEMPT)))
OBJ_DATA = $(addsuffix .o, $(basename $(SRC_DATA)))
OBJ_DATA_d = $(addsuffix .d, $(basename $(SRC_DATA)))
OBJ_DATABASE = $(addsuffix .o, $(basename $(SRC_DATABASE)))
OBJ_DATABASE_d = $(addsuffix .d, $(basename $(SRC_DATABASE)))
# alternative way, u can write directly name of object
# OBJECTS = seed.o seed-log.o seed-database.o

EXE = box
EXE_SENSOR = sensor
EXE_TEMPT = tempt
EXE_DATA = data
EXE_DATABASE = database

# $(EXE)
all: $(EXE)

#note : Write command in order : $(CXX) $(CFLAGS) $(OBJECTS) $(LDFLAGS) $(INCLUDES) $(LIBS) -o $(EXE)
$(EXE): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LDFLAGS) $(INCLUDES) $(LIBS) -o $(EXE)

$(EXE_SENSOR): $(OBJ_SENSOR)
	$(CXX) $(CFLAGS) $(OBJ_SENSOR) $(LDFLAGS) $(INCLUDES) -o $(EXE_SENSOR)

$(EXE_TEMPT): $(OBJ_TEMPT)
	$(CXX) $(CFLAGS) $(OBJ_TEMPT) $(LDFLAGS) $(INCLUDES) -o $(EXE_TEMPT)

$(EXE_DATA): $(OBJ_DATA)
	$(CXX) $(CFLAGS) $(OBJ_DATA) $(LDFLAGS) $(INCLUDES) -o $(EXE_DATA)

$(EXE_DATABASE): $(OBJ_DATABASE)
	$(CXX) $(CFLAGS) $(OBJ_DATABASE) $(LDFLAGS) $(INCLUDES) $(LIBS) -o $(EXE_DATABASE)

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)
-include $(OBJ_SENSOR:.o=.d)
-include $(OBJ_TEMPT:.o=.d)
-include $(OBJ_DATA:.o=.d)
-include $(OBJ_DATABASE:.o=.d)

%.o: %.cc
	$(CXX) -c $(CFLAGS) $(LDFLAGS) $(INCLUDES) $*.cc -o $*.o

	$(CXX) -MM $(CFLAGS) $(LDFLAGS) $(INCLUDES) $*.cc > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(LDFLAGS) $(INCLUDES) $*.cpp -o $*.o

	$(CXX) -MM $(CFLAGS) $(LDFLAGS) $(INCLUDES) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean-all: deep-clean deep-clean-sensor deep-clean-tempt deep-clean-data deep-clean-database

clean:
	$(RM) $(OBJECTS_d) $(EXE)

deep-clean:
	$(RM) $(OBJECTS) $(OBJECTS_d) $(EXE)

clean-sensor:
	$(RM) $(OBJ_SENSOR_d) $(EXE_SENSOR)

deep-clean-sensor:
	$(RM) $(OBJ_SENSOR) $(OBJ_SENSOR_d) $(EXE_SENSOR)

clean-tempt:
	$(RM) $(OBJ_TEMPT_d) $(EXE_TEMPT)

deep-clean-tempt:
	$(RM) $(EXE_TEMPT) $(OBJ_TEMPT) $(OBJ_TEMPT_d)

clean-data:
	$(RM) $(OBJ_DATA_d) $(EXE_DATA)

deep-clean-data:
	$(RM) $(EXE_DATA) $(OBJ_DATA) $(OBJ_DATA_d)

clean-database:
	$(RM) $(OBJ_DATA_d) $(EXE_DATA)

deep-clean-database:
	$(RM) $(EXE_DATABASE) $(OBJ_DATABASE) $(OBJ_DATABASE_d)