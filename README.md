# Progetto-SO

In the project i implemented an IPC system based on message queues in disastrOS.

In order to allow the communication between processes i added in disastrOS two different structures (Messagequeues and Descrittori) and some syscall:

- mq_open, that open a Messagequeues and create it is specified
- mq_close and mq_unlink, that remove the given messagequeue
- mq_send and receive, that allow sending and receiving messages through the system

In order to test them implemented a call of each function in disastrOS_test.c
