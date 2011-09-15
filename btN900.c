/*
 * =====================================================================================
 *
 *       Filename:  btN900.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/15/11 04:15:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tom Xue (), tom.xue@nokia.com
 *        Company:  Nokia
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
int main (int argc,char **argv)
{
     struct sockaddr_rc loc_addr ={0},rem_addr={0};
     char buf[1024] ={0},*addr;
     int s,client, bytes_read,result;
     int opt = sizeof(rem_addr);
  
     printf("Creating socket...\n");
     s=socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);
     if(s<0)
       {
         perror("create socket error");
         exit(1);
       }
     else
       {
         printf("success!\n");
       }

     loc_addr.rc_family=AF_BLUETOOTH;
     loc_addr.rc_bdaddr=*BDADDR_ANY;
     loc_addr.rc_channel=(uint8_t)1;
  
     printf("Binding socket...\n");
     result=bind(s,(struct sockaddr *)&loc_addr, sizeof(loc_addr));
     if(result<0)
       {
         perror("bind socket error:");
         exit(1);
       }
     else
       {
         printf("success!\n");
       }
   
     void baswap(bdaddr_t *dst, const bdaddr_t *src)
       {
register unsigned char *d = (unsigned char *) dst;
register const unsigned char *s = (const unsigned char *) src;
register int i;
for (i = 0; i < 6; i++)
    d[i] = s[5-i];
       }

     int ba2str(const bdaddr_t *ba, char *str)
       {
uint8_t b[6];

baswap((bdaddr_t *) b, ba);
return sprintf(str, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
    b[0], b[1], b[2], b[3], b[4], b[5]);
       }

     /*result=ba2str(&loc_addr.rc_bdaddr,addr);
     if(result<0)
       {
         perror("addr convert error");
         exit(1);
       }
       printf("local addr is:%s\n",addr);*/
     printf("Listen... ");
     result=listen(s,1);
     if(result<0)
       {
         printf("error:%d\n:",result);
         perror("listen error:");
         exit(1);
       }
     else
       {
         printf("requested!\n");
       }

     printf("Accepting...\n");
     client= accept(s,(struct sockaddr *)&rem_addr,&opt);
     if(client<0)
       {
         perror("accept error");
         exit(1);
       }
     else
       {
         printf("OK!\n");
       }
     ba2str(&rem_addr.rc_bdaddr,buf);
     fprintf(stderr,"accepted connection from %s \n",buf);
     memset(buf,0,sizeof(buf));

     while(1)
       {
         bytes_read = read(client,buf,sizeof(buf));
         if(bytes_read>0){
       printf("received[%s]\n",buf);
         exit(1);
         }
       }


     close(client);
     close(s);
     return 0 ;
}



