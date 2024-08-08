# System Programming - Exercise 2

## Compilation

In the `source_code` directory:

```bash
make poller
make pollSwayer
```

## Execution

```bash
make run-poller
make run-pollSwayer
```

Arguments are already defined in Makefile and defined so that server and client are on the same machine (localhost).

If we want to run the server and client on different machines, we can do the following:

Let's assume that server is running on machine linux25.di.uoa.gr and client is running on machine linux20.di.uoa.gr.

On linux25.di.uoa.gr:

```bash
poller 2839 8 16 pollLog.txt pollStats.txt
```

On linux20.di.uoa.gr:

```bash
pollSwayer linux25.di.uoa.gr 2839 inputFile.txt
```

We can also check server by using telnet as a client (to see also the messages that are sent to the client):

```bash
telnet linux25.di.uoa.gr 2839
```

## Implementation

Voter name should be given as two words, separated by whitespace characters.

Delimiter between voter name and vote is a space, but can also be any whitespace character.

### Problem 1

Master thread is the main function of the running program, not a separated thread is created for this.

Each server worker thread runs infinitely and they are synchronized with various mutexes and condition variables.

Synchronization between workers and master is known as producer-consumer problem and it is implemented with a bounded buffer as instructions say.

We also use internal structures (2 maps) to store the votes and the voters, each of them is protected by its own mutex. We could use one map,
but we prefer to use 2 maps for O(1) complexity of insert and find operations including duplicates.

We also set a signal handler for SIGINT signal, so that we can terminate the program as intended. The whole process is terminated, including all threads obviously.

### Problem 2

Similar to problem 1, here we just create a new thread for each client that connects to the server.

### Problem 3

All logs are created in the `Logs` directory (in `source_code` folder) but for passing txt files as arguments we should use the correct relative path. Examples are given in `simulate.sh` and `test_case.sh` scripts. It is highly recommended to download, create all paths to Logs directory.

Except of the required 3 scripts, we also created a `simulate.sh` script that runs the whole program with different arguments and creates the logs in the `Logs` directory. It runs server in background and after client program and then it kills the server process. At the end, after we ran all the scripts,
we check with `diff` command if the logs are the same as the expected ones. Same, for the `test_case.sh` script, but it runs the program with the inputFile given
as test case in the exercise (not randomly created through `create_input.sh` and overwrite previous existed files). They both return if tests were successful or not.
