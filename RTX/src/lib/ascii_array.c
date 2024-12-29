/*
 * THIS IS THE ARRAY FOR TIDS AT THEIR CORRESPONDING CHAR INDEX
 * REMOVE THIS AND MODIFY set_tid_at_char_index FUNCTION IF YOU WANT
*/

#include "ascii_array.h"

int key_tid_array[62];

/*  @brief gets the index of char
 *  @param c character
 *  @return character index if success, -1 if error
 */
int char_to_index(unsigned char c)
{
  if (c > 47 && c < 58) // numbers
  {
    return c - 48;
  }
  else if (c > 64 && c < 91) // cap letters
  {
    return c - 55;
  }
  else if (c > 96 && c < 123) // lower letters
  {
    return c - 61;
  }
  else return -1;
}

/*  @brief assign a tid at the index of char
 *  @param c character
 *  @param tid tid
 *  @return 0 if success, -1 if error
 */
int set_tid_at_char_index(unsigned char c, int tid)
{
  int idx = char_to_index(c);
  key_tid_array[idx] = tid;
  return 0;
}

void ascii_array_init(void)
{
  for (int i = 0; i < 62; i++)
  {
    key_tid_array[i] = 999;
  }
}

/*
void print_array()
{
  for(int i = 0; i < sizeof(key_tid_array); i++)
    {
      printf("%d\n", key_tid_array[i]);
    }
}
*/

/* example usage
int main() {
  set_tid_at_index('d', 4);
  print_array();
  return 0;
}
*/
