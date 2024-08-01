#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<termios.h>
#include<unistd.h>
#include <time.h>
#define RANDOM rand()%100+1
#define MONSTER_RANDOM(X,Y) rand()%X+Y
#define MONSTER_HP(X,Y) ((double)rand()/RAND_MAX*(Y-X) + X) // ((double)rand() / RAND_MAX) * (100 - 50) + 50
#define ITEM_RANDOM(X,Y) rand()%X+Y
#define ARRAY_SIZE 52//+2 테두리
#define FLOOR_SIZE 6
#define monster_n 20

typedef enum{ sword, armer, shoes, gloves, cape, mask }equipment;

typedef struct{
    equipment equipment_type;
    int equipment_LV;
    int equipment_grade;
    double equipment_attack;
    double equipment_protect;
    int equipment_flag;
}user_equipment;

typedef struct{
    char name[20];
    float HP[2];
    int money;
    user_equipment *equipment;
    char equipment_name[6][4][30];
    double attack;
    double protect;
    int item[6];//item[0]=회복1,item[1]=회복2,item[2]=회복3,item[3]=회복4,item[4]=강화 주문서,item[5]=엘릭서
    int move_item[8][4];//
    int num;
}user_warrior;

typedef struct{//오크 좀비 구울 해골 스파토이 보스 네임드 
    char name[20];
    float HP;
    float attack;
}monster;

void LinEZ_map(char ****);
int move_map(char ****, int *, int *,int *,user_warrior *user, int *m_c,int *M_flag);
int getch();
int monster_HP(user_warrior *,monster *,int ,int,int,int*,int);
void monster_battle(monster *LIN_monster,int num);
int item_use(char ****,user_warrior *boki,int *x,int *y,int *z,int select_num,int *m_c);
int user_win(user_warrior *boki,int number);
void boki_attack(user_warrior *boki);
void boki_protect(user_warrior *boki);
int store(user_warrior* boki);
int enhancement(user_warrior* boki);
void Drop_item(user_warrior *boki, int tier);
void Boki_Droped(user_warrior *boki, int choice);
void intro();
void store_intro();
void smelter_intro();
void sanctum_intro();
void boss_kill_outtro();
int main(){

    char ***map;//맵 구현
    int x=1,y=1,z=0;//시작 위치
    int i=0,j=0,location=0;
    int m_c=0,select_num=0,number=0;
    int monster_flag=0, boki_flag=0, M_flag=0,flag=0,win_flag=0;
    int move_item=0,boki_count=0,boss_kill=0,named_kill_num=-1;
    int monster_random=0,boss_random_1=0,boss_random_2=0,boss_random_3=0;
    int store_count=0,smelter_count=0,sanctum_count=0; //입장횟수 카운트
    char buffer=0;

    // user_warrior boki={

    //     .name="boki",
    //     .HP={100,100},//[0]=현재체력,[1]=총체력
    //     .money=300,
    //     .attack=0,
    //     .protect=0,     //sword, armure, shoes, gloves, cape, mask
    //     .equipment={{sword,0,1,4,0,0},{armer,0,1,0,1,0},{shoes,0,1,0,1,0},{gloves,0,1,0,1,0},{cape,0,1,0,1,0},{mask,0,1,0,1,0}},//장비 종류, 강화 수치, 장비 티어, 장비 공격력, 장비 방어력, 장비 내구도
    //     .item={0},       //item[0]=회복1,item[1]=회복2,item[2]=회복3,item[3]=회복4,item[4]=강화 주문서,item[5]=엘릭서
    //     .move_item={{0}},
    //     .num=6
    // };
    user_warrior boki={

        .name="boki",
        .HP={10,100},//[0]=현재체력,[1]=총체력
        .money=300,
        .equipment_name={"1T기본검","2T장검","3T일본도","4T싸울아비장검",
                         "1T기본갑빠","2T반팔갑빠","3T후드갑빠","4T용갑빠",
                         "1T기본장화","2T슬리퍼","3T운동화","4T에어조단",
                         "1T기본장갑","2T고무장갑","3T면장갑","4T가죽장갑",
                         "1T기본망토","2T면망토","3T비단망토","4T방탄망토",
                         "1T기본마스크","2TK80마스크","3TK94마스크","4T타이거마스크"},
        .attack=0,
        .protect=0,     //sword, armure, shoes, gloves, cape, mask
        .item={10,10,10,10,10,10},       //item[0]=회복1,item[1]=회복2,item[2]=회복3,item[3]=회복4,item[4]=강화 주문서,item[5]=엘릭서
        .move_item={{0,0,0,10},{0},{0},{0},{0},{0},{0},{10}},
        .num=6
    };
    boki.equipment = (user_equipment*)malloc(boki.num * sizeof(user_equipment));

    for( int i =0 ; i<boki.num ; ++i){// 초기 장비 세팅
        boki.equipment[i].equipment_type=i;
        boki.equipment[i].equipment_LV=0;           
        boki.equipment[i].equipment_grade=1;
        if(boki.equipment[i].equipment_type==0)
            boki.equipment[i].equipment_attack=999999;
        else
            boki.equipment[i].equipment_protect=1;
        boki.equipment[i].equipment_flag=0;  
    }

    monster LIN_monster={
        .name="monster",
        .HP=0.0,
        .attack=0,
    };

    srand(time(NULL));
    LinEZ_map(&map);
    // intro();

    do{
        boki.equipment = (user_equipment*)realloc(boki.equipment,(boki.num+1) * sizeof(user_equipment));

        boki_attack(&boki);
        boki_protect(&boki);

        monster_flag=0;

        monster_random=RANDOM;
        boss_random_1=RANDOM;
        boss_random_2=RANDOM;
        boss_random_3=RANDOM;

        if(boki_flag==1){
            boki_flag=0;
            map[z][y][x]='.';
            boki.HP[0]= boki.HP[1]*0.1;
            x=46; y=6; z=0;
            printf("마을로 이동 1\n");
            sleep(5);
        }
        
        location=move_map(&map,&x,&y,&z,&boki,&m_c,&M_flag);
        if(boki.HP[0]>boki.HP[1]) boki.HP[0]=boki.HP[1];
        if(location>=100) continue;

        else if(location == 1){
            if(!store_count)store_intro();
            store_count=1;
            printf("잡화상에 입장합니다.\n");
            sleep(1);
            do{
                boki_count=store(&boki);
                boki.equipment = (user_equipment*)realloc(boki.equipment,(boki.num+2) * sizeof(user_equipment));
            }while(boki_count!=-1);
                
        }

        else if(location == 2){   
            if(!smelter_count)smelter_intro();
            smelter_count=1;
            printf("제련소에 입장합니다.\n");
            sleep(1);

            do{
                boki_count=enhancement(&boki);
                if(0<= boki_count && boki_count <boki.num){
                    Boki_Droped(&boki, boki_count);
                }
            }while(boki_count!=-1);
                
        }
        else if(location == 3){
            if(!sanctum_count)sanctum_intro();
            sanctum_count=1;
            printf("성소에 입장합니다.");
            sleep(1);
            boki.HP[0]=boki.HP[1];
        }
      
        if(1<= monster_random && monster_random <=70 && z>=1)
            monster_flag=1;
        if(71<= monster_random && monster_random <=100 && z>=1)
            monster_flag=2;
        if((1<= boss_random_1 && boss_random_1<=20) && (z==5) &&(boss_kill>=0))
            monster_flag=3;
        if((1<= boss_random_2 && boss_random_2<=10)&& (z==5) &&(boss_kill>=1))
            monster_flag=4;
        if((1<= boss_random_3 && boss_random_3<=5)&& (z==5) &&(boss_kill>=2))
            monster_flag=5;

        
        number=monster_HP(&boki,&LIN_monster,z,monster_flag,boss_kill,&named_kill_num,win_flag);
        win_flag=0;
        
        while((z>=1 && monster_flag && location ==4)||3<=monster_flag){

            monster_battle(&LIN_monster,number);

            boki_attack(&boki);
            boki_protect(&boki);

            printf("전투!! 아래 창에서 행동 선택\n");
            printf("===================================\n");
            printf("용사 %s HP %.2f 공격력 %.2f 방어력 %.2f \n",boki.name, boki.HP[0],boki.attack,boki.protect);
            printf("-----------------------------------\n");
            printf("%d층, %s 몬스터, HP %.2f 공격력 %.2f\n",z,LIN_monster.name, LIN_monster.HP,LIN_monster.attack);
            printf("===================================\n");
            printf("1. 전투, 2. 아이템, 3. 도망\t:");
            scanf("%d",&select_num);
            scanf("%c",&buffer);

            if(!(1<= select_num && select_num<=3)) 
                continue;

            if(select_num==1){
                printf("%s 몬스터, HP %.2f\n",LIN_monster.name, LIN_monster.HP);
                LIN_monster.HP-=boki.attack;

                if(LIN_monster.HP<=0){
                    win_flag=user_win(&boki,number);

                    if(number>=6 && boss_kill==0) boss_kill=1;
                    if(number>=7 && boss_kill==1) boss_kill=2;
                    if(number>=8 && boss_kill==2) boss_kill=3;
                    if(boss_kill==3) boss_kill_outtro();
                    
                    break;
                }
            }

            else if(select_num==2){
                printf("1. 이동 아이템, 2. 회복 아이템\t:");
                scanf("%d",&select_num);
                scanf("%c",&buffer);
                move_item=item_use(&map,&boki,&x,&y,&z,select_num,&m_c);
                if(move_item==10) break;

            }

            else if(select_num==3){
                if(1<= RANDOM && RANDOM <=100){
                    printf("도망 성공!!!!\n\n\n");
                    break;
                }
            }
            
            printf("\n\n\n\n%s 몬스터, 공격력 %.2f\n\n\n\n\n",LIN_monster.name,LIN_monster.attack);
            printf("boki HP %.2f, boki protect %.2f, monster attack%.2f\n",boki.HP[0],boki.protect,LIN_monster.attack);
            boki.HP[0]=boki.HP[0]+boki.protect-LIN_monster.attack;

            if(boki.HP[0]>boki.HP[1]) boki.HP[0]=boki.HP[1];
            if(boki.HP[0]<1){
                boki_flag=1;
                select_num=0;
                printf("전투 후 boki flag %d\n boki 체력 %.2f\n",boki_flag,boki.HP[0]);
                break;
            }
          
            printf("===================================\n");
            printf("용사 %s HP %.2f 공격력 %.2f 방어력 %.2f \n",boki.name, boki.HP[0],boki.attack,boki.protect);
            printf("-----------------------------------\n");
            printf("%d층 %s 몬스터, HP %.2f 공격력 %.2f\n",z,LIN_monster.name, LIN_monster.HP,LIN_monster.attack);
            printf("===================================\n");

            

        }//while 전투

    }while(1);//do while

}//메인

int getch(){
    int c;
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);           // 현재 터미널 설정 읽음
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);         // CANONICAL과 ECHO 끔
    newattr.c_cc[VMIN] = 1;                      // 최소 입력 문자 수를 1로 설정
    newattr.c_cc[VTIME] = 0;                     // 최소 읽기 대기 시간을 0으로 설정
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);  // 터미널에 설정 입력
    c = getchar();                               // 키보드 입력 읽음
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);  // 원래의 설정으로 복구

    return c;
}

int move_map(char ****array_map, int *x_coordinate, int *y_coordinate,int *z_coordinate,user_warrior *user,int *m_c,int *M_flag){

    int _x=0,_y=0,_z=0;
    char user_input=0,react=0,buffer=0;
    int i=0,j=0,k=0;
    int location=0,item_select=0;
    int y_x_mon_random[5][monster_n][2];//몬스터y,x좌표

    if((*M_flag)==0){
        for (i = 1; i < FLOOR_SIZE; i++)
            for (j = 1; j < ARRAY_SIZE-1; j++) 
                for (k = 1; k < ARRAY_SIZE-1; k++)
                    (*array_map)[i][j][k] = '.';

        for(i=0 ; i<5 ; i++){
            for (j = 0; j <monster_n ; j++){
                y_x_mon_random[i][j][0] = MONSTER_RANDOM(48,2);           
                y_x_mon_random[i][j][1] = MONSTER_RANDOM(48,2);
            }
        }

        for(i = 1, j=0; i <= 5, j<5; i++,j++) {    
            for (k = 0; k < monster_n; k++) {
            (*array_map)[i][y_x_mon_random[j][k][0]][y_x_mon_random[j][k][1]]='M';
            }
        }

        (*M_flag)=1;
    }

    for(i=1 ; i<5 ; i++){
        (*array_map)[i][50][50]='O';
        (*array_map)[i][1][1]='O';
    }
    (*array_map)[0][50][50]='O';
    (*array_map)[5][1][1]='O';

     

    for (j = 0; j < ARRAY_SIZE; j++) {
        for (k = 0; k < ARRAY_SIZE; k++) {
            printf("%3c",(*array_map)[*z_coordinate][j][k]);
        }
        printf("\n");
    }
 
    user_input=getch();
    if(user_input==91)
        user_input = getch();
    system("clear");
    
    _x=*x_coordinate;
    _y=*y_coordinate;
    _z=*z_coordinate;
    printf("층수는 %d층 입니다.\n",*z_coordinate);//맵 층수 보여줌
    printf("x= %d, y = %d, 층 = %d\n",*x_coordinate,*y_coordinate,*z_coordinate);
        
    switch(user_input){
            
        case 'a':
        case 68:
            --*x_coordinate;
            if(*x_coordinate<=1)*x_coordinate=1;
            break;
        case 'd':
        case 67:
            ++*x_coordinate;
            if(*x_coordinate>=ARRAY_SIZE-2)*x_coordinate=ARRAY_SIZE-2;
            break;
        case 'w':
        case 65:
            --*y_coordinate;
            if(*y_coordinate<=1)*y_coordinate=1;
            break;
        case 's':
        case 66:
            ++*y_coordinate;
            if(*y_coordinate>=ARRAY_SIZE-2)*y_coordinate=ARRAY_SIZE-2;
            break;
        case 'm':                                                       //순간이동 좌표저장
            printf("현재 위치를 저장하시겠습니까? (y/n)\n");
            react=getch();
            if(react == 'y' || react =='Y')
                if(*m_c == 7)
                    printf("저장할 수 있는 공간이 다 찼습니다.");
                else
                {
                    user->move_item[*m_c][0]=* z_coordinate;
                    user->move_item[*m_c][1]=* y_coordinate;
                    user->move_item[*m_c][2]=* x_coordinate;
                    printf("위칫값이 저장되었습니다. %d. %d층(%d,%d)\n",
                            *m_c+1,user->move_item[*m_c][0],user->move_item[*m_c][1],user->move_item[*m_c][2]);
                    (*m_c)++;
                }
            break;
        case 'n':  //순간이동 선택
            if(user->move_item[0][3]<=0) break;

            printf("m_c  %d\n",*m_c);
            if(*m_c>0){   
                printf("어느곳으로 이동하시겠습니까?\n");
                for(i=0; i<*m_c ; i++){    
                    printf("%d. %d층(%d,%d)\n",i+1,user->move_item[i][0],
                            user->move_item[i][1],user->move_item[i][2]);
                }
                    printf("이동할 번호 : ");
                    scanf("%d",&j);
                    getchar();
                    if(j<=*m_c){
                        (*array_map)[*z_coordinate][*y_coordinate][*x_coordinate]='.';
                        * z_coordinate=user->move_item[j-1][0];
                        * y_coordinate=user->move_item[j-1][1];
                        * x_coordinate=user->move_item[j-1][2];
                        user->move_item[0][3]-=1;
                    }
                    else
                        printf("잘못된 번호입니다.\n");
            }
            else
                printf("저장된 위치가 없습니다.(m으로 위치저장)\n");
            break;
        case 'b':
            if(user->move_item[7][0]<=0) break;
            printf("판도사 상점으로 이동\n");
            *x_coordinate=46;
            *y_coordinate=6;
            *z_coordinate=0;
            printf("마을로 이동 2\n");
            sleep(5);
            user->move_item[7][0]-=1;
            break;

        case 'i':
            printf("용사 %s 총 HP %.2f, 현재 HP %.2f, 공격력 %.2f 방어력 %.2f \n",user->name, user->HP[1],user->HP[0],user->attack,user->protect);
            printf("용사 %s 골드 %d 1물약 %d, 2물약 %d, 3물약 %d, 4물약 %d \n",user->name, user->money,user->item[0],user->item[1],user->item[2],user->item[3]);//현금 아이템 아이템
            printf("용사 %s 강화 주문서 %d, 마을 이동 주문서 %d, 순간 이동 주문서 %d, 엘릭서 %d  \n",user->name,user->item[4],user->move_item[7][0],user->move_item[0][3],user->item[5]);
            for(i =0 ; i<user->num ; i++){
                if(user->equipment[i].equipment_flag==0){
                    printf("%d)",i);
                    printf("장비 종류 : %s  \n",user->equipment_name[user->equipment[i].equipment_type][user->equipment[i].equipment_grade-1]);
                    printf("장비 강화레벨 : %d  \n",user->equipment[i].equipment_LV);           
                    printf("장비 티어 : %d  \n",user->equipment[i].equipment_grade);
                    printf("장비 공격력 : %.2f  \n",user->equipment[i].equipment_attack);
                    printf("장비 방어력 : %.2f  \n\n",user->equipment[i].equipment_protect);
                }
            }
            location=100;
            break;
        case 'p':
            printf("1. 빨간물약, 2. 주황물약, 3. 맑은물약, 4. 고농도물약 :\t");
            scanf("%d",&item_select);
            scanf("%c",&buffer);
            if(user->item[item_select-1]<=0)
                return 0;
            switch(item_select){
                case 1:
                    user->item[0]-=1;
                    printf("빨간물약 사용\n");
                    user->HP[0]+=30;
                    break;
                case 2:
                    user->item[1]-=1;
                    printf("주황물약 사용\n");
                    user->HP[0]+=50;
                    break;
                case 3:
                    user->item[2]-=1;
                    printf("맑은물약 사용\n");
                    user->HP[0]+=70;
                    break;
                case 4:
                    user->item[3]-=1;
                    printf("고농도물약 사용\n");
                    user->HP[0]+=150;
                    break;
                default:
                    printf("--\n");
                    break;
            }
            break;
        default:
            printf("--\n");
            break;
    }

    if((*array_map)[*z_coordinate][*y_coordinate][*x_coordinate]=='M'){
        printf("몬스터 만남\n");
        (*array_map)[_z][_y][_x]='.';
        (*array_map)[*z_coordinate][*y_coordinate][*x_coordinate]='X';
        location =4;
        return location;
    }
        
    (*array_map)[_z][_y][_x]='.';

    if(* x_coordinate==50 && * y_coordinate==50 && *z_coordinate < 5){      //층수이동 조건(올라갈때)
            ++(*z_coordinate);
            * x_coordinate=1,* y_coordinate=2;
            * M_flag=0;
    }
    else if(* x_coordinate==1 && * y_coordinate==1 && *z_coordinate > 0){  //층수이동 조건(내려갈때)
            --(*z_coordinate);
            * x_coordinate=49,* y_coordinate=49;
            * M_flag=0;
    }

    if(*z_coordinate==0 && (* x_coordinate>40 && * x_coordinate<48) && (* y_coordinate>0 && * y_coordinate<5)) //잡화상 입장조건
        {   
            printf("잡화점에 입장하시겠습니까? y/n\n");
            react=getch();
            if(react == 'y' || react =='Y'){    
                location=1;
                * x_coordinate = _x;
                * y_coordinate = _y;
            }
            else{
                * x_coordinate = _x;
                * y_coordinate = _y;
            }
        }
        else if(*z_coordinate==0 && (* x_coordinate>21 && * x_coordinate<30) && (* y_coordinate>46 && * y_coordinate<51)) //제련소 입장조건
        {   
            printf("제련소에 입장하시겠습니까? y/n\n");
            react=getch();
            if(react == 'y' || react =='Y'){    
                location=2;
                * x_coordinate = _x;
                * y_coordinate = _y;
            }
            else{
                * x_coordinate = _x;
                * y_coordinate = _y;
            }
        }
        else if(*z_coordinate==0 && (* x_coordinate>45 && * x_coordinate<51) && (* y_coordinate>40 && * y_coordinate<45))//성소 입장조건
        {   
            printf("성소에 입장하시겠습니까? y/n\n");
            react=getch();
            if(react == 'y' || react =='Y'){    
                location=3;
                * x_coordinate = _x;
                * y_coordinate = _y;
            }
            else{
                * x_coordinate = _x;
                * y_coordinate = _y;
            }
        }
    
    (*array_map)[*z_coordinate][*y_coordinate][*x_coordinate]='X';//유저 위치 표시

    return location;
}

void LinEZ_map(char ****array_map) {

    int i=0,j=0,k=0;

    *array_map = (char ***)malloc(FLOOR_SIZE * sizeof(char **));

    for (i = 0; i < FLOOR_SIZE; i++){
        (*array_map)[i] = (char **)malloc(ARRAY_SIZE * sizeof(char *));
        for (j = 0; j < ARRAY_SIZE; j++) {
            (*array_map)[i][j] = (char *)malloc(ARRAY_SIZE * sizeof(char));
            for (k = 0; k < ARRAY_SIZE; k++) {
                (*array_map)[i][j][k] = '.';
                if ((k== 0 || k == ARRAY_SIZE-1) || (j == 0 || j == ARRAY_SIZE-1)) // 외곽 테두리
                    (*array_map)[i][j][k] = 'A';
                else if (i == 0 && (k > 40 && k < 48) && (j >0 && j<5)) // 잡화점
                   (*array_map)[i][j][k] = '+';
                else if (i == 0 && (k > 21 && k < 30) && (j > 46 && j < 51)) // 제련소
                    (*array_map)[i][j][k] = '+';
                else if (i == 0 && (k > 45 && k < 51) && (j > 40 && j < 45)) // 성소
                    (*array_map)[i][j][k] = '+';
            }
        }
    }
    
}

int monster_HP(user_warrior *boki,monster *LIN_monster,int z, int flag,int b_kill,int *named_kill_num,int win_flag){
    char name [30][15] = {"유시온","김승수","권철민","안광민","강진영","김영곤","박선후","김건","이준호","이철","이동준","황은비",
                          "조세빈","김성근","이은승","박희정","박장미","김민아","조대정","김재신","박민건","임석현","황운하","노주영",
                          "김혜빈","서훈","오은지","바포메트","이동녘","류홍걸"};
    int boss_radname=0;
    int monster_number=0;
    int rannum=0;
    boss_radname=MONSTER_RANDOM(27+b_kill,0);

    if (*named_kill_num == boss_radname && win_flag == 1) {
        switch (flag) {
            case 2:
                rannum = 70;
                break;
            case 3:
                rannum = 80;
                break;
            case 4:
                rannum = 90;
                break;
            case 5:
                rannum = 95;
                break;
        }
        if (RANDOM <= rannum) {
            do {
                boss_radname = MONSTER_RANDOM(27 + b_kill, 0);
            } while (*named_kill_num == boss_radname);
        }
    }

    if(boss_radname==27)flag=3;
    if(boss_radname==28)flag=4;
    if(boss_radname==29)flag=5;
 
    switch(flag){
        case 1:
            if(z==1){
                strcpy(LIN_monster->name,"ORK");
                LIN_monster->HP=MONSTER_RANDOM(51,50);
                monster_number=0;
            }
            if(z==2){
                strcpy(LIN_monster->name,"ZOMBI");
                LIN_monster->HP=MONSTER_RANDOM(51,130);
                monster_number=1;
            }
            if(z==3){
                strcpy(LIN_monster->name,"GHOUL");
                LIN_monster->HP=MONSTER_RANDOM(121,160);
                monster_number=2;
            }
            if(z==4){
                strcpy(LIN_monster->name,"SKELETON");
                LIN_monster->HP=MONSTER_RANDOM(201,60);
                monster_number=3;
            }
            if(z==5){
                strcpy(LIN_monster->name,"SPATOY");
                LIN_monster->HP=MONSTER_RANDOM(261,100);
                monster_number=4;
            }
            break;            
        case 2:
            strcpy(LIN_monster->name,name[boss_radname]);
            LIN_monster->HP=boki->HP[1]*2.0;
            monster_number=5;
            break;
        case 3:
            strcpy(LIN_monster->name,"바포메트");
            LIN_monster->HP=boki->HP[1]*5.0;
            monster_number=6;
            break;
        case 4:
            strcpy(LIN_monster->name,"이동녘");
            LIN_monster->HP=boki->HP[1]*7.0;
            monster_number=7;
            break;
        case 5:
            strcpy(LIN_monster->name,"류홍걸");
            LIN_monster->HP=boki->HP[1]*10.0;
            monster_number=8;
            break;
    }
    *named_kill_num=boss_radname;
    return monster_number;

}

void monster_battle(monster *LIN_monster,int num){

    switch(num){
        case 0:
            LIN_monster->attack=MONSTER_RANDOM(6,10);
            break;
        case 1:
            LIN_monster->attack=MONSTER_RANDOM(14,17);
            break;
        case 2:
            LIN_monster->attack=MONSTER_RANDOM(26,20);
            break;
        case 3:
            LIN_monster->attack=MONSTER_RANDOM(28,28);
            break;
        case 4:
            LIN_monster->attack=MONSTER_RANDOM(44,32);
            break;
        case 5:
            LIN_monster->attack=MONSTER_RANDOM(101,200);
            break;
        case 6:
            LIN_monster->attack=MONSTER_RANDOM(181,270);
            break;
        case 7:
            LIN_monster->attack=MONSTER_RANDOM(251,300);
            break;
        case 8:
            LIN_monster->attack=MONSTER_RANDOM(801,500);
            break;
        default:
            printf("--\n");
            break;
        
    }
}

int item_use(char ****array_map,user_warrior *boki,int *x,int *y,int *z,int select_num,int *m_c){
    int item_select=0;
    char buffer=0;
    int i=0,j=0;

    if(select_num==1){
        printf("1. 마을 이동 주문서, 2, 순간이동 주문서 :\t");
        scanf("%d",&item_select);
        scanf("%c",&buffer);

        if(boki->move_item[item_select+6][0]<=0)
            return 0;
        else if(boki->move_item[item_select-2][3]<=0)
            return 0;
        
        switch(item_select){
            case 1:
                if(boki->move_item[7][0]<=0) break;
                boki->move_item[7][0]-=1;
                printf("마을로 이동 합니다.\n");
                (*array_map)[*z][*y][*x]='.';
                item_select=10;
                *z=0; *x=46; *y=6;
                printf("마을로 이동 3\n");
                sleep(5);
                break;
            case 2:
                // if(boki->move_item[0][3]<=0); break;
                printf("선택한 좌표로 이동합니다.\n");
                for(i=0; i<*m_c ; i++){    
                    printf("%d. %d층(%d,%d)\n",i+1,boki->move_item[i][0],boki->move_item[i][1],boki->move_item[i][2]);
                }
                    printf("이동할 번호 : ");
                    scanf("%d",&j);
                    getchar();
                    if(j<=*m_c){
                        (*array_map)[*z][*y][*x]='.';
                        * z=boki->move_item[j-1][0];
                        * y=boki->move_item[j-1][1];
                        * x=boki->move_item[j-1][2];
                        boki->move_item[0][3]-=1;
                    }
                    else
                        printf("잘못된 번호입니다.\n");
                item_select=10;
                break;
            default:
                printf("--\n");
                break;
        }
    }

    else if(select_num==2){
        printf("1. 빨간물약, 2. 주황물약, 3. 맑은물약, 4. 고농도물약 :\t");
        scanf("%d",&item_select);
        scanf("%c",&buffer);
        if(boki->item[item_select-1]<=0)
            return 0;
        switch(item_select){
            case 1:
                boki->item[0]-=1;
                printf("빨간물약 사용\n");
                boki->HP[0]+=30;
                break;
            case 2:
                boki->item[1]-=1;
                printf("주황물약 사용\n");
                boki->HP[0]+=50;
                break;
            case 3:
                boki->item[2]-=1;
                printf("맑은물약 사용\n");
                boki->HP[0]+=70;
                break;
            case 4:
                boki->item[3]-=1;
                printf("고농도물약 사용\n");
                boki->HP[0]+=150;
                break;
            default:
                printf("--\n");
                break;
        
        }
    }
    if(boki->HP[0]>boki->HP[1])
        boki->HP[0]=boki->HP[1];

    return item_select;

}

int user_win(user_warrior *boki,int number){
    
    switch(number){
        case 0:
            if(80<=RANDOM && RANDOM<=100) boki->move_item[7][0]+=1;
            boki->money+=ITEM_RANDOM(26,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.01;
            break;
        case 1:
            if(80<=RANDOM && RANDOM<=100) boki->move_item[7][0]+=1;
            boki->money+=ITEM_RANDOM(56,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.02;
            break;
        case 2:
            if(80<=RANDOM && RANDOM<=100) boki->move_item[7][0]+=1;
            boki->money+=ITEM_RANDOM(96,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.03;
            break;
        case 3:
            if(80<=RANDOM && RANDOM<=100) boki->move_item[7][0]+=1;
            boki->money+=ITEM_RANDOM(126,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.05;
            break;
        case 4:
            if(80<=RANDOM && RANDOM<=100) boki->move_item[7][0]+=1;
            boki->money+=ITEM_RANDOM(196,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.07;
            Drop_item(boki,2);
            boki->num++;
            break;
        case 5:
            if(80<=RANDOM && RANDOM<=100) boki->move_item[7][0]+=1;
            if(RANDOM<=30) boki->move_item[0][3]+=ITEM_RANDOM(4,0);
            boki->money+=ITEM_RANDOM(496,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.2;
            Drop_item(boki,2);
            Drop_item(boki,3);
            break;
        case 6:
            if(RANDOM<=30) boki->move_item[0][3]+=ITEM_RANDOM(4,0);
            boki->money+=ITEM_RANDOM(696,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.3;
            Drop_item(boki,3);
            Drop_item(boki,4);
            break;
        case 7:
            if(RANDOM<=30) boki->move_item[0][3]+=ITEM_RANDOM(4,0);
            if(RANDOM<=10) boki->item[5]+=ITEM_RANDOM(3,1);
            boki->money+=ITEM_RANDOM(996,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*0.6;
            Drop_item(boki,3);
            Drop_item(boki,4);
            break;
        case 8:
            if(RANDOM<=30) boki->move_item[0][3]+=ITEM_RANDOM(4,0);
            if(RANDOM<=10) boki->item[5]+=ITEM_RANDOM(3,1);
            boki->money+=ITEM_RANDOM(2996,5);
            boki->HP[1]=boki->HP[1]+boki->HP[1]*1.0;
            Drop_item(boki,3);
            Drop_item(boki,4);
            break;
        default:
            printf("--\n");
            break;
        
    }
    return 1;
}

void boki_attack(user_warrior *boki){
    int i=0;
    boki->attack=0;
    for(i=0; i<boki->num;i++)
        if(boki->equipment[i].equipment_flag==0)
            boki->attack+=boki->equipment[i].equipment_attack;
}

void boki_protect(user_warrior *boki){
    int i=0;
    boki->protect=0;
    for(i=0; i<boki->num;i++){
        if(boki->equipment[i].equipment_flag==0)
            boki->protect+=boki->equipment[i].equipment_protect;
    }

}

int store(user_warrior* boki){

    int n;
    int num=0;
    char input,buffer;

    printf("무기 구매? (y/n) \n");
    input=getch();

    if(!(input=='y' ||input=='Y'))
        return -1;

    printf("1. sword, 2. armer, 3.shoes, 4.gloves, 5.cape, 6.mask 7. 빨간물약\n");
    scanf("%d",&n);
    scanf("%c",&buffer);

    switch(n){  
        case 1:
            if(boki->money<50) break;
            boki->equipment[boki->num].equipment_type=sword;
            boki->equipment[boki->num].equipment_LV=0;
            boki->equipment[boki->num].equipment_grade=1;
            boki->equipment[boki->num].equipment_attack=2;
            boki->equipment[boki->num].equipment_protect=0;
            boki->equipment[boki->num].equipment_flag=0;
            printf("boki 장비 %d\n",boki->equipment[boki->num].equipment_type);
            boki->money-=50;
            boki->num++;
            num+=1;
            break;
        case 2:
            if(boki->money<30) break;
            boki->equipment[boki->num].equipment_type=armer;
            boki->equipment[boki->num].equipment_LV=0;
            boki->equipment[boki->num].equipment_grade=1;
            boki->equipment[boki->num].equipment_attack=0;
            boki->equipment[boki->num].equipment_protect=1;
            boki->equipment[boki->num].equipment_flag=0;
            printf("boki 장비 %d\n",boki->equipment[boki->num].equipment_type);
            boki->money-=30;
            boki->num++;
            num+=1;
            break;
        case 3:
            if(boki->money<30) break;
            boki->equipment[boki->num].equipment_type=shoes;
            boki->equipment[boki->num].equipment_LV=0;
            boki->equipment[boki->num].equipment_grade=1;
            boki->equipment[boki->num].equipment_attack=0;
            boki->equipment[boki->num].equipment_protect=1;
            boki->equipment[boki->num].equipment_flag=0;
            printf("boki 장비 %d\n",boki->equipment[boki->num].equipment_type);
            boki->money-=30;
            boki->num++;
            num+=1;
            break;
        case 4:
            if(boki->money<30) break;
            boki->equipment[boki->num].equipment_type=gloves;
            boki->equipment[boki->num].equipment_LV=0;
            boki->equipment[boki->num].equipment_grade=1;
            boki->equipment[boki->num].equipment_attack=0;
            boki->equipment[boki->num].equipment_protect=1;
            boki->equipment[boki->num].equipment_flag=0;
            printf("boki 장비 %d\n",boki->equipment[boki->num].equipment_type);
            boki->money-=30;
            boki->num++;
            num+=1;
            break;
        case 5:
            if(boki->money<30) break;
            boki->equipment[boki->num].equipment_type=cape;
            boki->equipment[boki->num].equipment_LV=0;
            boki->equipment[boki->num].equipment_grade=1;
            boki->equipment[boki->num].equipment_attack=0;
            boki->equipment[boki->num].equipment_protect=1;
            boki->equipment[boki->num].equipment_flag=0;
            printf("boki 장비 %d\n",boki->equipment[boki->num].equipment_type);
            boki->money-=30;
            boki->num++;
            num+=1;
            break;
        case 6:
            if(boki->money<30) break;
            boki->equipment[boki->num].equipment_type=mask;
            boki->equipment[boki->num].equipment_LV=0;
            boki->equipment[boki->num].equipment_grade=1;
            boki->equipment[boki->num].equipment_attack=0;
            boki->equipment[boki->num].equipment_protect=1;
            boki->equipment[boki->num].equipment_flag=0;
            printf("boki 장비 %d\n",boki->equipment[boki->num].equipment_type);
            boki->money-=30;
            boki->num++;
            num+=1;
            break;
        case 7:
            if(boki->money<30) break;
            boki->item[0]++;
            break;
        default:
            printf("--\n");
            break;
        
    }
    
    for(int i=0; i<boki->num;i++){
        printf("장비 종류 : %s  ",boki->equipment_name[boki->equipment[i].equipment_type][boki->equipment[i].equipment_grade-1]);
        printf("boki 장비 강화레벨 %d\n",boki->equipment[i].equipment_LV);
        printf("boki 장비 티어 %d\n",boki->equipment[i].equipment_grade);
        printf("boki 장비 공격력 %.2f\n",boki->equipment[i].equipment_attack);
        printf("boki 장비 방어력 %.2f\n",boki->equipment[i].equipment_protect);
        printf("boki 빨간물약 %d\n",boki->item[0]);
        printf("=====================================================================================\n");
    }
    return num;
}

int enhancement(user_warrior* boki){
    
    float enhancement[10][2]={{1,1.1},{2,1.2},{3,1.3},{4,1.4},{5,1.5},{6,1.6},{7,1.7},{8,1.8},{9,1.9},{10,2.0}};
    int i=0,choice=0,num=0;
    int equipment_random=0;
    char input=0,buffer=0;

    equipment_random=RANDOM;
    if(boki->num<1){
        printf("강화 무기 없음\n");
        sleep(1);
        return -1;
    }

    printf("무기를 강화 것인가? (y/n)\n");
    input=getch();
    if(!(input=='y' || input=='Y'))
        return -1;
    printf("강화 방법 선택 - 1. 강화 주문서 2. 엘릭서 :\t");
    scanf("%d",&num);
    scanf("%c",&buffer);
    if(!(1<=num && num<=2)){
        printf("제련소 쫓겨남\n");
        return -1;
    }

    if(boki->item[num+3]<=0) return 0;

    switch(num){

        case 1:
            printf("어떤 무기를 강화할 것인가? \n");
            for( i =0 ; i<boki->num ; ++i){
                if(boki->equipment[i].equipment_flag==0){
                    printf("%d)",i);
                    printf("장비 종류 : %s  \n",boki->equipment_name[boki->equipment[i].equipment_type][boki->equipment[i].equipment_grade-1]);
                    printf("장비 강화레벨 : %d  \n",boki->equipment[i].equipment_LV);           
                    printf("장비 티어 : %d  \n",boki->equipment[i].equipment_grade);
                    printf("장비 공격력 : %.2f  \n",boki->equipment[i].equipment_attack);
                    printf("장비 방어력 : %.2f  \n\n",boki->equipment[i].equipment_protect);
                }
            }
            scanf("%d",&choice);
            scanf("%c",&buffer);

            if(choice<boki->num && equipment_random<=20 && boki->equipment[choice].equipment_flag==0){
                printf("강화 성공\n");
                if(boki->equipment[choice].equipment_LV<10){
                    boki->equipment[choice].equipment_LV +=1;
                    if(boki->equipment[choice].equipment_attack>0)
                        boki->equipment[choice].equipment_attack*=enhancement[boki->equipment[choice].equipment_LV-1][1];
                    else
                        boki->equipment[choice].equipment_protect*=enhancement[boki->equipment[choice].equipment_LV-1][1];
                }
                boki->item[4]-=1;
            }
            else if(20<equipment_random){
                printf("장비 파괴\n");
                boki->equipment[choice].equipment_flag=1;
                boki->item[4]-=1;
                return choice;
            }
            else
                printf("뭐? 그런 무기는 없잖아 다시골라봐.\n");

            break;
        case 2:
            printf("어떤 무기를 강화할 것인가? \n");
            for( i =0 ; i<boki->num ; ++i){
                if(boki->equipment[i].equipment_flag==0){
                    printf("%d)",i);
                    printf("장비 종류 : %s  \n",boki->equipment_name[boki->equipment[i].equipment_type][boki->equipment[i].equipment_grade-1]);
                    printf("장비 강화레벨 : %d  \n",boki->equipment[i].equipment_LV);           
                    printf("장비 티어 : %d  \n",boki->equipment[i].equipment_grade);
                    printf("장비 공격력 : %.2f  \n",boki->equipment[i].equipment_attack);
                    printf("장비 방어력 : %.2f  \n\n",boki->equipment[i].equipment_protect);
                }
            }
            scanf("%d",&choice);
            scanf("%c",&buffer);

            if(choice<boki->num && boki->equipment[choice].equipment_flag==0){
                if(boki->equipment[choice].equipment_LV<10){
                    boki->equipment[choice].equipment_LV +=1;
                    if(boki->equipment[choice].equipment_attack>0)
                        boki->equipment[choice].equipment_attack*=enhancement[boki->equipment[choice].equipment_LV-1][1];
                    else
                        boki->equipment[choice].equipment_protect*=enhancement[boki->equipment[choice].equipment_LV-1][1];
                }
                boki->item[5]-=1;
            }
            else
                printf("뭐? 그런 무기는 없잖아 다시골라봐.\n");

            break;
        default:
            printf("--\n");
            break;

    }

    for( int i =0 ; i<boki->num ; ++i){
        if(boki->equipment[i].equipment_flag==0){
            printf("%d)",i);
            printf("장비 종류 : %s  \n",boki->equipment_name[boki->equipment[i].equipment_type][boki->equipment[i].equipment_grade-1]);
            printf("장비 강화레벨 : %d  \n",boki->equipment[i].equipment_LV);           
            printf("장비 티어 : %d  \n",boki->equipment[i].equipment_grade);
            printf("장비 공격력 : %.2f  \n",boki->equipment[i].equipment_attack);
            printf("장비 방어력 : %.2f  \n",boki->equipment[i].equipment_protect);
        }
    }

    return -2;
}

void Drop_item(user_warrior *boki, int tier){
    int random;
    random=ITEM_RANDOM(6,0);
    switch(tier){
        case 2:
            if(81<=RANDOM && RANDOM<=100){//2티어
                boki->equipment[boki->num].equipment_type=random;
                boki->equipment[boki->num].equipment_grade=2;
                if(random==0)
                    boki->equipment[boki->num].equipment_attack=5;
                else   
                    boki->equipment[boki->num].equipment_protect=4;
                boki->equipment[boki->num].equipment_flag=0;
                boki->num++;
            }
            break;
        case 3:
            if(81<=RANDOM && RANDOM<=100){//3티어
                boki->equipment[boki->num].equipment_type=random;
                boki->equipment[boki->num].equipment_grade=3;
                if(random==0)
                    boki->equipment[boki->num].equipment_attack=10;
                else   
                    boki->equipment[boki->num].equipment_protect=8;
                boki->equipment[boki->num].equipment_flag=0;
                boki->num++;
            }
            break;
        case 4:
            if(1<=RANDOM && RANDOM<=10){//3티어
                boki->equipment[boki->num].equipment_type=random;
                boki->equipment[boki->num].equipment_grade=4;
                if(random==0)
                    boki->equipment[boki->num].equipment_attack=20;
                else   
                    boki->equipment[boki->num].equipment_protect=13;
                boki->equipment[boki->num].equipment_flag=0;
                boki->num++;
            }
            break;
        defalut:
            printf("--\n");
            break;
    }
 
}

void Boki_Droped(user_warrior *boki, int choice){
    int i=0;

    for(i=choice; i<boki->num-1; i++){
        boki->equipment[i].equipment_type=boki->equipment[i+1].equipment_type;
        boki->equipment[i].equipment_LV=boki->equipment[i+1].equipment_LV;           
        boki->equipment[i].equipment_grade=boki->equipment[i+1].equipment_grade;
        boki->equipment[i].equipment_attack=boki->equipment[i+1].equipment_attack;
        boki->equipment[i].equipment_protect=boki->equipment[i+1].equipment_protect;
        boki->equipment[i].equipment_flag=boki->equipment[i+1].equipment_flag;
    }
    (boki->num)--;
}

void intro(){
    printf("\n\n\n              용사 '복이'는 신성한 검을 휘둘러 악을 처단하는 용감한 전사다.\n");
    sleep(2);
    printf("              그는 이번에도 바포메트를 처치해 달라는 의뢰를 받고, 모험을 떠나 말하는 섬에 도착한다.\n");
    sleep(2);
    printf("              이곳에서 그를 기다리는 건 어떤 위험이 있는지\n");
    sleep(2);
    printf("              그의 운명은 어디로 흘러갈지\n");
    sleep(2);
    printf("              모든 것은 그의 선택에 달려 있다.\n");
    sleep(2);
    system("clear");
}

void store_intro(){
    printf("\n\n\n              어? 못보던 얼굴인데...\n");
    sleep(2);
    printf("              아~ 이번에 바포메트를 쓰러트리겠다고 온 용사님이군요?\n");
    sleep(2);
    printf("              저는 이 섬의 잡화를 판매하는 '판도라' 라고합니다.\n");
    sleep(2);
    printf("              후후.....필요한 물건이 있나요?\n");
    sleep(2);
    system("clear");
}

void smelter_intro(){
    printf("\n\n\n              ...\n");
    sleep(2);
    printf("              자네, 보잘 것 없어 보이는군\n");
    sleep(2);
    printf("              그 무기로는 너가 원하는 걸 얻을 수 없어.\n");
    sleep(2);
    printf("              강화를 원한다면 이 몸이 해줄테니 가져와봐\n");
    sleep(2);
    system("clear");
}

void sanctum_intro(){
    printf("\n\n\n              이곳은 신성한 축복이 깃드는 장소입니다.\n");
    sleep(2);
    printf("              헌데 당신, 많이 지쳐보이는 군요.\n");
    sleep(2);
    printf("              힘이들면 언제든지 이곳에 와 몸과 마음을 정화하세요.\n");
    sleep(2);
    system("clear");
}

void boss_kill_outtro(){
    printf("\n\n\n              ..........!\n");
    sleep(2);
    printf("              제법 성장했군...\n");
    sleep(3);
    printf("              하지만 넌 C언어도 제대로 못쓰는 우매한 중생일 뿐..\n");
    sleep(2);
    printf("              이게 끝이아니지....\n");
    sleep(2);
    printf("              파이썬!!!!!!!!!!!!!\n");
    sleep(2);
    printf("              C++!!!!!!!!!!!!!!!\n");
    sleep(2);
    printf("              C#!!!!!!!!!!!!!!!!\n");
    sleep(2);
    printf("              DB!!!!!!!!!!!!!!!!\n");
    sleep(2);
    printf("              TCP IP!!!!!!!!!!!!\n");
    sleep(2);
    printf("              SQL!!!!!!!!!!!!!!!\n");
    sleep(2);
    printf("              대학원!!!!!!!!!!!!!!!\n");
    sleep(2);
    printf("              그래.. 나도 아직 대학원은 끝내지 못했지...\n");
    sleep(2);
    printf("              으하하하하!!!!!! 넌 결국 평생 공부할 뿐이다!!");
    sleep(2);
    printf("              이 게임은 결국 끝이 없는거야!!!\n"); 
    sleep(2);
    system("clear");
}

