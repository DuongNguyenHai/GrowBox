#include "bone-config.h"

namespace BONE {

bool PRINT_MONITOR = true;			// define for print on terminal in Log function.
bool PRINT_FILE = false;			// define for print to file in Log function.
int32_t DEBUG_LEVEL = 3;			// define for debug in database function.
int32_t INTERVAL_REQUEST = 0;
std::vector<int> SCHEDULE;
std::string SCHEDULE_TYPE[2] = {"schedule", "timer"};
std::string LOG_FILE ("root.log");
std::string MASTER_IP ("127.0.0.1");

std::string DEFINE_ARGS[6][2] = 	{ 	
										{"LOG_FILE",Tp_STR},
										{"PRINT_MONITOR", Tp_BOOL},
										{"PRINT_FILE", Tp_BOOL},
										{"DEBUG_LEVEL", Tp_INT},
										{"SCHEDULE", Tp_TIME},
										{"MASTER_IP",Tp_STR}
									};

const char *usage = 	"Need a config file: \"config/root.conf\" \n"
						"Usage : <option> = <content>\n"
						"example :"
						"\n"
						"##################################################"
						"\n\n"
						"LOG_FILE=/home/username/root/root.log\n"
						"PRINT_MONITOR=true\n"
						"PRINT_FILE=true\n"
						"DEBUG_LEVEL=1\n"
						"MASTER_IP=192.168.13.13"
						"SCHEDULE={\"schedule\":[\"7h\",\"12h30'30\",\"18h\",\"0h\"]}\n"
						"#SCHEDULE={\"timer\":[1800]}\n"
						"\n"
						"##################################################";
}// end of namespace TREE
