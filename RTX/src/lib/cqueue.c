#include "cqueue.h"
#include "../INC/common.h"

//#define DEBUG_0

#ifdef DEBUG_0
#include <printf.h>
#endif

int cq_init(CIRC_Q *cq, void *buf, int size) {
  // Error checking
  if (cq == NULL || buf == NULL || size <= sizeof(RTX_MSG_HDR)) {
    return RTX_ERR;
  }
  // Init queue members
  CSIZE = size;
  NUM_MSGS = 0;
  BUF = buf;
  HEAD = buf;
  TAIL = buf;
  // Zero out the data
  cq_clear(cq);
  return RTX_OK;
}

int cq_clear(CIRC_Q *cq) {
  // Error check
  if (cq == NULL) {
    return RTX_ERR;
  }
  // Iterate through the buffer and zero out every index
  for (int i = 0; i < CSIZE; i++) {
    ((unsigned char *)BUF)[i] = 0;
  }
  return RTX_OK;
}

int cq_write(CIRC_Q *cq, int len, unsigned char *msg_buf) {
  // Error check
  if (cq == NULL || len <= sizeof(RTX_MSG_HDR) || msg_buf == NULL) {
    return RTX_ERR;
  }
  // Check if queue has enough space left
#ifdef DEBUG_0
  printf("cq internal count of remaining space: %d\n", cq_count_space(cq));
#endif
  if (len > cq_count_space(cq)) {
    return RTX_ERR;
  }

  // Add to the queue
  int bytes_remaining = len; // Starts at len and counts down to 0

  while (bytes_remaining) {
    // Copy a byte from buffer to queue
    *(unsigned char *)(TAIL) = msg_buf[len - bytes_remaining];
    TAIL = (void*)(((int)(TAIL) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(TAIL) + 1));
    bytes_remaining--;
  }
  NUM_MSGS++;
  return RTX_OK;
}

int cq_read(CIRC_Q *cq, int len, unsigned char *msg_buf) {
  // Error check
  if (cq == NULL || len <= sizeof(RTX_MSG_HDR) || msg_buf == NULL) {
    return RTX_ERR;
  }

  unsigned char *rd_ptr = (unsigned char *)HEAD;
  unsigned char msg_s[4];
  for (int i = 0; i < 4; i++) {
    msg_s[i] = *rd_ptr;
    rd_ptr = (void*)(((int)(rd_ptr) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(rd_ptr) + 1));
  }
  unsigned int msg_length = *(unsigned int *)(&msg_s);

  if (msg_length > len) {
    // Remove the message and return error
    int bytes_remaining = msg_length;
    while (bytes_remaining) {
      HEAD = (void*)(((int)(HEAD) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(HEAD) + 1));
      bytes_remaining--;
    }
    NUM_MSGS--;
    return RTX_ERR;
  }
  // Else read message from queue into the buffer
  int bytes_remaining = msg_length; // Starts at len and counts down to 0
  while (bytes_remaining) {
    // Copy a byte from buffer to queue
    msg_buf[msg_length - bytes_remaining] = *(unsigned char *)(HEAD);
    HEAD = (void*)(((int)(HEAD) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(HEAD) + 1));
    bytes_remaining--;
  }
  NUM_MSGS--;
  return RTX_OK;
}

int cq_count_space(CIRC_Q *cq) {
  if (HEAD > TAIL || (HEAD == TAIL && NUM_MSGS != 0)) {
    return (int)HEAD - (int)TAIL;
  } else {
    return (CSIZE - ((int)TAIL - (int)HEAD));
  }
}

#ifdef DEBUG_0
void cq_print(CIRC_Q *cq) {
  if (NUM_MSGS == 0) {
    printf("\nMessage queue is empty.\n");
    return;
  }
  int msgs_remaining = NUM_MSGS;
  unsigned char *rd_ptr = (unsigned char *)HEAD;
  printf("\n-----------------------------\n");
  while (msgs_remaining) {
    printf("\nMessage #%d | buf[%d]:\n", NUM_MSGS - msgs_remaining,
           (unsigned int)rd_ptr - (unsigned int)HEAD);
    // Msg size (S)
    unsigned char msg_s[4];
    for (int i = 0; i < 4; i++) {
      msg_s[i] = *rd_ptr;
      rd_ptr = (void*)(((int)(rd_ptr) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(rd_ptr) + 1));
    }
    unsigned int msg_size = *(unsigned int *)(&msg_s);
    printf("(S): %d \n", msg_size);

    // Msg type (T)
    unsigned char msg_t[4];
    for (int i = 0; i < 4; i++) {
      msg_t[i] = *rd_ptr;
      rd_ptr = (void*)(((int)(rd_ptr) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(rd_ptr) + 1));
    }
    unsigned int msg_type = *(unsigned int *)(&msg_t);
    printf("(T): %s \n", (msg_type == DEFAULT)   ? "DEFAULT"
                         : (msg_type == KCD_REG) ? "KCD_REG"
                         : (msg_type == KCD_CMD) ? "KCD_CMD"
                         : (msg_type == DISPLAY) ? "DISPLAY"
                         : (msg_type == KEY_IN)  ? "KEY_IN"
                                                 : "UNKNOWN");
    // Msg sender (ID)
    unsigned char msg_id[4];
    for (int i = 0; i < 4; i++) {
      msg_id[i] = *rd_ptr;
      rd_ptr = (void*)(((int)(rd_ptr) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(rd_ptr) + 1));
    }
    unsigned int msg_sender = *(unsigned int *)(&msg_id);
    printf("(ID): %d \n", msg_sender);

    // Msg contents (M)
    for (int i = 0; i < msg_size - sizeof(RTX_MSG_HDR); i++) {
      printf("(M): %d ", *(unsigned char*)rd_ptr);
      rd_ptr = (void*)(((int)(rd_ptr) == ((int)(BUF) + CSIZE - 1)) ? ((int)(BUF)) : ((int)(rd_ptr) + 1));
    }
    printf("\n");
    msgs_remaining--;
  }
  printf("\n-----------------------------\n");
}
#endif
