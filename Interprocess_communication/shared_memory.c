#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

#define PATHNAME "/tmp"
#define PROJ_ID 0x7692

int main() {
	key_t key = ftok(PATHNAME, PROJ_ID);
	if (key == -1) {
		perror("ftok");
		return __LINE__;
	}
	printf("%u\n", key);

	int shmid;
	pid_t cpid = fork();

	if (cpid == 0) {
		int i = 0;
		sleep(1);
		shmid = shmget(key, 8192, IPC_CREAT);
		if (shmid == -1) {
			perror("shmget");
			return __LINE__;
		}

		char *addr = shmat(shmid, NULL, 0);
		if (-1 == (void *)addr) {
			perror("shmat");
			return __LINE__;
		}

		while (i < 26) {
			addr[i] = 'A' + i;
			i++;
			addr[i] = 0;
			sleep(1);
		}
		shmdt(addr);
		sleep(2);

	} else if (cpid > 0) {
		shmid = shmget(key, 8192, IPC_CREAT | 0666);
		if (shmid == -1) {
			perror("shmget");
			shmid = shmget(key, 8192, IPC_CREAT | 0666);
			if (shmid != -1) {
				if (-1 == shmctl(shmid, IPC_RMID, NULL))
					perror("shmctl");
			}
			kill(0, SIGTERM);
			return __LINE__;
		}

		char *addr = shmat(shmid, NULL, 0);
		if (-1 == (void *)addr) {
			perror("shmat");
			return __LINE__;
		}

		int i = 0;
		while (i++ < 26) {
			printf("parent:%s\n", addr);
			sleep(1);
		}
		shmdt(addr);
		sleep(2);
/*
		if (shmctl(shmid, IPC_RMID, NULL) < 0) {
			perror("shmctl");
			return __LINE__;
		}
		*/
	}

	return 0;
}
