#include "defines.h"

class Buffer {
public:
	Buffer(int bufferSize);
	~Buffer();
	void write(int socketDescriptor);
	void addPollEntry(std::string name, std::string vote);
	int read();
	bool pollEntryExists(std::string name);
	void writeStats();
private:
	int* buffer;
	int bufferSize;
	int start;
	int end;
	int count;
	pthread_mutex_t mtx;
	pthread_mutex_t logMtx;
	pthread_mutex_t pollVoterMtx;
	pthread_mutex_t pollCountMtx;
	pthread_cond_t cond_nonempty;
	pthread_cond_t cond_nonfull;
	std::ofstream pollLogFile;
	std::ofstream pollStatsFile;
	std::map<std::string, std::string> pollVoter;
	std::map<std::string, int> pollCount;
};

extern Buffer *buffer;
extern std::string pollLog;
extern std::string pollStats;
