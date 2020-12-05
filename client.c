#include "stdio.h"
#include "string.h"
#include "windows.h"
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#define sleep(x) Sleep(1000 * (x))

char reg_name[30]="",reg_pwd[10]="",input[1024]="";
char on_name[30],on_pwd[10],Die[10],c_ans[100];
int test=0,playernum=1,Round=1,step=1,kill=0,n,career,killer_state=0;//round
int police_state=0;
int gamestart=0,speakable=1,my_num,output=0,alive=1;
int vote_num=0,vote_state=0;
struct Player
{
    char name[10];
    int num;
    int career;
};
struct Player user;
SOCKET        	sd;
struct sockaddr_in serv;
char  		str[1024];
char  		str_r[1024];
WSADATA 		wsadata;
int timeout = 50,n;
pthread_t thread1;

///用戶註冊系統
void regist()
{
    system("cls");
    printf("\n\n\t\t\t歡迎使用用狼人殺註冊系統\n\n");
    while(1)
    {
        //輸入用戶名
        printf("\t\t請輸入暱稱[不能超過10個字]：");
        scanf("%s",reg_name);

        //判斷暱稱
        if(strlen(reg_name)<=10)
        {
            while(1)
            {
                //輸入入密碼
                printf("\n\t\t請輸入密碼[密碼長度為五位數]：");
                scanf("%s",reg_pwd);

                //判斷密碼
                if(strlen(reg_pwd)==5)
                {
                    printf("\n\n\t\t已註冊完成!!! 您的暱稱是%s,密碼是%s\n\n",reg_name,reg_pwd);
                    system("pause");
                    system("cls");
                    break;
                }
                else
                {
                    printf("\n\t\t密碼的長度為%d，請重新輸入\n",strlen(reg_pwd));
                    //system("pause");
                    sleep(0);
                }
            }
            break;
        }
        else
        {
            printf("\n\t\t暱稱的長度為%d，請重新輸入\n\n",strlen(reg_name));
            sleep(0);
        }
    }
}

void *recvMess(void *argu)      ///接收
{
    //printf("等待配對中......(%d/8)\n",playernum);
    while (1)
    {
        //setsockopt(sd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
        n=recv(sd, str_r, 1024, 0); //接收來自其他client的訊息
        if(n>1)
        {//strcpy
            printf("recv: %s (%d bytes)\n",str_r,strlen(str_r)+1);
            if(strcmp(str_r,"newplayer!!!")==0)
            {
                playernum++;
                printf("new player join\n");
            }
            else if(strcmp(str_r,"gamestart")==0)
            {
                gamestart=1;
                printf("gamestart=%d\n",gamestart);
                strcpy(str_r,"");
                while(1)///1.白天誰死了->2.自由說話->3.投票處死->4.黑夜殺手殺人->5.警察指認->...
                {
                    if(step==1)///白天------------------------------------------
                    {
                        if(kill!=0)
                        {
                            if(atoi(Die)==user.num)
                            {
                                printf("You are Dead...\n");
                                alive=0;
                            }
                            else
                                printf("%s was killed...\n",Die);
                        }
                        n=recv(sd, str_r, 1024, 0); //接收來自其他client的訊息
                        if(strcmp(str_r,"Next step")==0)
                        {
                            printf("%s\n\n",str_r);
                            step++;
                            continue;
                        }
                    }
                    if(step==2)///自由對話-------------------------------------
                    {
                        strcpy(str_r,"");
                        while(1)
                        {
                            n=recv(sd, str_r, 1024, 0); //接收來自其他client的訊息
                            if(strcmp(str_r,"Next step")==0)
                            {
                                printf("%s\n\n",str_r);
                                step++;
                                break;
                            }
                            if(n>5)
                                printf("recv: %s (%d bytes)\n",str_r,strlen(str_r)+1);
                        }
                    }
                    if(Round!=1)///投票-----------------------------------------
                    {
                        if(step==3)
                        {
                            while(1)///接收存活名單
                            {
                                n=recv(sd, str_r, 1024, 0);
                                if(n>1)
                                {
                                    printf("%s\n",str_r,strlen(str_r)+1);
                                    if(strcmp(str_r,"please vote")==0)
                                    {
                                        vote_state=1;
                                        break;
                                    }
                                }
                            }
                            n=recv(sd, str_r, 10, 0);///接收投票結果
                            if(n>5)
                            {
                                vote_num=atoi(str_r);
                                if(user.num==vote_num)
                                {
                                    printf("you are dead!!\n");
                                    alive=0;
                                }
                                else
                                {
                                    printf("player %d dead!!\n",vote_num);
                                }
                            }
                            while(1)
                            {
                                n=recv(sd, str_r, 100, 0);///接收next step指令
                                if(strcmp(str_r,"Next step")==0)
                                {
                                    printf("%s\n",str_r);
                                    step++;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        sleep(1);
                        step++;
                    }
                    if(step==4)///殺手回合----------------------------------------
                    {
                        printf("midnight，killer start...\n");
                        strcpy(str_r,"");
                        while(1)
                        {
                            if(user.career!=1)///非殺手
                            {
                                n=recv(sd, Die, 10, 0);///收到死者代碼(killer_state==2)
                                if(n>5)
                                {
                                    kill++;
                                    int x=atoi(Die);
                                    if(x==user.num)
                                    {
                                        alive=0;
                                    }
                                }
                                n=recv(sd, str_r, 1000, 0);
                                if(strcmp(str_r,"Next step")==0)///收到next step指令
                                {
                                    step++;
                                    break;
                                }
                            }
                            else///殺手
                            {
                                n=recv(sd, str_r, 1024, 0);
                                if(n>5)
                                {
                                    if(strcmp(str_r,"Next step")==0)
                                    {
                                        step++;
                                        break;
                                    }
                                    if(killer_state==0)///殺手聊天階段
                                        printf("recv: %s (%d bytes)\n",str_r,strlen(str_r)+1);
                                    else if(killer_state==1)///收到存活名單
                                    {
                                        printf("%s\n",str_r);
                                    }
                                    else if(killer_state==2)///收到死者代碼
                                    {
                                        n=recv(sd, Die, 10, 0);
                                        if(n>1)
                                        {
                                            kill++;
                                            int x=atoi(Die);
                                            if(x==user.num)
                                            {
                                                alive=0;
                                            }
                                        }
                                        killer_state=0;///回到階段0仍可接收到來自另一殺手的訊息，但不能說話了
                                    }
                                }
                            }
                        }
                    }
                    if(step==5)///警察回合----------------------------------------------
                    {
                        printf("midnight，police start...\n");
                        while(1)
                        {
                            if(user.career!=2)
                            {
                                n=recv(sd, str_r, 1024, 0);
                                if(strcmp(str_r,"Next step")==0)
                                {
                                    step=1;
                                    Round+=1;
                                    break;
                                }
                            }
                            else
                            {
                                n=recv(sd, str_r, 1024, 0);
                                if(n>5)
                                {
                                    if(police_state==0)
                                        printf("recv: %s (%d bytes)\n",str_r,strlen(str_r)+1);
                                    else if(police_state==1)
                                    {
                                        printf("%s\n",str_r);
                                    }
                                    else if(police_state==2)
                                    {
                                        n=recv(sd, c_ans, 100, 0);
                                        if(n>1)
                                        {
                                            printf("%s\n",c_ans);
                                        }
                                        n=recv(sd, str_r, 1024, 0);
                                        if(strcmp(str_r,"Next step")==0)
                                        {
                                            step=1;
                                            Round+=1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Gameplay()///輸入
{
    int k=0,w=0;
    while(1)
    {
        //printf("waiting for playerww......\n");
        if(gamestart==0)
        {
            sleep(1);
            printf("waiting for playerww gamestart=%d......\n",gamestart);
        }
        if(gamestart!=0)
        {
            //system("cls");
            printf("game start!!!\n");
            break;
        }
        continue;
    }
    int d=0;
    while(1)///1.白天誰死了->2.自由說話->3.投票處死->4.黑夜殺手殺人->5.警察指認->...
    {
        //if(alive==0)
            //break;
        if(step==1)
        {
            printf("\n\nstep:1\n");
            printf("\n\n\nDay coming!!\n");
            if(Round==1)
            {
                if(user.career==1)
                {
                    printf("You are Killer !!\n");
                }
                else if(user.career==2)
                {
                    printf("You are Police !!\n");
                }
                else if(user.career==3)
                {
                    printf("You are People !!\n");
                }
                while(1)
                {
                    if(step==2)
                        break;
                }
            }
            else
            {
                while(1)
                {
                    if(step==2)
                        break;
                }
                //continue;
            }
        }
        if(step==2)
        {
            printf("\n\nstep:2\n");
            strcpy(input,"");
            printf("Round #%d!!\n",Round);
            printf("Dead : %d\n",kill);
            printf("free chat time~\n");
            printf("輸入next_step來準備進入下一階段\n");
            send(sd, input, strlen(input)+1, 0);
            while(1)
            {
                if(alive==0)
                {
                    printf("請輸入遺言:\n");
                    gets(input);
                    strcat(input,"(遺言)");
                    send(sd, input, strlen(input)+1, 0);
                    //closesocket(sd);
                    break;
                }
                gets(input);
                send(sd, input, strlen(input)+1, 0);
                if(strcmp(input,"next_step")==0)
                {
                    break;
                }

            }
            if(alive==0)
                break;
            while(1)
            {
                if(step==3)
                {
                    printf("next step to 3\n");
                    break;
                }
                else
                    continue;
            }
        }
        if(Round!=1)
        {
            printf("\n\nstep:3\n");
            if(step==3)
            {
                while(1)
                {
                    if(vote_state==1)
                        break;
                }
                char vo[10];
                sleep(1);
                printf("please vote the number : ");
                gets(vo);
                send(sd, vo, strlen(vo)+1, 0);
            }
            while(1)
            {
                if(step==4)
                    break;
            }
        }
        else
        {
            printf("\n\nstep:3\n");
            while(1)
            {
                if(step==4)
                {
                    break;
                }
                continue;
            }
        }
        if(step==4)
        {
            printf("\n\nstep:4\n");
            if(user.career==1)///殺手
            {
                printf("nigth，killer could chat(killcode=kill)\n");
                while(1)
                {
                    gets(input);
                    send(sd, input, strlen(input)+1, 0);///殺手對話
                    if(strcmp(input,"kill")==0)///kill指令
                    {
                        killer_state=1;
                        strcpy(input,"");
                        sleep(1);
                    }
                    if(killer_state==1)///輸入要殺的人
                    {
                        gets(input);
                        send(sd, input, strlen(input)+1, 0);
                        killer_state=2;
                        break;
                    }
                }
                while(1)
                {
                    if(step==5)
                    {
                        break;
                    }
                }
            }
            else
            {
                while(1)
                {
                    if(step==5)
                    {
                        break;
                    }
                }
            }
        }
        if(step==5)
        {
            printf("\n\nstep:5\n");
            if(user.career==2)///警察
            {
                printf("nigth，police could chat(police code=choose)\n");
                while(1)
                {
                    gets(input);
                    send(sd, input, strlen(input)+1, 0);///警察對話
                    if(strcmp(input,"choose")==0)///choose指令
                    {
                        police_state=1;
                    }
                    if(police_state==1)///輸入要指認的人
                    {
                        gets(input);
                        send(sd, input, strlen(input)+1, 0);
                        police_state=2;
                    }
                    break;
                }
                while(1)
                {
                    if(step==1)
                    {
                        break;
                    }
                }
            }
            else
            {
                while(1)
                {
                    if(step==1)
                    {
                        break;
                    }
                }
            }
        }
    }

}

void Connect_server()
{
    WSAStartup(0x101,(LPWSADATA) &wsadata); // 呼叫 WSAStartup() 註冊 WinSock DLL 的使用


    sd=socket(AF_INET, SOCK_STREAM, 0); //開啟一個 TCP socket.

    serv.sin_family       = AF_INET;
    serv.sin_addr.s_addr  = inet_addr("127.0.0.1");
    serv.sin_port         = htons(5678);

    connect(sd, (LPSOCKADDR) &serv, sizeof(serv)); // 連接至server
    send(sd, reg_name, strlen(reg_name)+1, 0);
    n=recv(sd, str_r, 1024, 0); //接收來自其他client的訊息
    if(n>1)
    {
        my_num=atoi(str_r);
        printf("my number is : %d\n",my_num);
    }
    n=recv(sd, str_r, 1024, 0); //接收來自其他client的訊息
    if(n>1)
    {
        career=atoi(str_r);
        printf("my Career is : %d\n",career);
    }
    user.career=career;
    strcpy(user.name,on_name);
    user.num=my_num;
}
//判斷是否註冊
int judge()
{
    if(strcmp(reg_name,"")==0&&strcmp(reg_pwd,"")==0)
    {
        printf("\n\n\t\t您尚未註冊，請先註冊！\n\n");
        //system("pause");
        printf("\t\t兩秒後自動跳轉...");
        sleep(2);
        system("cls");
        return 0;
    }
    else
    {
        return 1;
    }
}

//用戶登入
void dl()
{
    int i;
    system("cls");
    printf("\n\n\t\t\t歡迎使用狼人殺登入系統\n\n");
    //三次登入機會
    for(i=1; i<=3; i++)
    {
        printf("\t\t請輸入暱稱:");
        scanf("%s",on_name);
        printf("\n\t\t請輸入密碼:");
        scanf("%s",on_pwd);
        if(strcmp(reg_name,on_name)==0&&strcmp(reg_pwd,on_pwd)==0)
        {
            printf("\n\n\t\t歡迎遊玩狼人殺!!\n\n");
            test++;
            //system("pause");
            printf("\n\n\t\t導向遊戲選單頁面.....\n\n");
            sleep(1);
            system("cls");
            break;
        }
        else
        {
            printf("\n\n\t\t登入失敗，請重新登入，您還有%d次機會\n\n",3-i);
            //system("pause");
            if(3-i==0)
            {
                printf("\t\t兩秒後自動跳轉...");
                sleep(2);
            }
        }
    }
}

//遊戲規則
void game_rule()
{
    int y;
    system("cls");
    printf("\n\n\t\t\t遊戲規則\n\n");
    printf("\n\n\t\t\t111\n\n");

    system("pause");
}

int Main_Menu()
{
    int id;
    system("cls");
    //輸出界面
    printf("\n\n\t\t\t歡迎遊玩狼人殺\n\n");

    printf("\t\t\t1:註冊\n");
    printf("\t\t\t2:登入\n");
    printf("\t\t\t0:離開\n\n");

    //輸入功能號碼
    printf("\t\t請選擇號碼：");
    scanf("%d",&id);
    return id;
}

int Game_Menu()
{
    int id2;
    system("cls");
    printf("\n\n\t\t\t遊戲選單\n\n");
    printf("\n\n\t\t\t嗨! %s\n\n",on_name);
    printf("\t\t\t1:遊戲匹配\n");
    printf("\t\t\t2:遊戲規則\n");
    printf("\t\t\t0:登出\n\n");

    printf("\t\t請選擇號碼：");
    scanf("%d",&id2);
    return id2;
}

int main()
{
    int id,id2,out;
    int y_or_n;

    while(1)///登入畫面
    {
        int test2=0;
        /*
        while(1)///主選單
        {
            id=Main_Menu();
            switch(id)
            {
                case 1:
                    regist();///註冊
                    break;
                case 2:
                    if(judge()>0)
                        dl();///登入
                    break;
                case 0:
                    printf("\n\t\t\t確定真的要離開遊戲嗎TAT\n\n");
                    printf("\t\t\t1:是\n");
                    printf("\t\t\t2:否\n");
                    printf("\n\t\t請選擇號碼：");
                    scanf("%d",&y_or_n);
                    switch(y_or_n)
                    {
                        case 1:
                            exit(1);
                            break;
                        case 2:
                            continue;
                            break;
                        default:
                            printf("\n\t\t輸入的號碼有誤，請回到選單再次輸入\n");
                            //system("pause");
                            printf("\t\t兩秒後自動跳轉...");
                            sleep(2);
                            system("cls");
                    }
                    break;
                default:
                    printf("\n\t\t輸入的號碼有誤，請再次輸入\n");
                    printf("\t\t兩秒後自動跳轉...");
                    sleep(2);
                    //system("pause");
                    system("cls");
            }
            if(test==1)
                break;
        }
        */
        while(1)///遊戲選單
        {
            printf("\t\t請輸入暱稱[不能超過10個字]：");
            scanf("%s",reg_name);
            //sleep(2);
            //id2=Game_Menu();
            id2=1;
            switch(id2)
            {
            case 1:
                system("cls");
                printf("\t\t\t\nConnect to server\n");
                Connect_server();///連結至server
                pthread_create(&thread1, NULL, &recvMess, NULL);///開啟多緒
                Gameplay();
                break;
            case 2:
                game_rule();
                break;
            case 0:
                printf("\n\t\t\t確定真的要登出嗎TAT\n\n");
                printf("\t\t\t1:是\n");
                printf("\t\t\t2:否\n");
                printf("\n\t\t請選擇號碼：");
                scanf("%d",&out);

                if(out==1)
                {
                    printf("\t\t登出中...");
                    sleep(1);
                    test2++;
                }
                else if(out==2)
                    continue;
                else
                {
                    printf("\n\t\t輸入的號碼有誤，請回到選單再次輸入\n");
                    printf("\t\t兩秒後自動跳轉...");
                    sleep(2);
                    system("cls");
                }
                break;
            }
            if(test2!=0)
                break;
        }

    }
}
