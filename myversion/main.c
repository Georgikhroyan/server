#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h>
#include <math.h> // floor 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#define _OPEN_SYS_ITOA_EXT 
#define TRUE   1 
#define FALSE  0
const int level_change[5][5] = {
{ 4, 4, 2, 1, 1 },
{ 3, 4, 3, 1, 1 },
{ 1, 3, 4, 3, 1 },
{ 1, 1, 3, 4, 3 },
{ 1, 1, 2, 4, 4 }
};

typedef struct Node {
    int value;
    struct Node *next;
} Node;

int pop(Node **head) {
    Node* prev = NULL;
    int val;
    if (head == NULL) {
        exit(-1);
    }
    prev = (*head);
    val = prev->value;
    (*head) = (*head)->next;
    free(prev);
    return val;
}

Node* getLast(Node *head) {
    if (head == NULL) {
        return NULL;
    }
    while (head->next) {
        head = head->next;
    }
    return head;
}

void push(Node **head, int data) {
    Node *tmp = (Node*) malloc(sizeof(Node));
    tmp->value = data;
    tmp->next = (*head);
    (*head) = tmp;
}
void pushBack(Node *head, int value) {
    Node *last = getLast(head);
    Node *tmp = (Node*) malloc(sizeof(Node));
    tmp->value = value;
    tmp->next = NULL;
    last->next = tmp;
}

void printLinkedList(const Node *head) {
    while (head) {
        printf("%d ", head->value);
        head = head->next;
    }
    printf("\n");
}

struct bnk
{
    int move_num;
    int lvl;
    int alive_players; 
}; 
struct PlayerInfo
{
    int alive;
    int player_num;
    int money;
    int production;
    int factories_work;
    int factories_build;
    int raw;
    int turn;
};
int main(int argc, char *argv[]){
    
  if (argc != 3 ) 
  {
    printf("Неверное количество аргументов \n");
    return 0;
  }
  int start_game = 0;
  int ls;
  int len;
  int opt = TRUE;
  int connfd;
  int count = 0;
  int max_sd;
  int new_socket;
  int activity;
  int valread;
  int sd;
  int turn;
  int test;
  int i ;
  int build_col = 0;
  char buffer[1025];
  test = 0 ;
  fd_set readfds;  
  char *p,*p1;
  int port = strtol(argv[1], &p, 10);
  int maxpl = strtol(argv[2],&p1,10);
  struct PlayerInfo PlayersInfo[maxpl];
  struct bnk bank;
  int turn_check = 0 ;
  int production_list[maxpl];
  int shop_list[maxpl][3];
  int sell_list[maxpl][3];
  int build[maxpl][2];
  Node* build_list = NULL;
  push(&build_list,-1);
  Node* start = build_list;
  for (int i = 0 ; i < maxpl ;  i++){
        shop_list[i][0] = i;
        shop_list[i][1] = 0; 
        shop_list[i][2] = 0;   
        sell_list[i][0] = i;
        sell_list[i][1] = 0;
        sell_list[i][2] = 0;
        build[i][0] = i;
        build[i][1] = 0;

    }
  
  for (int i = 0; i < maxpl; i++)
    {
        production_list[i] = 0;
        PlayersInfo[i].alive=TRUE;
        PlayersInfo[i].money=10000;
        PlayersInfo[i].production=2;
        PlayersInfo[i].factories_work=2;
        PlayersInfo[i].factories_build=0;
        PlayersInfo[i].raw=4;
        PlayersInfo[i].turn = 1;
    }
   bank.move_num = 2;
   bank.lvl = 3;
   bank.alive_players = 0;
int statistics[maxpl][7];
for (int i = 0; i<maxpl; i++)
        {
            statistics[i][0]= bank.move_num = 1;
            statistics[i][1]= bank.alive_players;
            statistics[i][2]=PlayersInfo[i].alive; 
            statistics[i][3]=PlayersInfo[i].money;
            statistics[i][4]=PlayersInfo[i].production;
            statistics[i][5]=PlayersInfo[i].factories_work;
            statistics[i][6]=PlayersInfo[i].factories_build;
            statistics[i][7]=PlayersInfo[i].raw;
        }
 float market[5][5]; 
 int store[2][2];
 for(int i = 0; i < 2 ; i++){
    for (int k = 0 ; k < 2 ; k++){
        store[i][k] = 0;     
    }
 }

for (int i=0; i<5; i++)
    {
        market[i][0]=i;
        market[i][1]= (1+0.5*i);
        market[i][3]= (3-0.5*i);
        market[i][4]= 6500-i*500;
    }
    market[0][2]=800;
    market[1][2]=650;
    market[2][2]=500;
    market[3][2]=400;
    market[4][2]=300;
//  for(int i = 0; i < 5 ; i++){
//     for (int k = 0 ; k < 5 ; k++){
//         printf("%lf\t",market[i][k]);
//     }
//     printf("\n");
//  }

  if(port < 1024){
    printf("---неверный порт---");
    return 0;
  }
  ls = socket(AF_INET,SOCK_STREAM,0);
  int player_list[maxpl];
  for (int i = 0 ; i<maxpl ; i++)
  {
    player_list[i] = 0;
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if( setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
  bind(ls, (struct sockaddr *) &addr, sizeof(addr));
  if ((listen(ls, 5)) != 0) {
        printf("Прослушивание провалено...\n");
        exit(0);
    }
    else
        printf("ожидание игроков..\n");
    len = sizeof(addr);
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(ls, &readfds);  
        max_sd = ls;      
        //add child sockets to set 
        for ( i = 0 ; i < maxpl ; i++)  
        {  
            //socket descriptor 
            sd = player_list[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
    if ((activity < 0) && (errno!=EINTR))  
    {  
            printf("Ошибка select");  
    }
    if (FD_ISSET(ls, &readfds))  
        {  
            if(start_game != 1){
            if ((new_socket = accept(ls,(struct sockaddr *)&addr, (socklen_t*)&len))<0)  
            {  
                perror("Ошибка accept");  
                exit(EXIT_FAILURE);  
            } 
            
            //inform user of socket number - used in send and receive commands
            count += 1;
            if (count == maxpl){
                start_game += 1;
            }    
            printf("Новое соединение , fd сокета %d , ip  : %s , port : %d  \n  start_game -->%d/%d<---" , new_socket , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port),count,maxpl);  
            
            
            // //send new connection greeting message 
            // if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            // {  
            //     perror("Ошибка send");  
            // }  
                 
            // puts("Приветственное сообщение отправлено успешно");  
                 
            //add new socket to array of sockets 
            for (i = 0; i < maxpl; i++)  
            {  
                //if position is empty 
                if( player_list[i] == 0 )       
                {  
                    player_list[i] = new_socket;  
                    PlayersInfo[i].alive=TRUE;
                    bank.alive_players += 1;
                    store[0][0] = (int)(market[bank.lvl - 1][1] * bank.alive_players);
                    store[1][0] = (int)(market[bank.lvl - 1][3] * bank.alive_players);
                    store[0][1] = market[bank.lvl - 1][2] ;
                    store[1][1] = market[bank.lvl - 1][4] ;
                    printf("Добавление в список сокетов ,как %d\n" , i);  
                         
                    break;  
                }  
            }
            }
        else{
            int scd = accept(ls,(struct sockaddr *)&addr, (socklen_t*)&len);
            send(scd,"\n\n\nИгра уже началась !\n\n\n",strlen("\n\n\nИгра уже началась !\n\n\n"),0);
            close(scd);
        }
        }
    if(count == maxpl){
        if(test == 0){
                            for(int i = 0 ; i < bank.alive_players; i++){             
                            send(player_list[i],"\n\n\n=======\nИГРА НАЧАЛАСЬ\nРаунд : 1\nДля помощи пропишите help\n=======\n\n\n",strlen("\n\n\n=======ИГРА НАЧАЛАСЬ\nРаунд : 1\nДля помощи пропишите help=======\n\n\n"),0);
                            test = 1;
                            }
                        }
    }
    //else its some IO operation on some other socket
    for (i = 0; i < maxpl; i++)  
        {  
            sd = player_list[i];               
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&addr , \
                        (socklen_t*)&len);  
                    printf("Игрок отключился , ip %s , port %d , num  %d \n " , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port),i);  
                    bank.alive_players -= 1;
                    store[0][0] = (int)(market[bank.lvl - 1][1] * bank.alive_players);
                    store[1][0] = (int)(market[bank.lvl - 1][3] * bank.alive_players);
                    store[0][1] = market[bank.lvl - 1][2] ;
                    store[1][1] = market[bank.lvl - 1][4] ;
                    PlayersInfo[i].alive=FALSE;
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    player_list[i] = 0;  
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0'; 
                    if(start_game == 1){ 
                        if(strncmp(buffer,"help",4) != 0 && strncmp(buffer,"market",6) != 0 && strncmp(buffer,"player",6) != 0 && strncmp(buffer,"prod",4) != 0 && strncmp(buffer,"buy",3) != 0 && strncmp(buffer,"sell",4) != 0 && strncmp(buffer,"build",5) != 0 &&  strncmp(buffer,"turn",4) != 0 && strncmp(buffer,"stats",5) != 0 ){
                            if (PlayersInfo[i].alive!=0){
                            send(sd , "Неверная команда ---- для получения информации введите help\n" , strlen("Неверная команда ---- для получения информации введите help\n") , 0 );  
                            }
                        }
                        else
                        {
                            if(strncmp(buffer,"help",4) == 0 ){ 
                            printf("\n\nЗапрос ""help"" от игрока %d\n\n",i); 
                            if (PlayersInfo[i].alive!=0){
                            send(sd , "\n\nhelp - помощь \nmarket - магазин \nplayer - информация об игроке - \nprod - продукция - \nbuy - купить -  \nsell - продать - \nbuild - построить - \nturn - ход -\n\n" , strlen("\n\nhelp - помощь \nmarket - магазин \nplayer - информация об игроке - \nprod - продукция - \nbuy - купить -  \nsell - продать - \nbuild - построить - \nturn - ход -\n\n") , 0 );  
                            }}
                            if(strncmp(buffer,"market",5) == 0 ){ 
                                printf("\n\nЗапрос ""market"" от игрока %d\n\n",i);
                                if (PlayersInfo[i].alive!=0){
                                if(PlayersInfo[i].turn != 0 ){
                                        char market_msg[150];
                                        snprintf(market_msg,150,"\n\n\nСырье\tколичество|\tmin. цена\n\t%d\t\t%d\nПродкуция\tколичество|\tmax. ценга\n\t%d\t\t%d\nMARKET LVL : %d\n\n\n",store[0][0],store[0][1],store[1][0],store[1][1],bank.lvl);
                                        send(sd,market_msg,strlen(market_msg),0);
                                               
                                    }}
                            }
                            if(strncmp(buffer,"build",5) == 0 ){                        
                                printf("\n\nЗапрос ""build"" от игрока %d\n\n",i);
                                if (PlayersInfo[i].alive!=0){
                                if(PlayersInfo[i].turn != 0 ){   
                                if(atoi(buffer+6) <= 0 ){
                                    send(sd,"\n\n\nНеверное количество ! \n\n\n",strlen("\n\n\nНеверное количество ! \n\n\n"),0);
                                }
                                else if(atoi(buffer+6) * 2500 > PlayersInfo[i].money){
                                    send(sd,"\n\nНедостаточно денег \n\n",strlen("\n\nНедостаточно денег \n\n"),0);
                                }
                                else{
                                    send(sd,"\n\n\nзаяква принята \n\n\n",strlen("\n\n\nзаяква принята \n\n\n"),0);
                                    build[i][1] = atoi(buffer+6);
                                }
                                }}
                            }
                            if(strncmp(buffer,"sell",4) == 0 ){
                                if (PlayersInfo[i].alive!=0){
                                        if(PlayersInfo[i].turn != 0 ){   
                                            if(atoi(buffer+4)<=0 || atoi(buffer+4)>store[1][0] || atoi(buffer+4) > PlayersInfo[i].production){
                                                send(sd,"\n\n\nУкажите верное количество !!!!\n\n\n",strlen("\n\n\nУкажите верное количество !!!!\n\n\n"),0);
                                            }
                                            else if(atoi(buffer+6) > store[1][1])
                                            {
                                                send(sd,"\n\n\nУкажите верную цену !!!!\n\n\n",strlen("\n\n\nУкажите верную цену !!!!\n\n\n"),0);
                                            }
                                            else{
                                                send(sd,"\n\n\nзаяква принята \n\n\n",strlen("\n\n\nзаяква принята \n\n\n"),0);
                                                sell_list[i][1] = atoi(buffer+4);
                                                sell_list[i][2] = atoi(buffer+6);
                                            } 
                                        }
                                }
                            }
                            if(strncmp(buffer,"buy",3) == 0 ){
                                if (PlayersInfo[i].alive!=0){
                                        if(PlayersInfo[i].turn != 0 ){   
                                            if(atoi(buffer+4)<=0 || atoi(buffer+4)>store[0][0]){
                                                send(sd,"\n\n\nУкажите верное количество !!!!\n\n\n",strlen("\n\n\nУкажите верное количество !!!!\n\n\n"),0);
                                            }
                                            else if(atoi(buffer+6) < store[0][1])
                                            {
                                                send(sd,"\n\n\nУкажите верную цену !!!!\n\n\n",strlen("\n\n\nУкажите верную цену !!!!\n\n\n"),0);
                                            }
                                            else{
                                                send(sd,"\n\n\nзаяква принята \n\n\n",strlen("\n\n\nзаяква принята \n\n\n"),0);
                                                shop_list[i][1] = atoi(buffer+4);
                                                shop_list[i][2] = atoi(buffer+6);
                                            } 
                                        }
                                }
                            }
                            if(strncmp(buffer,"prod",4) == 0 ){
                               
                                printf("\n\nЗапрос ""prod"" от игрока %d\n\n",i);
                                if (PlayersInfo[i].alive!=0){
                                        if(PlayersInfo[i].turn != 0 ){                     
                                            if(atoi(buffer+5) > PlayersInfo[i].factories_work)
                                            {
                                                send(sd,"\n\n\nНе хватает заводов ! \n\n\n",strlen("\n\n\n Не хватает заводов ! \n\n\n"),0);      
                                            }
                                            else if(atoi(buffer+5) * 2000 > PlayersInfo[i].money){
                                                send(sd,"\n\n\nНе хватает денег ! \n\n\n",strlen("\n\n\n Не хватает денег ! \n\n\n"),0);                                                
                                            }
                                            else if(atoi(buffer+5) > PlayersInfo[i].raw){
                                                 send(sd,"\n\n\nНе хватает сырья ! \n\n\n",strlen("\n\n\n Не хватает сырья ! \n\n\n"),0);           
                                            }
                                            else {
                                                send(sd,"\n\n\nзаяква принята \n\n\n",strlen("\n\n\nзаяква принята \n\n\n"),0);
                                                production_list[i] = atoi(buffer+5); 
                                                PlayersInfo[i].raw -= atoi(buffer+5);      
                                            }
                                            break;
                                    }
                                }              
                            }                 
                            if(strncmp(buffer,"turn",4) == 0 ){
                                printf("\n\nЗапрос ""turn"" от игрока %d\n\n",i);
                                if (PlayersInfo[i].alive!=0){
                                if(PlayersInfo[i].turn != 0 ){ 
                                       PlayersInfo[i].turn = 0;
                                       turn_check += 1;
                                send(sd,"--ход окончен--\n" ,strlen("--ход окончен--\n"),0);}}
                            }
                            if(strncmp(buffer,"player",6) == 0){
                                char player[100] ;
                                printf("\n\nЗапрос ""player"" от игрока %d\n\n",i);
                                
                                if (PlayersInfo[i].alive!=0){
                                if(PlayersInfo[i].turn != 0 ){
                                snprintf(player,100,"\n\n\nid = %d \nalive = %d \nmoney = %d\nproduction = %d\nfatctories_work = %d\nfactories_build = %d\nraw = %d\n\n\n",i,PlayersInfo[i].alive 
                                ,PlayersInfo[i].money,PlayersInfo[i].production,PlayersInfo[i].factories_work,PlayersInfo[i].factories_build,PlayersInfo[i].raw);
                                printf("%s",player);
                                send(sd,player,strlen(player),0);
                                send(sd,"\n",strlen("\n"),0);}}
                            }
                            if(strncmp(buffer, "stats" , 5) == 0){
                                char stats[100];
                                printf("Запрос ""stats"" от игрока %d\n",i);
                                for (int i = 0 ; i < maxpl ;i++){
                                    if(PlayersInfo[i].alive!=0){
                                        if(PlayersInfo[i].turn != 0){
                                        snprintf(stats,150,"\n\n======\n---player %d ---\nalive = %d\nmoney = %d \n production = %d \nfactories_work = %d \nfactories_build = %d \nraw = %d \n======\n\n",i,PlayersInfo[i].alive ,PlayersInfo[i].money,PlayersInfo[i].production,PlayersInfo[i].factories_work,PlayersInfo[i].factories_build,PlayersInfo[i].raw);
                                        send(sd,stats,strlen(stats),0);
                                        printf("%s",stats);}}

                                
                                snprintf(stats,40,"turn = %d\nalive_players = %d\n",bank.move_num,bank.alive_players);
                                // printf("\n\n======\n%s\n======\n\n",stats);
                                send(sd,stats,strlen(stats),0);}
                     
                            }
                            if(turn_check == bank.alive_players){
     
                                int r = rand()%12+1;
                                int lvl_num = 0;
                                int next_lvl = 0;
                                int max_cost = 0;
                                int min_cost = 10000;
                                int buyer_num ;
                                int seller_num;
                                for (int i = 0 ; i < maxpl ; i++){
                                    if (PlayersInfo[i].alive != 0 ){
                                        if(build[i][1] != 0){
                                            PlayersInfo[i].factories_build += build[i][1];
                                            PlayersInfo[i].money -= 2500 * build[i][1];
                                            pushBack(build_list,build[i][0]);
                                            pushBack(build_list,build[i][1]);
                                            pushBack(build_list, 5);
                                            build[i][1] = 0;
                                        }
                                    }
                                }

                                for (int i = 0 ; i < 5 ; i++){
                                    if(lvl_num < r)
                                    {
                                        lvl_num += level_change[bank.lvl-1][i];
                                        next_lvl = i+1;
                                    }
                                }
                                int buy_requests = 0;
                                for(int i = 0 ; i < maxpl; i++){
                                    if(shop_list[i][1] > 0){
                                        buy_requests+=1;
                                    }
                                }
                                int sell_request = 0;
                                for(int i = 0; i < maxpl; i++){
                                    if(sell_list[i][1] > 0){
                                        sell_request+=1;
                                    }
                                }                         
                                while(build_list->next != NULL){
                                    build_list = build_list->next->next->next; 
                                    build_list->value -= 1;
                                    if(build_list->value == 0){
                                        build_col+=1;
                                    }
                                }
                                build_list =start;
                                build_list = build_list->next;
                                for(int i = 0 ; i < build_col ; i++){
                                    PlayersInfo[build_list->value].factories_work += build_list->next->value;
                                    PlayersInfo[build_list->value].factories_build -= build_list->next->value;
                                    PlayersInfo[build_list->value].money -= 2500;
                                    for(int i = 0 ; i < maxpl; i++){
                                        if(PlayersInfo[i].alive != 0 ){
                                            if(player_list[i] !=0 ){
                                                char build_msg[100] ;
                                                snprintf(build_msg, 100,"\n\nИгрок id: %d построил %d фабрик \n\n ",build_list->value,build_list->next->value);
                                                send(player_list[i],build_msg,strlen(build_msg),0);
                                            }
                                        }
                                    }
                                    pop(&build_list);
                                    pop(&build_list);
                                    pop(&build_list);                                 
                                }
                                build_col = 0;
                                start->next = build_list; 
                                build_list = start;
                                // printf("list--\n");
                                // printLinkedList(build_list);
                                while(sell_request>0 && store[1][0] >0){
                                    char sell_msg[100];                    
                                    for(int i = 0 ; i < maxpl ; i++){
                                        if(PlayersInfo[i].alive != 0 ){
                                            if(sell_list[i][2] < min_cost && sell_list[i][2] > 0){
                                                min_cost = sell_list[i][2];
                                                seller_num = i ;
                                            }
                                        }
                                    }
                                    // printf("------sell req %d \t pl_id %d\n\tsell_cst %d",sell_request,seller_num,min_cost);
                                    while(sell_list[seller_num][1] > store[1][0]){
                                        sell_list[seller_num][1] -= 1;                       
                                    }
                                    if(sell_list[seller_num][1] == 0){
                                            sell_request -= 1;
                                            min_cost = 10000;
                                    }
                                    if(sell_list[seller_num][1] > 0){
                                    PlayersInfo[seller_num].production -= sell_list[seller_num][1];
                                    PlayersInfo[seller_num].money += min_cost * sell_list[seller_num][1];
                                    store[1][0] -= sell_list[seller_num][1];
                                    for(int i = 0 ; i < maxpl ; i++){
                                    if (PlayersInfo[i].alive != 0){
                                    snprintf(sell_msg,100,"\nИгрок id:%d продал %d продкуции за %d\n",seller_num,sell_list[seller_num][1],min_cost*sell_list[seller_num][1]);
                                    send(player_list[i],sell_msg,strlen(sell_msg),0);
                                    }
                                    }
                                    sell_list[seller_num][1] = 0;
                                    sell_list[seller_num][2] = 0;
                                    sell_request-=1;
                                    min_cost = 10000;}
                                }
                                // for(int i = 0;i < maxpl;i++){
                                //     printf("%d\t %d \t %d\t\n",sell_list[i][0],sell_list[i][1],sell_list[i][2]);
                                // }
                                while(buy_requests>0 && store[0][0] >0){
                                    char buy_msg[100];
                                    for(int i = 0 ; i < maxpl ; i++){
                                        if(PlayersInfo[i].alive != 0 ){
                                            if(shop_list[i][2] > max_cost){
                                                max_cost = shop_list[i][2];
                                                buyer_num = i ;
                                            }
                                        }
                                    }
                                    while(shop_list[buyer_num][1] > store[0][0]){
                                        shop_list[buyer_num][1] -= 1;                       
                                    }
                                    if(shop_list[buyer_num][1] == 0){
                                            buy_requests -= 1;
                                            max_cost = 0;
                                    }
                                    if(shop_list[buyer_num][1] > 0){
                                    PlayersInfo[buyer_num].money -= max_cost * shop_list[buyer_num][1];
                                    PlayersInfo[buyer_num].raw += shop_list[buyer_num][1];  
                                    store[0][0] -= shop_list[buyer_num][1];
                                    for(int i = 0 ; i < maxpl ; i++){
                                    if (PlayersInfo[i].alive != 0){
                                    snprintf(buy_msg,100,"\nИгрок id:%d купил %d сырья за %d\n",buyer_num,shop_list[buyer_num][1],max_cost*shop_list[buyer_num][1]);
                                    send(player_list[i],buy_msg,strlen(buy_msg),0);
                                    }
                                    }
                                    shop_list[buyer_num][1] = 0;
                                    shop_list[buyer_num][2] = 0;
                                    buy_requests-=1;
                                    max_cost = 0;}
                                }

                                bank.lvl = next_lvl;
                                store[0][0] = (int)(market[bank.lvl - 1][1] * bank.alive_players);
                                store[1][0] = (int)(market[bank.lvl - 1][3] * bank.alive_players);
                                store[0][1] = market[bank.lvl - 1][2] ;
                                store[1][1] = market[bank.lvl - 1][4] ;
                                bank.move_num += 1;
                                turn_check = 0;
                                for(int i = 0 ; i <  bank.alive_players;i++){
                                    PlayersInfo[i].turn = 1;
                                    char info[50];
                                    snprintf(info,50,"\n\n====\nРаунд : %d\n====\n\n",bank.move_num);
                                    if(player_list[i] != 0){
                                    send(player_list[i],info,strlen(info),0);}
                        }
                            for(int i = 0 ; i < maxpl; i++){
                                if(PlayersInfo[i].alive != 0 ){
                                    PlayersInfo[i].money -= PlayersInfo[i].raw * 300 + PlayersInfo[i].production * 500 + PlayersInfo[i].factories_work * 1000;
                                }
                            }
                            for(int i = 0 ; i < maxpl ; i++)
                            {
                                if(production_list[i] != 0){
                                    PlayersInfo[i].production += production_list[i];
                                    PlayersInfo[i].money -= 2000*production_list[i];
                                    char prod_msg[100];
                                    snprintf(prod_msg,100,"\n\n\nБыло создано %d продукции\n\n\n",production_list[i]);
                                    send(player_list[i],prod_msg,strlen(prod_msg),0);
                                }
                            }  
                            for(int i = 0 ; i < maxpl ; i++){
                                int lose;
                                if (PlayersInfo[i].money <= 0){
                                    PlayersInfo[i].alive = 0;
                                    lose = i;
                                    for (int i = 0; i < maxpl; i++){
                                        if(player_list[i] != 0){
                                            char lose_msg[100];
                                            snprintf(lose_msg,100,"\n\n\nИгрок id:%d ,банкрот !\n\n\n",lose);
                                            send(player_list[i],lose_msg,strlen(lose_msg),0);
                                        }
                                    }
                                    send(player_list[i],"\n\n\nВЫ ПРОИГРАЛИ!\n\n\n",strlen("\n\n\nВЫ ПРОИГРАЛИ!\n\n\n"),0);
                                    bank.alive_players-=1;
                                }
                            }

                            if(bank.alive_players == 1){
                                int pob;
                                for (int i = 0 ; i < maxpl; i++){
                                    if (PlayersInfo[i].alive != 0 ){
                                        pob = i;
                                        }
                                    }
                                    for(int i = 0 ; i < maxpl ; i++){
                                        if(player_list[i] != 0)
                                        {
                                            char win_msg[200];
                                            snprintf(win_msg,200,"\n\n\n──────── • • • ────────\nИгрок --->id:%d ПОБЕДИЛ !\n──────── • • • ────────\n\n\n ",pob);
                                            send(player_list[i],win_msg,strlen(win_msg),0);                                
                                        }                                  
                                    }
                                    printf("\n\n\n──────── • • • ────────\nИгрок --->id:%d ПОБЕДИЛ !\n──────── • • • ────────\n\n\n ",pob);
                                    exit(0);
                                }
                            }
                        }
                    }
                }  
            }  
        } 
    }  
    return 0;
}