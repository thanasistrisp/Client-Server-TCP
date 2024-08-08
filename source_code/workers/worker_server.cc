#include "defines.h"

#include "buffer.h"

#include <sstream>

using namespace std;

// implement worker thread that reads socket descriptors from buffer and handles them
void *worker_server(void *) {
	while (true) {
		// wait if buffer is empty
		int socketDescriptor = buffer->read();

		if (write(socketDescriptor, "SEND NAME PLEASE\n", 17) < 0) {
			perror_exit("write");
		}

		// read name
		char name[256];
		int n;
		if ((n = read(socketDescriptor, name, 256)) < 0) {
			perror_exit("read");
		}
		name[n] = '\0';
		// remove newline chars
		for (int i = 0; i < n; i++) {
			if (name[i] == '\n' || name[i] == '\r') {
				name[i] = '\0';
			}
		}

		// check if voter has already voted terminate connection if so
		if (buffer->pollEntryExists(name)) {
			if (write(socketDescriptor, "ALREADY VOTED\n", 14) < 0) {
				perror_exit("write");
			}
			if (close(socketDescriptor) < 0) {
				perror_exit("close");
			}
			continue;
		}

		if (write(socketDescriptor, "SEND VOTE PLEASE\n", 17) < 0) {
			perror_exit("write");
		}

		// read vote
		char vote[256];
		if ((n = read(socketDescriptor, vote, 256)) < 0) {
			perror_exit("read");
		}
		vote[n] = '\0';
		// remove newline chars
		for (int i = 0; i < n; i++) {
			if (vote[i] == '\n' || vote[i] == '\r') {
				vote[i] = '\0';
			}
		}

		// check if vote is valid terminate connection if not
		if (string(vote).find_first_not_of(" \t\n\v\f\r") == string::npos || string(name).find_first_not_of(" \t\n\v\f\r") == string::npos) {
			if (write(socketDescriptor, "INVALID VOTE\n", 13) < 0) {
				perror_exit("write");
			}
			if (close(socketDescriptor) < 0) {
				perror_exit("close");
			}
			continue;
		}

		buffer->addPollEntry(name, vote);

		stringstream ss2;
		ss2 << "VOTE for PARTY " << vote << " ACCEPTED\n";
		string message = ss2.str();
		if (write(socketDescriptor, message.c_str(), message.length()) < 0) {
			perror_exit("write");
		}

		// terminate connection
		if (close(socketDescriptor) < 0) {
			perror_exit("close");
		}
	}
}