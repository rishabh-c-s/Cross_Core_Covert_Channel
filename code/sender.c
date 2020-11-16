#include "util.h"

long addr;
int  sharedfile;

void sigint_handler(int num){
	printf("%s\n",close(sharedfile)?"File close unsuccessful":"File close successful");
	printf("---SENDER END---\n");
	exit(0);
}

inline unsigned int __attribute__((always_inline)) rdtscp() {
	unsigned int t;
	asm volatile ("rdtscp"
	: /* outputs */ "=a" (t));

	return t;
}

inline unsigned int __attribute__((always_inline)) cc_sync() {
    while((rdtscp() & 0x000FFFFF) > 256);
    return rdtscp();
}

char *string_to_binary(char *s)
{
	size_t len = strlen(s) - 1;

	char *binary = malloc(len * 8 + 1);
	binary[0] = '\0';

	for (size_t i = 0; i < len; ++i) {
		char ch = s[i];
		for (int j = 7; j >= 0; --j) {
		    if (ch & (1 << j)) {
		        strcat(binary, "1");
		    } else {
		        strcat(binary, "0");
		    }
		}
	}

	return binary;
}

void send_bit(int bit)
{
	unsigned int t0 = cc_sync();
	if(bit==1){
		while ((rdtscp() - t0) < INTERVAL) _mm_clflush((int*)addr);
	}
	else while (rdtscp() - t0 < INTERVAL);
}

int main(){
	signal(SIGINT,sigint_handler);
	sharedfile = open(SHARED_FILE,O_RDONLY);
	if(sharedfile==-1){
		printf("Unable to open shared file\n");
		exit(1);
	}
	
	void *mapaddr = mmap(NULL,4096,PROT_READ,MAP_SHARED,sharedfile,0);
	if (mapaddr == (void*) -1 ) {
		printf("Memory mapping failed\n");
		exit(1);
	}
	addr = (long) mapaddr;
	
	printf("---SENDER START---\n");
	while (1) {
		char message_buffer[128];
		fgets(message_buffer, sizeof(message_buffer), stdin);
		
		char *message = string_to_binary(message_buffer);
		
		send_bit(1);
		send_bit(0);
		send_bit(1);
		send_bit(0);
		send_bit(1);
		send_bit(0);
		send_bit(1);
		send_bit(1);
		
		for (int i = 0; i < strlen(message); i++) {
			if (message[i] == '1') {
				send_bit(1);
			} else {
				send_bit(0);
			}
		}
	}
	return 0;
}
