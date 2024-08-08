# Κάνοντας compile το <foo>_test.cpp με μια υλοποίηση <foo>.cpp του
# συγκεκριμένου τύπου, παράγουμε ένα test για την υλοποίηση αυτή.

poller_OBJS = main_server.o \
			  structures/buffer.o \
			  workers/worker_server.o \
			  general/errors.o \

poller_ARGS = 2839 8 16 log.txt stat.txt

pollSwayer_OBJS = main_client.o \
				  general/errors.o \
				  workers/worker_client.o \

pollSwayer_ARGS = localhost 2839 Logs/inputFile

# Ο βασικός κορμός του Makefile
include ../common.mk
