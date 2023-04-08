#include "packets.h"
#include <stdint.h>

// only C -> S
enum PacketType str_toenum(char *type)
{
    if (!strcmp(type, "LIST"))
        return LIST;
    else if (!strcmp(type, "EXIT"))
        return EXIT;
    else if (!strcmp(type, "CHAT"))
        return CHAT;
    else if (!strcmp(type, "DOWN"))
        return DOWN;
    else if (!strcmp(type, "UP"))
        return UP;
    else
        return NONE;
}

struct packet makeinitpkt(char *raw_input)
{
    struct packet thispack = {0};
    char name[100] = {0};
    char args[100] = {0};

    raw_input[strcspn(raw_input, "\n")] = 0;

    strcpy(thispack.username, raw_input);
    enum PacketType thistype = str_toenum(raw_input);
    thispack.mtype = thistype; //NONE 0
    thispack.ptype = INIT;

    return thispack;
}

struct packet makeauthpkt(int mtype, int subtype) //make chatauth (DO WILL WONT for chat)
{
    struct packet thispkt = {0};
    thispkt.mtype = mtype;
    thispkt.ptype = AUTH;

    switch (subtype)
    {
    case 0: // Do
        thispkt.F_Do = true;
        thispkt.F_Will = false;
        thispkt.F_Wont = false;
        break;
    case 1: // Will
        thispkt.F_Do = false;
        thispkt.F_Will = true;
        thispkt.F_Wont = false;
        break;
    case 2: // Wont
        thispkt.F_Do = false;
        thispkt.F_Will = false;
        thispkt.F_Wont = true;
        break;
    }
    return thispkt;
}

struct packet makemessagepkt(char *raw_input)//from fgets.
{   
    struct packet thispack = {0};
    char mtype[100] = {0};
    char args[900] = {0};
    //printf("raw input")

    sscanf(raw_input, "%s %[^\n]", mtype, args);
    // printf("mtype: %s\n", mtype); //str
    // printf("args: %s\n", args);
    enum PacketType thistype = str_toenum(mtype); // NONE if raw chat message

    thispack.ptype = MESSAGE;
    thispack.mtype = thistype; //NONE 0

    if(thispack.mtype == NONE && thispack.ptype != INIT){
        // puts("inside");
        char temp[1024] = {0};
        sprintf(temp, "%s %s", mtype, args);
        // printf("temp: %s\n", temp);
        strcpy(thispack.msg_args, temp);
    }
    else{
        strcpy(thispack.msg_args, args);
    }

    return thispack;

}

int sendpacket(struct packet thispkt, int sockfd)
{
    // serialize only first 2 fields: username and inchat.
    int bytes_sent = 0;
    int packet_size = sizeof(thispkt);
    char *buf = (char *)malloc(packet_size);

    memcpy(buf, &thispkt, sizeof(thispkt));

    while (bytes_sent < packet_size) {
        int result = send(sockfd, buf + bytes_sent,  packet_size - bytes_sent, 0);
        if (result == -1) {
            // handle error
            return -1;
        }
        bytes_sent += result;
    }
    return bytes_sent;
}

int recpacket(struct packet *thispkt, int sockfd)
{
    char *tempbuf = (char *)malloc(sizeof(*thispkt)); //sizeofpkt = 8.
    int bytes_remaining = sizeof(*thispkt); 
   // printf("size pack: %d\n", bytes_remaining);
    int total_bytes_received = 0;

    while (bytes_remaining > 0) {
        int bytes_received = recv(sockfd, tempbuf + total_bytes_received, bytes_remaining, 0);
        if (bytes_received == -1) {
            // handle error
        } else if (bytes_received == 0) {
            // handle connection closed
        } else {
            //printf("bytes recd: %d\n", bytes_received);
            total_bytes_received += bytes_received;
            bytes_remaining -= bytes_received;
        }
    }
    //printf("recd fn: %d\n", total_bytes_received);

    // memcpy sizeof(type): sizeof(struct)
    memcpy((void *)thispkt, (void *)tempbuf, sizeof(*thispkt));
    //printf("%s\n", thispkt->msg_args);
    free(tempbuf);
    return total_bytes_received;
}

// int main(){
//     struct packet thispack = {0};
//     recpacket(&thispack, 3);
//     return 0;

// }


