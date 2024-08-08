#include "defines.h"
#include <sstream>
#include "worker_client.h"

using namespace std;

void *worker_client(void *arg) {
	// take struct args
	args *arguments = (args *) arg;
	string serverName = arguments->serverName;
	int portNum = arguments->portNum;
	string line = arguments->line;
	delete arguments;
	// find server
	struct hostent *rem;
	if ((rem = gethostbyname(serverName.c_str())) == NULL) {
		herror("gethostbyname");
		exit(EXIT_FAILURE);
	}
	// connect to server
	struct sockaddr_in server;
	struct sockaddr *serverptr =(struct sockaddr *) &server ;
	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(portNum);
	// create socket
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror_exit("socket");
	}
	if (connect(sock, serverptr, sizeof(server)) < 0) {
		perror_exit("connect");
	}
	// split line
	stringstream ss(line);
	string name;
 	string surname;
	string vote;
	ss >> name;
	ss >> surname;
	string namesur = name + ' ' + surname;
	ss >> vote;
	// wait for response
	char response[256];
	int n;
	if ((n = read(sock, response, 256)) < 0) {
		perror_exit("read");
	}
	response[n] = '\0';
	// send one by one
	if (write(sock, namesur.c_str(), namesur.length()) < 0) {
		perror_exit("write");
	}
	// wait for response
	if ((n = read(sock, response, 256)) < 0) {
		perror_exit("read");
	}
	response[n] = '\0';
	if (write(sock, vote.c_str(), vote.length()) < 0) {
		perror_exit("write");
	}
	// close socket
	if (close(sock) < 0) {
		perror_exit("close");
	}
	return NULL;
}