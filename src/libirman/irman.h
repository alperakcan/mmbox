/*
    Based on libirman, see README.libirman
*/
/*
    Copyright (C) 2002  Alper Akcan

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    distch@hotmail.com
    icq : 651561
    tel : 0532 414 05 12
*/
/* number of bytes sent back from the IR interface */
#define IR_CODE_LEN			6

/* timing details. we have `SSEC' instead of `SEC' due to clashes with
 * some (ie Solaris) <time.h> definitions  */
#define SSEC	* 1000000
#define MSEC	* 1000
#define USEC

/* Assuming LONG_MAX to be 2,000,000,000, we have a maximum timeout of
 * approx 2,000s, ie over half an hour.  Plenty! (I should hope)
 */
/* time we allow the port to sort itself out with */ 
#define IR_POWER_ON_LATENCY		(10 MSEC)
/* gap between sending 'I' and 'R' */
#define IR_HANDSHAKE_GAP		(500 USEC)

/* successive initial garbage characters should not be more than this apart */
#define IR_GARBAGE_TIMEOUT		(50 MSEC)
/* letters 'O' and 'K' should arrive within this */
#define IR_HANDSHAKE_TIMEOUT		(2 SSEC)
/* successive bytes of an ir pseudocode should arrive within this time limit */
#define IR_POLL_TIMEOUT			(1 MSEC)

/* timeout for blocking IO */
#define IR_BLOCKING			(-1)

/* return from ir_get_command() on error */
#define IR_CMD_ERROR			-1
#define IR_CMD_UNKNOWN			0

/* size of hash table in ircmd.c.  must be prime */
#define IR_HT_SIZE			271

/* size of string to hold default Irman port name, eg /dev/ttyS0 */
#define IR_PORTNAME_LEN			127

/*
 * Function prototypes
 */

/* irfunc.c */
int ir_init (char *filename);
int ir_finish (void);
unsigned char *ir_get_code (void);
unsigned char *ir_get_code_time (unsigned long timeout);
unsigned char *ir_poll_code (void);
int ir_valid_code (char *text);
char *ir_code_to_text (unsigned char *code);
unsigned char *ir_text_to_code (char *text);
void ir_set_enabled (int val);

/* irio.c */
int ir_open_port (char *filename);
int ir_get_portfd (void);
int ir_close_port (void);
int ir_write_char (unsigned char data);
int ir_read_char (long timeout);
void ir_clear_buffer (void);
void ir_usleep (unsigned long usec);
