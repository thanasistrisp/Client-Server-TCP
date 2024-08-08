#include "defines.h"

#include "buffer.h"
#include "worker_server.h"

using namespace std;

Buffer *buffer;
string pollLog;
string pollStats;
pthread_t *workerthreads;
int numWorkerthreads;
int socketDescriptor;

void sigint_handler(int signum) {
	signal(signum, sigint_handler);
	cout << "Exiting..." << endl;
	buffer->writeStats();
	if (close(socketDescriptor) < 0) {
		perror_exit("close");
	}
	delete buffer;
	delete[] workerthreads;
	exit(0);
}

int main(int argc, char** argv) {
	struct sigaction act;
	act.sa_handler = sigint_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror_exit("sigaction");
	}

	if (argc != 6) {
		cout << "Usage: ./poller <portnum> <numWorkerthreads> <bufferSize> <pollLog> <pollStats>" << endl;
		return 1;
	}
	int portnum = atoi(argv[1]);
	numWorkerthreads = atoi(argv[2]);
	int bufferSize = atoi(argv[3]);
	pollLog = argv[4];
	pollStats = argv[5];

	// initialize buffer
	buffer = new Buffer(bufferSize);

	// create worker threads
	workerthreads = new pthread_t[numWorkerthreads];
	for (int i = 0; i < numWorkerthreads; i++) {
		if (pthread_create(&workerthreads[i], NULL, worker_server, NULL) != 0) {
			perror_exit("pthread_create");
		}
	}

	// listen for connections
	if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror_exit("socket");
	}
	// set socket option SO_REUSEADDR to avoid "Address already in use" error
	int option = 1;
	setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	struct sockaddr_in server;
	struct sockaddr *serverptr =(struct sockaddr *) &server ;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(portnum);
	if (bind(socketDescriptor, serverptr, sizeof(server)) < 0) {
		perror_exit("bind");
	}
	if (listen(socketDescriptor, SOMAXCONN) < 0) { // not small number as linux at lab cannot handle it (e.g. if set 5)
		perror_exit("listen");
	}

	cout << "Listening for connections to port " << portnum << endl;

	// accept connections and write socket descriptors to buffer
	int newsock;
	struct sockaddr_in client;
	struct sockaddr *clientptr =(struct sockaddr *) &client ;
	socklen_t clientlen = sizeof(client);
	while (1) {
		if ((newsock = accept(socketDescriptor, clientptr, &clientlen)) < 0) {
			perror_exit("accept");
		}
		buffer->write(newsock);
	}

	// we never reach this point

	return 0;
}
