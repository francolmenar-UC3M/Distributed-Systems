#include "read_line.h"

/*It sends a message of a determined lenght by a socket*/
int send_msg(int socket, char *message, int length){
	int r;
	int l = length; /*store the length in an auxiliary variable*/

	do {
		r = write(socket, message, l); /*write in the socket*/
		l = l -r; /*substracting the amount writen in the socket to the total lenght*/
		message = message + r; /* increment offset in the buffer */
	} while ((l>0) && (r>=0)); /*while there is something to write*/

	if (r < 0)
		return (-1);   /* fail */
	else
		return(0);	/* success */
}

/*It receives a message of a determined length*/
int recv_msg(int socket, char *message, int length){
	int r;
	int l = length; /*store the length in an auxiliary variable*/

	do {
		r = read(socket, message, l); /*read from the socket*/
		l = l -r ; /*substracting the amount read from the socket to the total lenght*/
		message = message + r;
	} while ((l>0) && (r>=0)); /*while there is something to read*/

	if (r < 0)
		return (-1);   /* fail */
	else
		return(0);	/* success */
}

/*It reads from	 a	file	or socket	 descriptor a	 string	 with	 a maximum length	of n bytes
	If this	number is	exceeded,	the	remaining	characters	are	discarded
	The	function	returns	a	string	that	ends	with the ASCII code	0
	If the source	 is	 performed by	 the screen,the	 function	 returns when	 the line	 break is	introduced.
	The	newline	character	is	not	included	in	the	string	returned	by	the	function.*/
ssize_t readLine(int fd, void *buffer, size_t n){
	ssize_t numRead;  /* num of bytes fetched by last read() */
	size_t totRead;	  /* total bytes read so far */
	char *buf;
	char ch;

	if (n <= 0 || buffer == NULL) { /*check the validity of the parameters*/
		errno = EINVAL;
		return -1;
	}

	buf = buffer; /*store the buffer in an auxiliary variable*/
	totRead = 0;

	for (;;) {
    numRead = read(fd, &ch, 1);	/* read a byte */

    if (numRead == -1) { /*check for errors while reading*/
      if (errno == EINTR)	/* interrupted -> restart read() */
        continue;
      else
				return -1;		/* some other error */
    }
		else if (numRead == 0) {	/* EOF */
      if (totRead == 0)	/* no byres read; return 0 */
      	return 0;
			else
      	break;
    }
		else {			/* numRead must be 1 if we get here*/
      if (ch == '\n')
        break;
      if (ch == '\0')
        break;
      if (totRead < n - 1) {		/* discard > (n-1) bytes */
				totRead++;
				*buf++ = ch; /*copy the char read*/
			}
		}
	}
	*buf = '\0'; /*the last character of the string*/
  return totRead; /*return the number of characters read*/
}
