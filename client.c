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

///�Τ���U�t��
void regist()
{
    system("cls");
    printf("\n\n\t\t\t�w��ϥΥίT�H�����U�t��\n\n");
    while(1)
    {
        //��J�Τ�W
        printf("\t\t�п�J�ʺ�[����W�L10�Ӧr]�G");
        scanf("%s",reg_name);

        //�P�_�ʺ�
        if(strlen(reg_name)<=10)
        {
            while(1)
            {
                //��J�J�K�X
                printf("\n\t\t�п�J�K�X[�K�X���׬������]�G");
                scanf("%s",reg_pwd);

                //�P�_�K�X
                if(strlen(reg_pwd)==5)
                {
                    printf("\n\n\t\t�w���U����!!! �z���ʺ٬O%s,�K�X�O%s\n\n",reg_name,reg_pwd);
                    system("pause");
                    system("cls");
                    break;
                }
                else
                {
                    printf("\n\t\t�K�X�����׬�%d�A�Э��s��J\n",strlen(reg_pwd));
                    //system("pause");
                    sleep(0);
                }
            }
            break;
        }
        else
        {
            printf("\n\t\t�ʺ٪����׬�%d�A�Э��s��J\n\n",strlen(reg_name));
            sleep(0);
        }
    }
}

void *recvMess(void *argu)      ///����
{
    //printf("���ݰt�襤......(%d/8)\n",playernum);
    while (1)
    {
        //setsockopt(sd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
        n=recv(sd, str_r, 1024, 0); //�����Ӧۨ�Lclient���T��
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
                while(1)///1.�դѽ֦��F->2.�ۥѻ���->3.�벼�B��->4.�©]������H->5.ĵ����{->...
                {
                    if(step==1)///�դ�------------------------------------------
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
                        n=recv(sd, str_r, 1024, 0); //�����Ӧۨ�Lclient���T��
                        if(strcmp(str_r,"Next step")==0)
                        {
                            printf("%s\n\n",str_r);
                            step++;
                            continue;
                        }
                    }
                    if(step==2)///�ۥѹ��-------------------------------------
                    {
                        strcpy(str_r,"");
                        while(1)
                        {
                            n=recv(sd, str_r, 1024, 0); //�����Ӧۨ�Lclient���T��
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
                    if(Round!=1)///�벼-----------------------------------------
                    {
                        if(step==3)
                        {
                            while(1)///�����s���W��
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
                            n=recv(sd, str_r, 10, 0);///�����벼���G
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
                                n=recv(sd, str_r, 100, 0);///����next step���O
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
                    if(step==4)///����^�X----------------------------------------
                    {
                        printf("midnight�Akiller start...\n");
                        strcpy(str_r,"");
                        while(1)
                        {
                            if(user.career!=1)///�D����
                            {
                                n=recv(sd, Die, 10, 0);///���즺�̥N�X(killer_state==2)
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
                                if(strcmp(str_r,"Next step")==0)///����next step���O
                                {
                                    step++;
                                    break;
                                }
                            }
                            else///����
                            {
                                n=recv(sd, str_r, 1024, 0);
                                if(n>5)
                                {
                                    if(strcmp(str_r,"Next step")==0)
                                    {
                                        step++;
                                        break;
                                    }
                                    if(killer_state==0)///�����Ѷ��q
                                        printf("recv: %s (%d bytes)\n",str_r,strlen(str_r)+1);
                                    else if(killer_state==1)///����s���W��
                                    {
                                        printf("%s\n",str_r);
                                    }
                                    else if(killer_state==2)///���즺�̥N�X
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
                                        killer_state=0;///�^�춥�q0���i������Ӧۥt�@���⪺�T���A�����໡�ܤF
                                    }
                                }
                            }
                        }
                    }
                    if(step==5)///ĵ��^�X----------------------------------------------
                    {
                        printf("midnight�Apolice start...\n");
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

void Gameplay()///��J
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
    while(1)///1.�դѽ֦��F->2.�ۥѻ���->3.�벼�B��->4.�©]������H->5.ĵ����{->...
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
            printf("��Jnext_step�ӷǳƶi�J�U�@���q\n");
            send(sd, input, strlen(input)+1, 0);
            while(1)
            {
                if(alive==0)
                {
                    printf("�п�J��:\n");
                    gets(input);
                    strcat(input,"(��)");
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
            if(user.career==1)///����
            {
                printf("nigth�Akiller could chat(killcode=kill)\n");
                while(1)
                {
                    gets(input);
                    send(sd, input, strlen(input)+1, 0);///������
                    if(strcmp(input,"kill")==0)///kill���O
                    {
                        killer_state=1;
                        strcpy(input,"");
                        sleep(1);
                    }
                    if(killer_state==1)///��J�n�����H
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
            if(user.career==2)///ĵ��
            {
                printf("nigth�Apolice could chat(police code=choose)\n");
                while(1)
                {
                    gets(input);
                    send(sd, input, strlen(input)+1, 0);///ĵ����
                    if(strcmp(input,"choose")==0)///choose���O
                    {
                        police_state=1;
                    }
                    if(police_state==1)///��J�n���{���H
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
    WSAStartup(0x101,(LPWSADATA) &wsadata); // �I�s WSAStartup() ���U WinSock DLL ���ϥ�


    sd=socket(AF_INET, SOCK_STREAM, 0); //�}�Ҥ@�� TCP socket.

    serv.sin_family       = AF_INET;
    serv.sin_addr.s_addr  = inet_addr("127.0.0.1");
    serv.sin_port         = htons(5678);

    connect(sd, (LPSOCKADDR) &serv, sizeof(serv)); // �s����server
    send(sd, reg_name, strlen(reg_name)+1, 0);
    n=recv(sd, str_r, 1024, 0); //�����Ӧۨ�Lclient���T��
    if(n>1)
    {
        my_num=atoi(str_r);
        printf("my number is : %d\n",my_num);
    }
    n=recv(sd, str_r, 1024, 0); //�����Ӧۨ�Lclient���T��
    if(n>1)
    {
        career=atoi(str_r);
        printf("my Career is : %d\n",career);
    }
    user.career=career;
    strcpy(user.name,on_name);
    user.num=my_num;
}
//�P�_�O�_���U
int judge()
{
    if(strcmp(reg_name,"")==0&&strcmp(reg_pwd,"")==0)
    {
        printf("\n\n\t\t�z�|�����U�A�Х����U�I\n\n");
        //system("pause");
        printf("\t\t����۰ʸ���...");
        sleep(2);
        system("cls");
        return 0;
    }
    else
    {
        return 1;
    }
}

//�Τ�n�J
void dl()
{
    int i;
    system("cls");
    printf("\n\n\t\t\t�w��ϥίT�H���n�J�t��\n\n");
    //�T���n�J���|
    for(i=1; i<=3; i++)
    {
        printf("\t\t�п�J�ʺ�:");
        scanf("%s",on_name);
        printf("\n\t\t�п�J�K�X:");
        scanf("%s",on_pwd);
        if(strcmp(reg_name,on_name)==0&&strcmp(reg_pwd,on_pwd)==0)
        {
            printf("\n\n\t\t�w��C���T�H��!!\n\n");
            test++;
            //system("pause");
            printf("\n\n\t\t�ɦV�C����歶��.....\n\n");
            sleep(1);
            system("cls");
            break;
        }
        else
        {
            printf("\n\n\t\t�n�J���ѡA�Э��s�n�J�A�z�٦�%d�����|\n\n",3-i);
            //system("pause");
            if(3-i==0)
            {
                printf("\t\t����۰ʸ���...");
                sleep(2);
            }
        }
    }
}

//�C���W�h
void game_rule()
{
    int y;
    system("cls");
    printf("\n\n\t\t\t�C���W�h\n\n");
    printf("\n\n\t\t\t111\n\n");

    system("pause");
}

int Main_Menu()
{
    int id;
    system("cls");
    //��X�ɭ�
    printf("\n\n\t\t\t�w��C���T�H��\n\n");

    printf("\t\t\t1:���U\n");
    printf("\t\t\t2:�n�J\n");
    printf("\t\t\t0:���}\n\n");

    //��J�\�ู�X
    printf("\t\t�п�ܸ��X�G");
    scanf("%d",&id);
    return id;
}

int Game_Menu()
{
    int id2;
    system("cls");
    printf("\n\n\t\t\t�C�����\n\n");
    printf("\n\n\t\t\t��! %s\n\n",on_name);
    printf("\t\t\t1:�C���ǰt\n");
    printf("\t\t\t2:�C���W�h\n");
    printf("\t\t\t0:�n�X\n\n");

    printf("\t\t�п�ܸ��X�G");
    scanf("%d",&id2);
    return id2;
}

int main()
{
    int id,id2,out;
    int y_or_n;

    while(1)///�n�J�e��
    {
        int test2=0;
        /*
        while(1)///�D���
        {
            id=Main_Menu();
            switch(id)
            {
                case 1:
                    regist();///���U
                    break;
                case 2:
                    if(judge()>0)
                        dl();///�n�J
                    break;
                case 0:
                    printf("\n\t\t\t�T�w�u���n���}�C����TAT\n\n");
                    printf("\t\t\t1:�O\n");
                    printf("\t\t\t2:�_\n");
                    printf("\n\t\t�п�ܸ��X�G");
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
                            printf("\n\t\t��J�����X���~�A�Ц^����A����J\n");
                            //system("pause");
                            printf("\t\t����۰ʸ���...");
                            sleep(2);
                            system("cls");
                    }
                    break;
                default:
                    printf("\n\t\t��J�����X���~�A�ЦA����J\n");
                    printf("\t\t����۰ʸ���...");
                    sleep(2);
                    //system("pause");
                    system("cls");
            }
            if(test==1)
                break;
        }
        */
        while(1)///�C�����
        {
            printf("\t\t�п�J�ʺ�[����W�L10�Ӧr]�G");
            scanf("%s",reg_name);
            //sleep(2);
            //id2=Game_Menu();
            id2=1;
            switch(id2)
            {
            case 1:
                system("cls");
                printf("\t\t\t\nConnect to server\n");
                Connect_server();///�s����server
                pthread_create(&thread1, NULL, &recvMess, NULL);///�}�Ҧh��
                Gameplay();
                break;
            case 2:
                game_rule();
                break;
            case 0:
                printf("\n\t\t\t�T�w�u���n�n�X��TAT\n\n");
                printf("\t\t\t1:�O\n");
                printf("\t\t\t2:�_\n");
                printf("\n\t\t�п�ܸ��X�G");
                scanf("%d",&out);

                if(out==1)
                {
                    printf("\t\t�n�X��...");
                    sleep(1);
                    test2++;
                }
                else if(out==2)
                    continue;
                else
                {
                    printf("\n\t\t��J�����X���~�A�Ц^����A����J\n");
                    printf("\t\t����۰ʸ���...");
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
