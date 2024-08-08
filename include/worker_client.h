#include <string>

void *worker_client(void *);

struct args {
	std::string serverName;
	int portNum;
	std::string line;
} typedef args;