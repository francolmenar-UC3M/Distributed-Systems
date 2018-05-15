#include <unistd.h>
#include <errno.h>

/*It sends a message of a determined lenght by a socket*/
int send_msg(int socket, char *message, int length);

/*It receives a message of a determined length*/
int recv_msg(int socket, char *message, int length);

/*It reads from	 a	file	or socket	 descriptor a	 string	 with	 a maximum length	of n bytes
	If this	number is	exceeded,	the	remaining	characters	are	discarded
	The	function	returns	a	string	that	ends	with the ASCII code	0
	If the source	 is	 performed by	 the screen,the	 function	 returns when	 the line	 break is	introduced.
	The	newline	character	is	not	included	in	the	string	returned	by	the	function.*/
ssize_t readLine(int fd, void *buffer, size_t n);
