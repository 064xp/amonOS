#ifndef IPC_TYPES
#define IPC_TYPES

typedef struct __attribute__((__packed__)) resPacket {
  char user[12];
  char cwd[TOKENLEN];
  char buffer[OUTPUT_BUF_LEN];
  int secCode;
} ResponsePacket;

typedef struct __attribute__((__packed__)) reqPacket {
  char user[12];
  char buffer[OUTPUT_BUF_LEN];
  int secCode;
} RequestPacket;

#endif
