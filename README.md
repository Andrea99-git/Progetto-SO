# Progetto-SO

In the project i implemented an IPC system based on message queues in disastrOS.

In order to allow the communication between processes i added in disastrOS two different structures (Messagequeues and Descrittori) and some syscall:

- mq_open, that open a Messagequeues and create it, if it's specified. Returns the mq_descriptor in case of success, a number < 0 in other cases (depending on the error obtained)
- mq_close, that close the descriptor of the given messagequeue. Returns 0 in case of success; in case of failure it returns the number of the error (< 0)
- mq_unlink, that remove the given messagequeue. Returns 0 in case of success; in case of failure it returns the number of the error (< 0)
- mq_send and receive, that allow sending and receiving messages through the system. They both follow the "blocking message queue rules": if a message cannot be sent or received, sleep for a bit until i do that. In case of failure they return the number of the error (< 0)

In order to test them, i implemented a call of each function in disastrOS_test.c. In order to test the "blocking part" of the syscalls i also implementented a specific test in ./disastrOS_test_mq_bloccanti

## How to run:
- $ make
- $ ./disastrOS_test
- $ ./disastrOS_test_mq_bloccanti
