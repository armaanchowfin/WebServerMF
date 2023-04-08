#include "DLLNode.h"


struct DLLNode *head = NULL; // ensures we have a global DLL.
struct DLLNode *tail = NULL;


struct DLLNode *createUser(int _sockfd){
    struct DLLNode *thisuser = (struct DLLNode *)malloc(sizeof(struct DLLNode));

    thisuser->prev = NULL;
    thisuser->state.s_sockfd = _sockfd;
	//strcpy(thisuser->state.clientname, name);
	thisuser->state.inchat = false;
    thisuser->next = NULL;

    //threadhandler, client num, connectedclientname updated by thread func.

    return thisuser;
}

struct DLLNode *addUser(int _sockfd){
	struct DLLNode*thisuser = createUser(_sockfd);

    //empty list
	if(head == NULL) {
		thisuser->prev = NULL;
		head = thisuser;
		tail = head;
	}
	else{
		thisuser->prev = tail;
		tail->next = thisuser;
		tail = tail->next;
	}
    return thisuser;
}

struct DLLNode *findUserByName(char *_clientname){
	struct DLLNode *listPointer = head;

	//if head is null then the list is empty
	if(head == NULL) {
		//puts("here0");
		return NULL;
	}
	else {
		listPointer = head; //start at the beginning
	
		while(listPointer != NULL) {
			//puts("here1");
			//printf("Listpointer name: %sx\n", listPointer->state.clientname);
			//printf("Client name: %sx\n", _clientname);
			if(!strcmp(listPointer->state.clientname, _clientname)) {
				return listPointer;
			}
			else {
				listPointer = listPointer->next;
			}
		}
	}
    return NULL; //user not connected.

}

void removeUserByName(char *_clientname){

    struct DLLNode *toremove = findUserByName(_clientname);
	if(head == tail) {
		//delete single item list
		head = NULL;
		tail = NULL;
	}
	else if(toremove == head) {
		//delete head
		head = toremove->next;		
		head->prev = NULL;
	} 
	else if(toremove == tail) {
		//delete tail
		tail = toremove->prev;
		tail->next = NULL;
	}
	else {
		//delete non head/tail record
		toremove->next->prev = toremove->prev;
		toremove->prev->next = toremove->next;
	}	

	//free the memory location
	free(toremove);
}

	//returns pointer to space separated users
char *getDLLNodes(){
	char *users = (char *)malloc(100 * MAXCLIENTS);
	memset(users, 0, 100 * MAXCLIENTS *sizeof(char));
	struct DLLNode *curr = head;
	int i = 0;

	do{
		int len = (int)strlen(curr->state.clientname) + 1;//includes \0
		memcpy(users + i, curr->state.clientname, strlen(curr->state.clientname) + 1);// EOF issue
		*(users + len + i - 1) = ' ';//replace \0 with space.

		i +=  len; //i = i + size +1;
		curr = curr->next;
	}while(curr !=NULL);

	*(users + i) = '\n'; //as per makemessagepacket.
	*(users + i + 1) = '\0';

	return users;
}