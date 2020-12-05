#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */
#include <time.h>
#include <ctype.h>
#define sleep(x) Sleep(1000 * (x))

void *ThreadMain(void *arg);             /* Main program of a thread */

struct ThreadArgs /* Structure of arguments to pass to client thread */
{
    int clntSock;                      /* Socket descriptor for client */
    int num;
    char name[10];
    int career;
};
int cli[10]= {0},cnum=0,gamestart=0,killer=0,police=0,people=0,next=0,alive[8]= {1,1,1,1,1,1,1,1};
int killtemp=0,kdone=0;
int cdone=0,ctemp=0;
int career[8];
char player[8][10];
time_t t1,t2;
int main(int argc, char *argv[])
{
    srand( time(NULL) );
    struct ThreadArgs *threadArgs;   /* Pointer to argument structure for thread */
    DWORD  threadID;                 /* Thread ID from CreateThread() */
    char str[1024]="";
    int servSock;                    /* Socket descriptor for server */
    int clntSock,j=0,i,new;          /* Socket descriptor for client */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    if (WSAStartup(0x101, &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        printf("WSAStartup() failed");
        exit(1);
    }

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        printf("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(5678);              /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        printf("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, 3) < 0)
        printf("listen() failed");

    printf("Server is waiting for clients.\n");
    for (;;) /* Run forever */
    {
        clntLen = sizeof(echoClntAddr);

        clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen);

        if(cnum>=8)///更改人數改這<-
        {
            strcpy(str,"The number is full");
            send(clntSock,str,strlen(str)+1,0);
            closesocket(clntSock);
            break;
        }
        else
        {
            strcpy(str,"");
            recv(clntSock, str, 1000, 0);///接收玩家暱稱
            for(i=0; i<8; i++)
            {
                if(cli[i]==0)
                {
                    int c;
                    cli[i]=clntSock;
                    new=i;
                    cnum++;
                    printf("The %d client socket is in cli_sd[%d]\n",cnum,i);
                    threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
                    threadArgs -> clntSock = clntSock;
                    threadArgs -> num = new+1;
                    strcpy(threadArgs -> name,str);
                    strcpy(player[i],str);
                    itoa(new+1,str,10);
                    send(cli[new],str,strlen(str)+1,0);///回傳玩家號碼num
                    while(1)///分配職業
                    {
                        int x=rand();
                        x%=3;
                        if(x==0&&killer<2)
                        {
                            killer++;
                            c=1;
                            threadArgs ->career=c;
                            career[i]=c;
                            break;
                        }
                        else if(x==1&&police<2)
                        {
                            police++;
                            c=2;
                            threadArgs ->career=c;
                            career[i]=c;
                            break;
                        }
                        else if(x==2&&people<4)
                        {
                            people++;
                            c=3;
                            threadArgs ->career=c;
                            career[i]=c;
                            break;
                        }
                    }
                    printf("\n\tplayer num :%d\n\tname :%s\n\tcareer :%d",threadArgs->num,threadArgs->name,threadArgs->career);
                    strcpy(str,"");
                    itoa(c,str,10);
                    send(cli[new],str,strlen(str)+1,0);///回傳玩家職業
                    if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadMain, threadArgs, 0, (LPDWORD) &threadID) == NULL)
                        printf("thread_create() failed");

                    printf("New client with thread ID: %ld and socket:%d\n", threadID,clntSock);
                    printf("New client cnum:%d and cli_sd:%d\n", cnum,cli[cnum-1]);
                    break;
                }
            }
            strcpy(str,"newplayer!!!");
            for(j=0; j<10; j++)
            {
                if(cli[j]>0)
                {
                    if(j!=new)
                    {
                        send(cli[j],str,strlen(str)+1,0);
                    }
                }
            }
        }
        /* Create separate memory for client argument */

    }
    while(1)
    {

    }
}// end main

void *ThreadMain(void *threadArgs)
{
    int Round=1;///round
    int killer_alive=2,police_alive=2;
    int all_alive=8;
    int vote_done=0,vote_player=0,vote_num=0,vote[8]= {0};
    int next_ok[8]={0};
    int timeout = 50;
    int clntSock,j,i,num,step=1,c;
    char who[10];                  /* Socket descriptor for client connection */
    /* Extract socket file descriptor from argument */
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    num = ((struct ThreadArgs *) threadArgs) -> num;
    strcpy(who,((struct ThreadArgs *) threadArgs) -> name);
    c =((struct ThreadArgs *) threadArgs) ->career;
    char echoBuffer[1000];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
    /* Send received string and receive again until end of transmission */
    while(1)
    {
        if(cnum>=8&&gamestart==0)///更改人數改這<-
        {
            sleep(1);
            strcpy(who,"game are going to start...");
            send(clntSock, who, strlen(who)+1, 0);
            gamestart=1;
            strcpy(who,"gamestart");
            sleep(1);
            send(clntSock, who, strlen(who)+1, 0);
        }
        if(gamestart==1)///1.白天誰死了->2.自由說話->3.投票處死->4.黑夜殺手殺人->5.警察指認->...
        {
            while(1)
            {
                killtemp=0; kdone=0;
                cdone=0; ctemp=0;
                if(step==1)///白天---------------------------------------------------------------------------------------------------------------
                {
                    printf("\nstep1:%d\n",num);
                    printf("Day coming!!\n");
                    sleep(5);
                    strcpy(who,"Next step");
                    send(clntSock, who, strlen(who)+1, 0);
                    printf("sned Next step\n");
                    step++;
                }
                if(step==2)///自由對話------------------------------------------------------------------------------------------------------------
                {
                    printf("\nstep2:%d\n",num);
                    printf("free chat!!\n");
                    while (1)      // zero indicates end of transmission
                    {
                        // Receive message from client
                        //setsockopt(clntSock,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
                        if ((recvMsgSize = recv(clntSock, echoBuffer, 1000, 0)) < 0 )
                        {
                            int error_code=WSAGetLastError();
                            if(error_code==WSAECONNRESET)
                            {
                                printf("Host disconnected unexpectedly\n");
                                closesocket(clntSock);    // Close client socket
                                cli[num-1]=0;
                                cnum--;
                                break;
                            }
                            else
                                printf("recv failed with erroe code : %d\n",error_code);
                        }
                        // Echo message back to client
                        for(j = 0 ; j < 8 ; j++)
                        {
                            if(cli[j]!=clntSock)
                            {
                                sprintf(who, "%d", num);
                                //printf("str = %s\n", str);
                                strcat(who," : ");
                                strcat(who,echoBuffer);
                                send(cli[j], who, strlen(who)+1, 0);
                            }
                        }
                        printf("recv and send : %s\n",who);
                        if(strcmp(echoBuffer,"next_step")==0)
                        {
                            next++;
                            printf("next : %d\n",next);
                            while(1)
                            {
                                if(next>=all_alive)
                                {
                                    printf("next>=all_alive : %d\n",num);
                                    if(alive[num-1]==1)
                                    {
                                        strcpy(who,"Next step");
                                        send(clntSock, who, strlen(who)+1, 0);
                                    }
                                    break;
                                }
                            }
                        }
                        if(next>=all_alive)
                            break;
                    }
                    step++;
                }
                if(Round!=1)///投票------------------------------------------------------------------------------------------------------
                {
                    printf("\nstep3:%d\n",num);
                    if(step==3)
                    {
                        char pp[1024]="目前存活人數，請選擇代碼 ex:1";
                        send(clntSock, pp, strlen(pp)+1, 0);
                        for(j=0; j<8; j++)
                        {
                            if(alive[j]==1)
                            {
                                char temp[10]="";
                                itoa(j+1,temp,10);
                                strcpy(pp,temp);
                                strcat(pp," ");
                                strcat(pp,player[j]);
                                send(clntSock, pp, strlen(pp)+1, 0);
                            }
                        }
                        strcpy(pp,"please vote");
                        send(clntSock, pp, strlen(pp)+1, 0);
                        if ((recvMsgSize = recv(clntSock, echoBuffer, 1000, 0)) < 0)///接收代碼
                        {
                            int error_code=WSAGetLastError();
                            if(error_code==WSAECONNRESET)
                            {
                                printf("Host disconnected unexpectedly\n");
                                closesocket(clntSock);    // Close client socket
                                cli[num-1]=0;
                                cnum--;
                                break;
                            }
                            else
                                printf("recv failed with erroe code : %d\n",error_code);
                        }
                        else
                        {
                            int v=atoi(echoBuffer);
                            vote[v-1]++;
                            vote_done++;
                            while(1)
                            {
                                if(vote_done>=all_alive)
                                {
                                    int vmax=0;
                                    for(j=0; j<8; j++)///找票數最多
                                    {
                                        if(vote[j]>vote[vmax])
                                            vmax=j;
                                        vote[j]=0;
                                    }
                                    send(clntSock, pp, strlen(pp)+1, 0);
                                    step++;
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    printf("\nstep3:%d\n",num);
                    step++;
                }
                if(step==4)///殺手回合---------------------------------------------------------------------------------------------------------
                {
                    printf("\nstep4:%d\n",num);
                    sleep(1);
                    while(1)
                    {
                        if(c==1)///殺手
                        {
                            if ((recvMsgSize = recv(clntSock, echoBuffer, 1000, 0)) < 0)
                            {
                                int error_code=WSAGetLastError();
                                if(error_code==WSAECONNRESET)
                                {
                                    printf("Host disconnected unexpectedly\n");
                                    closesocket(clntSock);    // Close client socket
                                    cli[num-1]=0;
                                    cnum--;
                                    break;
                                }
                                else
                                    printf("recv failed with erroe code : %d\n",error_code);
                            }
                            else if(strcmp(echoBuffer,"kill")==0)///killer_state==1
                            {
                                char pp[1024]="The alive player，please choose the number.(ex:1)";
                                send(clntSock, pp, strlen(pp)+1, 0);
                                for(j=0; j<8; j++)
                                {
                                    if(alive[j]==1)///非殺手且活著
                                    {
                                        char temp[100]="";
                                        int t=j+1;
                                        itoa(t,temp,10);
                                        strcpy(pp,temp);
                                        strcat(pp," ");
                                        strcat(pp,player[j]);
                                        send(clntSock, pp, strlen(pp)+1, 0);
                                    }
                                }
                                recvMsgSize = recv(clntSock, echoBuffer, 1000, 0);
                                int x=atoi(echoBuffer);
                                killtemp=x;
                                kdone++;
                                for(j=0; j<8; j++)///傳給另一殺手你投給誰
                                {
                                    if(cli[j]!=clntSock&&career[j]==1)
                                    {
                                        if(alive[j]==1)
                                        {
                                            //printf("str = %s\n", str);
                                            strcpy(who,"the other killer voted : ");
                                            strcat(who,echoBuffer);
                                            send(cli[j], who, strlen(who)+1, 0);
                                        }
                                    }
                                }
                                break;
                            }
                            else
                            {
                                for(j=0; j<8; j++)///傳給另一殺手你的訊息
                                {
                                    if(cli[j]!=clntSock&&career[j]==1)
                                    {
                                        if(alive[j]==1)
                                        {
                                            sprintf(who, "%d", num);
                                            //printf("str = %s\n", str);
                                            strcat(who," : ");
                                            strcat(who,echoBuffer);
                                            send(cli[j], who, strlen(who)+1, 0);
                                        }
                                    }
                                }
                            }
                        }
                        else///非殺手
                        {
                            break;
                        }
                    }
                    while(1)
                    {
                        if(kdone>=killer_alive)
                        {
                            char deadplayer[10]="";
                            itoa(killtemp,deadplayer,10);
                            send(clntSock, deadplayer, strlen(deadplayer)+1, 0);///傳殺了誰(killer_state==2)
                            strcpy(who,"Next step");
                            send(clntSock, who, strlen(who)+1, 0);///next step指令
                            step++;
                            break;
                        }
                    }
                }
                if(step==5)///警察回合---------------------------------------------------------------------------------------------
                {
                    printf("\nstep5:%d\n",num);
                    while(1)
                    {
                        if(c==2)///警察
                        {
                            if ((recvMsgSize = recv(clntSock, echoBuffer, 1000, 0)) < 0)
                            {
                                int error_code=WSAGetLastError();
                                if(error_code==WSAECONNRESET)
                                {
                                    printf("Host disconnected unexpectedly\n");
                                    closesocket(clntSock);    // Close client socket
                                    cli[num-1]=0;
                                    cnum--;
                                    break;
                                }
                                else
                                    printf("recv failed with erroe code : %d\n",error_code);
                            }
                            if(strcmp(echoBuffer,"choose")==0)///police_state==1
                            {
                                char pp[1024]="目前存活人數，請選擇代碼 ex:1";
                                send(clntSock, pp, strlen(pp)+1, 0);
                                for(j=0; j<8; j++)
                                {
                                    if(alive[j]==1&&career[j]!=2)///非警察且活著
                                    {
                                        char temp[100]="";
                                        itoa(j+1,temp,10);
                                        strcpy(pp,temp);
                                        strcat(pp,player[j]);
                                        send(clntSock, pp, strlen(pp)+1, 0);
                                    }
                                }
                                recvMsgSize = recv(clntSock, echoBuffer, 1000, 0);
                                int x=atoi(echoBuffer);
                                ctemp=x;
                                cdone++;
                                for(j=0; j<8; j++)///傳給另一警察你投給誰
                                {
                                    if(cli[j]!=clntSock&&career[j]==2)
                                    {
                                        if(alive[j]==1)
                                        {
                                            //printf("str = %s\n", str);
                                            strcat(who,"the other police voted : ");
                                            strcat(who,echoBuffer);
                                            send(cli[j], who, strlen(who)+1, 0);
                                        }
                                    }
                                }
                                break;
                            }
                            else
                            {
                                for(j=0; j<8; j++)///傳給另一警察你的訊息
                                {
                                    if(cli[j]!=clntSock&&career[j]==1)
                                    {
                                        if(alive[j]==1)
                                        {
                                            sprintf(who, "%d", num);
                                            //printf("str = %s\n", str);
                                            strcat(who," : ");
                                            strcat(who,echoBuffer);
                                            send(cli[j], who, strlen(who)+1, 0);
                                        }
                                    }
                                }
                            }
                        }
                        else
                            break;
                    }
                    while(1)
                    {
                        if(cdone>=police_alive)
                        {
                            if(c==2)
                            {
                                char knowplayer[100]="";
                                char temp[100];
                                itoa(ctemp,temp,10);
                                if(career[ctemp-1]==1)
                                {
                                    strcpy(knowplayer,"player ");
                                    strcat(knowplayer,temp);
                                    strcat(knowplayer," killer!!");
                                }
                                else if(career[ctemp-1]==2)
                                {
                                    strcpy(knowplayer,"player ");
                                    strcat(knowplayer,temp);
                                    strcat(knowplayer," police!!");
                                }
                                else if(career[ctemp-1]==3)
                                {
                                    strcpy(knowplayer,"player ");
                                    strcat(knowplayer,temp);
                                    strcat(knowplayer," people!!");
                                }
                                send(clntSock, knowplayer, strlen(knowplayer)+1, 0);///傳殺了誰(killer_state==2)
                            }
                            strcpy(who,"Next step");
                            send(clntSock, who, strlen(who)+1, 0);///next step指令
                            step=1;
                            Round+=1;
                            break;
                        }
                    }
                }
                if(num==killtemp)
                {
                    all_alive-=1;
                    alive[killtemp-1]=0;
                    if(career[killtemp-1]==1)
                        killer_alive-=1;
                    else if(career[killtemp-1]==2)
                        police_alive-=1;
                }
            }
        }
    }
    free(threadArgs);
    return (NULL);
}
