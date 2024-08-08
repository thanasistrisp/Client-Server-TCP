#include "defines.h"
#include "worker_client.h"

using namespace std;

int main(int argc, char** argv) {
	if (argc != 4) {
		cout << "Usage: ./pollSwayer <serverName> <portNum> <inputFile.txt>" << endl;
		return 1;
	}
	string serverName = argv[1];
	int portNum = atoi(argv[2]);
	string inputFile = argv[3];

	string logPath = inputFile;

	// open input file
	ifstream input;
	input.open(logPath.c_str());
	if (!input.is_open()) {
		perror_exit("open");
	}
	// count lines
	int numLines = 0;
	string line;
	while (getline(input, line)) {
		numLines++;
	}
	input.close();
	// create threads
	pthread_t *threads = new pthread_t[numLines];
	// for each line send it to worker thread
	input.open(logPath.c_str());
	if (!input.is_open()) {
		perror_exit("open");
	}
	int i = 0;
	while (getline(input, line)) {
		args *arguments = new args;
		arguments->serverName = serverName;
		arguments->portNum = portNum;
		arguments->line = line;
		if (pthread_create(&threads[i], NULL, worker_client, (void *) arguments) != 0) {
			perror_exit("pthread_create");
		}
		i++;
	}
	input.close();
	// wait for threads to finish
	for (int i = 0; i < numLines; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			perror_exit("pthread_join");
		}
	}
	// delete threads
	delete[] threads;
	
	return 0;
}
