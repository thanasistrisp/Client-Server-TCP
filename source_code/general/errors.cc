#include "defines.h"

using namespace std;

void perror_exit(string message) {
	perror(message.c_str());
	exit(EXIT_FAILURE);
}
