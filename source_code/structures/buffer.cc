#include "defines.h"

#include "buffer.h"

using namespace std;

Buffer::Buffer(int bufferSize) {
	this->bufferSize = bufferSize;
	this->buffer = new int[bufferSize];
	this->start = 0;
	this->end = -1;
	this->count = 0;
	pthread_mutex_init(&mtx, 0);
	pthread_mutex_init(&logMtx, 0);
	pthread_mutex_init(&pollVoterMtx, 0);
	pthread_mutex_init(&pollCountMtx, 0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_cond_init(&cond_nonfull, 0);
	string pollLogPath = "Logs/" + pollLog;
	pollLogFile.open(pollLogPath.c_str(), std::ofstream::out | std::ofstream::trunc); // truncate
	if (!pollLogFile.is_open()) {
		perror_exit("open");
	}
	string pollStatsPath = "Logs/" + pollStats;
	pollStatsFile.open(pollStatsPath.c_str(), std::ofstream::out | std::ofstream::trunc); // truncate
	if (!pollStatsFile.is_open()) {
		perror_exit("open");
	}
}

Buffer::~Buffer() {
	// close remaining sockets
	while (count > 0) {
		int socketDescriptor = read();
		if (close(socketDescriptor) < 0) {
			perror_exit("close");
		}
	}
	delete[] buffer;
	pthread_cond_init(&cond_nonfull, 0);
	pthread_mutex_destroy(&mtx);
	pthread_mutex_destroy(&logMtx);
	pthread_mutex_destroy(&pollVoterMtx);
	pthread_mutex_destroy(&pollCountMtx);
	pollLogFile.close();
	pollStatsFile.close();
}

void Buffer::write(int socketDescriptor) {
	pthread_mutex_lock(&mtx);
	while (count == bufferSize) {
		pthread_cond_wait(&cond_nonfull, &mtx);
	}
	end = (end + 1) % bufferSize;
	buffer[end] = socketDescriptor;
	count++;
	pthread_mutex_unlock(&mtx);
	pthread_cond_signal(&cond_nonempty);
}

int Buffer::read() {
	pthread_mutex_lock(&mtx);
	while (count == 0) {
		pthread_cond_wait(&cond_nonempty, &mtx);
	}
	int socketDescriptor = buffer[start];
	start = (start + 1) % bufferSize;
	count--;
	pthread_mutex_unlock(&mtx);
	pthread_cond_signal(&cond_nonfull);
	return socketDescriptor;
}

void Buffer::addPollEntry(string name, string vote) {
	pthread_mutex_lock(&logMtx);
	pollLogFile << name << " " << vote << endl;
	pthread_mutex_unlock(&logMtx);
	pthread_mutex_lock(&pollVoterMtx);
	pollVoter[name] = vote;
	pthread_mutex_unlock(&pollVoterMtx);
	pthread_mutex_lock(&pollCountMtx);
	if (pollCount.find(vote) == pollCount.end()) {
		pollCount[vote] = 1;
	} else {
		pollCount[vote]++;
	}
	pthread_mutex_unlock(&pollCountMtx);
}

bool Buffer::pollEntryExists(string name) {
	pthread_mutex_lock(&pollVoterMtx);
	bool exists = pollVoter.find(name) != pollVoter.end();
	pthread_mutex_unlock(&pollVoterMtx);
	return exists;
}

void Buffer::writeStats() {
	pthread_mutex_lock(&pollCountMtx);
	for (map<string, int>::iterator it = pollCount.begin(); it != pollCount.end(); it++) {
		pollStatsFile << it->first << " " << it->second << endl;
	}
	int total = 0;
	for (map<string, int>::iterator it = pollCount.begin(); it != pollCount.end(); it++) {
		total += it->second;
	}
	pollStatsFile << "TOTAL " << total << endl;
	pthread_mutex_unlock(&pollCountMtx);
}