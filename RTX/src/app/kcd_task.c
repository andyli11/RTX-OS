/* The KCD Task Template File */

#include "..\kernel\k_mem.h"
#include "..\kernel\k_HAL_CA.h"
#include "..\INC\common.h"
#include "..\lib\ascii_array.h"
#include "..\kernel\k_msg.h"
#include "Serial.h"
#include "..\INC\RTX.h"

#ifdef DEBUG_0
#include "printf.h"
#endif

#define ENTER_KEY          (13)
#define BACKSPACE_KEY      (8)
#define HEADER_SIZE        (sizeof(RTX_MSG_HDR))
#define MAX_CMD_SIZE       (64)
#define KCD_BUF_SIZE       (MAX_CMD_SIZE + 1 + HEADER_SIZE)

unsigned char recv_buf[KCD_BUF_SIZE];   // Buf for holding the recieved message
unsigned char send_buf[MAX_CMD_SIZE + 1];   // Buf for holding the message to be sent

char *cantprocess = "Command cannot be processed\r\n";
char *invalid = "Invalid Command\r\n";

void kcd_task(void)
{
    // Create the mailbox
    if(mbx_create(KCD_MBX_SIZE) == RTX_ERR) {
#ifdef DEBUG_0
    	printf("Mailbox create failed!\n");
#endif
    }

    // Initialize the reg array
    ascii_array_init();

    int send_size = 0;

    while(1)
    {
        task_t sender_tid = 0;
        //read
        if(recv_msg(&sender_tid, recv_buf, KCD_BUF_SIZE)==RTX_ERR) {
#ifdef DEBUG_0
            printf("k_recv_msg error in kcd_task.c\n");
#endif
            continue;
        }
        // get header and data from received buffer
        RTX_MSG_HDR* header = ((RTX_MSG_HDR *) recv_buf);
        void *msg = (void*)((int)recv_buf + sizeof(RTX_MSG_HDR));
        // check message length is 1
        int msg_length = header->length - sizeof(RTX_MSG_HDR);
        if(msg_length > 1) continue;
        // ------- 2 cases ------
        // 1. KCD_REG
        // 2. KEY_IN
        if (header->type == KCD_REG)
        {
            // get single command char
            char command_identifier = *((char *)(msg));
            // set sender tid at command identifier
            if(set_tid_at_char_index(command_identifier, sender_tid)!=0) continue;
        }
        else if (header->type==KEY_IN && sender_tid==TID_UART_IRQ)
        {
            // KEY_IN
            char recv_character = *((char *)(msg));
            if (recv_character == ENTER_KEY)
            {
                if (send_size > 64 || send_buf[0] != '%') //array is size 65 so max index is 64
                {
                    // If size exceeds then send error and reset
                    SER_PutStr(1, invalid);
                    send_size = 0;
                    continue;
                }
                else
                {
                    // Look to send
                    // Check if key is invalid, identifier is not registered, or task no longer exists
                    int char_to_index_index = char_to_index(send_buf[1]);
                    task_t recv_tid = key_tid_array[char_to_index_index];
                    if (char_to_index_index==-1 || recv_tid == 999 || g_tcbs[recv_tid].state==DORMANT)
                    {
                        // Identifier not registered yet
                        SER_PutStr(1, cantprocess);
                        send_size = 0;
                        continue;
                    }

                    int send_msg_size = HEADER_SIZE + send_size - 1; // 12 + send size -1 for the %
                    char* send_msg = mem_alloc(send_msg_size);
                    RTX_MSG_HDR *send_msg_header = (RTX_MSG_HDR *) send_msg;

                    // Set header values for message to be sent
                    send_msg_header->length = send_msg_size;
                    send_msg_header->type = KCD_CMD;
                    send_msg_header->sender_tid = TID_KCD;
                    char *send_msg_data = (char*)(send_msg_header + 1);

                    // Copy message excluding the % character
                    for (int i = 1; i < send_size; i++) {
                        send_msg_data[i-1] = send_buf[i];
                    }

                    // SEND IT and reset
                    if(send_msg(recv_tid, send_msg) == RTX_ERR) {
                        SER_PutStr(1, cantprocess);
                    }
                    // Dealloc the mem
                    mem_dealloc(send_msg);
                    send_size = 0;
                }
            }
            //backspace
            else if (recv_character == BACKSPACE_KEY && send_size > 0) send_size--;
            //other character
            else
            {
				if (send_size <= 64) send_buf[send_size] = recv_character;
				//if send_size exceeds 65, no need to store more characters
				send_size++;
			}
        }
    }
}
