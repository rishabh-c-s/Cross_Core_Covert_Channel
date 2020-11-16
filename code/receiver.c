#include "util.h"

long addr;
int  sharedfile;

void sigint_handler(int num){
	printf("%s\n",close(sharedfile)?"File close unsuccessful":"File close successful");
	printf("---RECEIVER END---\n");
	exit(0);
}

unsigned int measure_one_block_access_time()
{
    unsigned int cycles;

    asm volatile("mov %1, %%r8\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "mov %%eax, %%edi\n\t"
            "mov (%%r8), %%r8\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "sub %%edi, %%eax\n\t"
    : "=a"(cycles) /*output*/
    : "r"(addr)
    : "r8", "edi");

    return cycles;
}

inline unsigned int __attribute__((always_inline)) rdtscp() {
	unsigned int t;
	asm volatile ("rdtscp"
	: /* outputs */ "=a" (t));

	return t;
}

inline __attribute__((always_inline)) unsigned int cc_sync() {
    while((rdtscp() & 0x000FFFFF) > 0x0100) {}
    return rdtscp();
}

char *conv_char(char *data, int size, char *msg)
{
    for (int i = 0; i < size; i++) {
        char tmp[8];

        for (int j = 0; j < 8; j++) {
            tmp[j] = data[8*i+j];
        }

        char tm = strtol(tmp, NULL, 2);
        msg[i] = tm;
    }

    msg[size] = '\0';
    return msg;
}

int detect_bit()
{
	int hits = 0, misses=0;
	unsigned int t0 = cc_sync();
	
	while ((rdtscp() - t0) < INTERVAL) {
		unsigned int access_time = measure_one_block_access_time(addr); 
		
		if (access_time > CACHE_MISS_LATENCY) {
			misses++;
		} else {
			hits++;
		}
	}

	return misses >= hits;
}

int main()
{
	signal(SIGINT,sigint_handler);
	int sharedfile = open(SHARED_FILE,O_RDONLY);
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
	
	char msg_ch[1000];

	unsigned int bitSequence = 0;
	unsigned int sequenceMask = ((unsigned int) 1<<6) - 1;
	unsigned int expSequence = 0b101011;
	
	fflush(stdout);
	printf("---RECEIVER START---\n");
	while (1) {
		int bitReceived = detect_bit();

		bitSequence = ((unsigned int) bitSequence<<1) | bitReceived;
		if ((bitSequence & sequenceMask) == expSequence) {
			int binary_msg_len = 0;
			int strike_zeros = 0;
			for (int i = 0; i < 1000; i++) {
				binary_msg_len++;

				if (detect_bit()) {
					msg_ch[i] = '1';
					strike_zeros = 0;
				} else {
					msg_ch[i] = '0';
					if (++strike_zeros >= 8 && i % 8 == 0) {
						break;
					}
				}
			}
			msg_ch[binary_msg_len - 8] = '\0';
			
			int ascii_msg_len = binary_msg_len / 8;
			char msg[ascii_msg_len];
			printf("%s\n", conv_char(msg_ch, ascii_msg_len, msg));
		}
	}
	return 0;
}
