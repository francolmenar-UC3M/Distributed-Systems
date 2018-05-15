/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include <pthread.h>

#include "user_storage.h"

#include "dlinkedlist.h"
#include "md5.h"

dll* head_users;
dll* head_messages;

unsigned int next_message_id;

pthread_mutex_t message_id_lock = PTHREAD_MUTEX_INITIALIZER;

int *
init_1_svc(struct svc_req *rqstp)
{
	static int  result;

	// pthread_mutex_init(&message_id_lock, NULL);
	next_message_id = 0;

	destroyList(head_users);
	destroyList(head_messages);

	head_users = createList();
	head_messages = createList();

	result = 0;

	return &result;
}

int *
register_user_1_svc(char *username,  struct svc_req *rqstp)
{
	static int  result;

	if (search_user(head_users, username) != NULL) {
		printf("s> REGISTER %s FAIL\n", username);
		result = 1;
		// return 1;
		return &result;
	} else {
		struct user *new_user = (struct user*)malloc(sizeof(struct user));
		strcpy(new_user->username, username);
		new_user->status = 0;
		new_user->ip_address = 0;
		new_user->port = 0;
		new_user->pending_messages = NULL;
		new_user->last_message = 0;

		Node* new_node = getNewNode((void*)new_user);
		insert_user(head_users, new_node);
		printf("s> REGISTER %s OK\n", username);
	}
	// return 0;
	result = 0;

	return &result;
}

int *
unregister_user_1_svc(char *username,  struct svc_req *rqstp)
{
	static int  result;

	/* Succesful unregister */
  if(delete_user(head_users, username) == 0){
    printf("s> UNREGISTER %s OK\n", username);
    // return 0;
		result = 0;
		return &result;
  }
  /* ERROR: user is not found in the data structure */
  else if(delete_user(head_users, username) == -1){
    printf("s> UNREGISTER %s FAIL\n", username);
    // return 1;
		result = 1;
		return &result;
  }
  /* ERROR: any other case */
  else{
    printf("s> UNREGISTER %s FAIL\n", username);
    // return 2;
		result = 2;
		return &result;
  }

	return &result;
}

int *
add_user_1_svc(struct user usr, struct svc_req *rqstp)
{
	static int result;
	Node* newNode = getNewNode((void*)&usr);

	if (search_user(head_users, usr.username) != NULL) {
		result = modify_user(head_users, newNode);
	} else {
		result = insert_user(head_users, newNode);
	}

	return &result;
}

struct user *
get_user_1_svc(char *username,  struct svc_req *rqstp)
{
	static struct user  result;

	struct Node *user_node;
	user_node = search_user(head_users, username);

	if (user_node == NULL) {
		return NULL;
	} else {
		result = *(struct user*)user_node->data;
		return &result;
	}

	return &result;
}

int *
add_message_1_svc(struct message msg,  struct svc_req *rqstp)
{
	static int  result;

	if ((strlen(msg.text)+1) > MAXSIZE) {
	 result = 2;
	 return &result;
	}

	insert_msg(head_messages, (void*)&msg);

	result = 0;

	return &result;
}

int *
get_total_messages_1_svc(char *username,  struct svc_req *rqstp)
{
	static int  result;

	int count = 0;
 	count = get_total_messages_by_user(head_messages, username);
 	result = count;

	return &result;
}

const char *md5sum(const char *chaine)
 {
     struct md5_ctx ctx;
     unsigned char digest[16];
     md5_init(&ctx);
     ctx.size = strlen(chaine);
     strcpy(ctx.buf, chaine);
     md5_update(&ctx);
     md5_final(digest, &ctx);
     return digest;
 }

struct message *
get_message_1_svc(char *username, u_int msg_id,  struct svc_req *rqstp)
{
	static struct message  result;

	 Node* mesg;
	 mesg = search_msg(head_messages, msg_id);

	 if (mesg == NULL) {
		return NULL;
	 }

	 /* CALCULATE MD5*/
	 strcpy(((struct message*)mesg->data)->md5, md5sum(((struct message*)mesg->data)->text));
	 result = *(struct message*)mesg->data;

	return &result;
}
