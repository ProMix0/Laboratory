#define WRITE_REQUEST 15
#define WRITE_ACK 48
#define READ_RESULT 76
#define READ_DONE 42

#define SH_FILE "/home/~/sharedlabfile"

#define MSG_QUEUE 36

typedef struct {
	long type;
} message_t;
