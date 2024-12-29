#include "common.h"

/*///////////////////////////////////////////////////////
/                STRUCT DEFINITIONS                     /
*////////////////////////////////////////////////////////

typedef struct circular_queue {
  unsigned int size;     // Total size of buf in bytes
  unsigned int num_msgs; // Number of messages currently in queu
  void *buf;             // Points to low address of buf
  void *head;            // Where to start reading from
  void *tail;            // Where to start writing from
} CIRC_Q;

#define CSIZE cq->size
#define NUM_MSGS cq->num_msgs
#define HEAD cq->head
#define TAIL cq->tail
#define BUF cq->buf

/*///////////////////////////////////////////////////////
/                FUNCTION DECLARATIONS                  /
*////////////////////////////////////////////////////////

/*  @brief initializes the circular queue
 *  @param pointer to location in memory of circular queue
           pointer to its buffer (propobly sizeof(CIRC_Q) bytes offset from cq)
           size in bytes of the circular queue
 *  @return 0 if success, -1 if error
 */
int cq_init(CIRC_Q *cq, void *buf, int size);

/*  @brief zeros out the circular queue and its head/tail pointers
 *  @param pointer circular queue
 *  @return 0 if success, -1 if error
 */
int cq_clear(CIRC_Q *cq);

/*  @brief writes to the circular queue a msg of size len bytes
 *  @param pointer to circular queue
           the length of the message buffer
           the buffer containing the message to write
 *  @return 0 if success, -1 if error
 */
int cq_write(CIRC_Q *cq, int len, unsigned char *msg_buf);

/*  @brief reads a message from circular queue
 *  @param pointer to circular queue
           length of the message buffer
           the buffer containing the message read from the buffer
 *  @return 0 if success, -1 if error
 */
int cq_read(CIRC_Q *cq, int len, unsigned char *msg_buf);

/*  @brief gets remaining space in the circular queue
 *  @param pointer to circular queue
 *  @return remaining space in queue in bytes
 */
int cq_count_space(CIRC_Q *cq);

/*  @brief prints all the messages in the queue to console
 *  @param pointer to circular queue
 *  @return none
 */

#ifdef DEBUG_0
void cq_print(CIRC_Q *cq);
#endif

