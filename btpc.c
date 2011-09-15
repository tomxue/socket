/*
 * =====================================================================================
 *
 *       Filename:  btpc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/15/11 04:14:10
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
int main( int argc , char **argv)
{
       struct sockaddr_rc addr={0};
       int s,status;
       char *dest,*buf; //="00:11:67:32:61:62";
   
       void baswap(bdaddr_t *dst, const bdaddr_t *src)
         {
register unsigned char *d = (unsigned char *) dst;
register const unsigned char *s = (const unsigned char *) src;
register int i;
for (i = 0; i < 6; i++)
      d[i] = s[5-i];
         }

       int str2ba(const char *str, bdaddr_t *ba)
         {
uint8_t b[6];
const char *ptr = str;
int i;

for (i = 0; i < 6; i++) {
      b[i] = (uint8_t) strtol(ptr, NULL, 16);
      if (i != 5 && !(ptr = strchr(ptr, ':')))
       ptr = ":00:00:00:00:00";
      ptr++;
}
baswap(ba, (bdaddr_t *) b);
return 0;
         }

       if(argc==2)
         {
           dest=argv[1];
         }
       else
         {
           printf("prarm error\n");
           exit(1);
         }
   
       // allocatea socket
       s=socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);
       if(s<0)
         {
           perror("create socket error");
           exit(1);
         }
   
       // set the connection parameters (who to connect to )
       addr.rc_family=AF_BLUETOOTH;
       addr.rc_channel=(uint8_t)1;
       str2ba(dest,&addr.rc_bdaddr);
   
       // connect to server
       printf("connectting...\n");
       status=connect(s,(struct sockaddr *)&addr,sizeof(addr));
   
       // send a message
       if(status==0){
         printf("scuess!\n");
         status=write(s,"hello!",6);

       close(s);
       return 0;
}

}
