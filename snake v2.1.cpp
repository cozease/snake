#include<graphics.h>
#include<easyx.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string.h>
#include<ctime>
#include<windows.h>
#include<chrono>
#include<thread>
#include <cmath>
#include <vector>
#include <algorithm>

#define SNAKE_NUM 500
using namespace std;
using namespace std::chrono;
//调用graphics图形库；键盘字符、系统时间的读取；定义贪吃蛇的最大长度

int n=70;                            //游戏难度
int v=0;                            //当前得分
int gametimes=0;                    //游戏次数
int highestscore=0;                 //最高分
int passscore=500;                  //通关分数
bool wise = false;                  //AUTO状态
enum DIR{Left,Right,up,down,emp};   //枚举方向(含空项)

//为了方便表述反向的direction类
class direction{
public:
    DIR d;
    direction reversed(){
        direction mt = *this ;
        switch(mt.d) {
            case Left:
                mt.d=Right;
                break;
            case Right:
                mt.d=Left;
                break;
            case up:
                mt.d=down;
                break;
            case down:
                mt.d=up;
                break;
            case emp:
                mt.d=emp;
        }
        return mt;
    }
}direction;
int vt = 0;
//寻线值
const int ROW = 640;
const int COL = 480;

//障碍物
struct barrier{
    int l;  //左边界坐标
    int r;  //上边界坐标
    int u;  //右边界坐标
    int d;  //下边界坐标
};
//障碍物的位置
barrier obstacle[15]={
        {340,360,380,360},
        {360,380,400,380},
        {380,400,380,360},
        {600,620,40,20},
        {580,600,60,40},
        {540,560,60,40},
        {470,490,300,180},
        {80,100,120,40},
        {30,50,420,300},
        {100,120,480,460},
        {220,320,260,240},
        {160,60,10},
        {240,60,10},
        {320,60,10}
};

//蛇
struct Snake{
    int size;               //长度
    DIR dir;                //运动方向
    int speed;              //速度
    POINT coor[SNAKE_NUM];  //蛇身坐标
}snake;

//普通食物
struct food{
    int x;          //横坐标
    int y;          //纵坐标
    int r;          //大小
    DWORD color;    //颜色
    bool flag;      //是否被吃
}food;
//高分食物
struct sweet{
    int x;
    int y;      //纵坐标
    int r;      //大小
    bool flag;  //是否存在
    int exist;  //存在计时
    bool eaten; //是否被吃
}sweet;
auto sweetstart=steady_clock::now();    //记录高分食物出现的时间

//智慧草
struct drug{
    int x;      //横坐标
    int y;      //纵坐标
    int r;      //大小
    bool flag;  //是否存在
}drug;

//得分函数
void yourPoint()
{
    char a='0',b='0',c='0';
    //当前分数
    v=(snake.size - 5)*10;
    b='0'+v%100/10;
    a='0'+v%1000/100;
    //显示计分器
    settextstyle(20,10,_T("Comic Sans MS"));
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    outtextxy(650, 190, _T("Your Points:"));
    outtextxy(680, 230, a);
    outtextxy(695, 230, b);
    outtextxy(710, 230, c);
}

//游戏初始化
void GameInit(){
    //初始化蛇
    snake.size=5;
    snake.speed=10;
    snake.dir=Right;
    for(int i=0;i<snake.size;i++){
        snake.coor[i].x=60-10*i;
        snake.coor[i].y=20;
    }
    //初始化普通食物
    food.x=rand()%620+10;
    food.y=rand()%460+10;
    food.color=RGB(0,0,0);
    food.r=5;
    food.flag=true;
    //确保普通食物生成在障碍物外
    for(int i=0;i<11;++i){
        if(food.x>=obstacle[i].l-5&&food.x<=obstacle[i].r+5
           &&food.y>=obstacle[i].d-5&&food.y<=obstacle[i].u+5)
            food.flag=false;
    }
    for(int i=11;i<14;++i){
        if(food.x>=obstacle[i].l-obstacle[i].u-5&&food.x<=obstacle[i].l+obstacle[i].u+5
           &&food.y<=obstacle[i].r+obstacle[i].u+5&&food.y>=obstacle[i].r-obstacle[i].u-5)
            food.flag=false;
    }
    //初始化高分食物
    sweet.flag=false;
    sweet.eaten=true;
    //初始化智慧草
    drug.flag=false;
}

//绘制游戏界面
void GameDraw(){
    BeginBatchDraw();
    //背景
    setbkcolor(RGB(28,115,119));
    cleardevice();
    //计分器
    setfillcolor(WHITE);
    solidrectangle(640,0,800,480);
    yourPoint();
    //蛇
    setfillcolor(RED);
    solidcircle(snake.coor[0].x,snake.coor[0].y,7);
    for(int i=1;i<snake.size;i++){
        solidcircle(snake.coor[i].x,snake.coor[i].y,5);
    }
    //食物
    if(food.flag){
        setfillcolor(YELLOW);
        solidcircle(food.x,food.y,food.r);
    }
    //障碍物
    setfillcolor(GREEN);
    for(int i=0;i<11;i++){
        fillrectangle(obstacle[i].l,obstacle[i].u,obstacle[i].r,obstacle[i].d);
    }
    for(int i=11;i<14;i++){
        fillcircle(obstacle[i].l,obstacle[i].r,obstacle[i].u);
    }
    //高分食物
    if(sweet.flag){
        setfillcolor(RGB(240,134,80));
        solidcircle(sweet.x,sweet.y,sweet.r);
        char countdown='0'+sweet.exist;
        settextstyle(40,20,_T("Arial Black"));
        settextcolor(RED);
        setbkmode(TRANSPARENT);
        outtextxy(700,400,countdown);
    }
    //智慧草
    if(drug.flag){
        setfillcolor(RGB(95,158,160));
        solidcircle(drug.x,drug.y,drug.r);
    }
    //AUTO状态
    if(wise){
        settextstyle(40,20,_T("Chiller"));
        settextcolor(RGB(255,192,24));
        outtextxy(660,45,_T("AUTO"));
    }
    EndBatchDraw();
}

//蛇的移动
void snakemove(){
    //身体的前进
    for(int i=snake.size-1;i>0;i--) snake.coor[i]=snake.coor[i-1];
    //头的前进
    switch(snake.dir){
        case up:
            snake.coor[0].y-=snake.speed;
            break;
        case down:
            snake.coor[0].y+=snake.speed;
            break;
        case Right:
            snake.coor[0].x+=snake.speed;
            break;
        case Left:
            snake.coor[0].x-=snake.speed;
            break;
    }
    //边界穿梭，不返回到边上的原因是避免穿模
    if(snake.coor[0].x<=0) snake.coor[0].x=630;
    else if(snake.coor[0].x>=640) snake.coor[0].x=10;
    else if(snake.coor[0].y<=0) snake.coor[0].y=470;
    else if(snake.coor[0].y>=480) snake.coor[0].y=10;
}

//蛇的转向
void KeyControl(){
    //非阻塞地读取输入，保证图形的连贯
    if(_kbhit()){
        //判读读入的按键
        switch(_getch()){
            case 'w':
            case 'W':
            case 72:
                if(snake.dir!=down) snake.dir=up;
                break;
            case 's':
            case 'S':
            case 80:
                if(snake.dir!=up) snake.dir=down;
                break;
            case 'a':
            case 'A':
            case 75:
                if(snake.dir!=Right) snake.dir=Left;
                break;
            case 'd':
            case 'D':
            case 77:
                if(snake.dir!=Left) snake.dir=Right;
                break;
            //空格键暂停
            case ' ':
                //再按一次空格键继续
                while(1){
                    if(_getch()==' ') return;
                }
                break;
        }
    }
}

//吃到食物
void eatfood(){
    //是否吃到普通食物
    if(food.flag&&snake.coor[0].x>=food.x-food.r&&snake.coor[0].x<=food.x+food.r
       &&snake.coor[0].y>=food.y-food.r&&snake.coor[0].y<=food.y+food.r){
        food.flag=false;    //改变普通食物的存在状态
        snake.size++;       //蛇长+1
    }
    //生成新的普通食物
    if(!food.flag){
        food.x=rand()%620+10;
        food.y=rand()%460+10;
        food.color=RGB(0,0,0);
        food.r=5;
        food.flag=true;

        for(int i=0;i<11;++i){
            if(food.x>=obstacle[i].l-10&&food.x<=obstacle[i].r+10
               &&food.y>=obstacle[i].d-10&&food.y<=obstacle[i].u+10)
                food.flag=false;
        }
        for(int i=11;i<14;++i){
            if(food.x>=obstacle[i].l-obstacle[i].u-10&&food.x<=obstacle[i].l+obstacle[i].u+10
               &&food.y<=obstacle[i].r+obstacle[i].u+10&&food.y>=obstacle[i].r-obstacle[i].u-10)
                food.flag=false;
        }
    }
    //每50分生成一个额外的高分食物
    if(v%50==0&&v!=0&&!sweet.flag&&sweet.eaten){
        sweet.flag=true;
        sweet.x=rand()%620+10;
        sweet.y=rand()%460+10;
        //确保高分食物生成在普通食物和障碍物外
        if(sweet.x-food.x<=10&&sweet.x-food.x>=-10
           &&sweet.y-food.y<=10&&sweet.y-food.y>=-10){
            sweet.x=rand()%620+10;
            sweet.y=rand()%460+10;
        }
        for(int i=0;i<11;++i){
            if(sweet.x>=obstacle[i].l-10&&sweet.x<=obstacle[i].r+10
               &&sweet.y>=obstacle[i].d-10&&sweet.y<=obstacle[i].u+10){
                sweet.x=rand()%620+10;
                sweet.y=rand()%460+10;
            }
        }
        for(int i=11;i<14;++i){
            if(sweet.x>=obstacle[i].l-obstacle[i].u-10&&sweet.x<=obstacle[i].l+obstacle[i].u+10
               &&sweet.y<=obstacle[i].r+obstacle[i].u+10&&sweet.y>=obstacle[i].r-obstacle[i].u-10){
                sweet.x=rand()%620+10;
                sweet.y=rand()%460+10;
            }
        }
        sweet.r=7;
        sweet.exist=5;
        //更新高分食物的产生时间
        sweetstart=steady_clock::now();
    }
    //是否吃到高分食物
    if(sweet.flag&&snake.coor[0].x>=sweet.x-sweet.r&&snake.coor[0].x<=sweet.x+sweet.r
       &&snake.coor[0].y>=sweet.y-sweet.r&&snake.coor[0].y<=sweet.y+sweet.r){
        sweet.flag=false;   //改变高分食物的存在状态
        snake.size+=2;      //蛇长+2
        sweet.eaten=true;   //改变被吃状态
    }
    //是否吃到智慧草
    if(drug.flag&&snake.coor[0].x>=drug.x-drug.r&&snake.coor[0].x<=drug.x+drug.r
       &&snake.coor[0].y>=drug.y-drug.r&&snake.coor[0].y<=drug.y+drug.r){
        drug.flag=false;   //改变智慧草的存在状态
        wise = true;       //改变AUTO状态
        snake.size+=2;     //蛇长+2
        vt = v + 20;       //更新产生智慧草的分数
    }
    //每80分生成一个智慧草
    if(!wise&&v%80==0&&!drug.flag){
        drug.flag = true;
        drug.x=rand()%620+10;
        drug.y=rand()%460+10;
        //确保智慧草生成在障碍物外
        if(drug.x-food.x<=10&&drug.x-food.x>=-10
           &&drug.y-food.y<=10&&drug.y-food.y>=-10){
            drug.x=rand()%620+10;
            drug.y=rand()%460+10;
        }
        if(drug.x-sweet.x<=7&&drug.x-sweet.x>=-7
           &&drug.y-sweet.y<=7&&drug.y-sweet.y>=-7){
            drug.x=rand()%620+10;
            drug.y=rand()%460+10;
        }
        for(int i=0;i<11;++i){
            if(drug.x>=obstacle[i].l-10&&drug.x<=obstacle[i].r+10
               &&drug.y>=obstacle[i].d-10&&drug.y<=obstacle[i].u+10){
                drug.x=rand()%620+10;
                drug.y=rand()%460+10;
            }
        }
        for(int i=11;i<14;++i){
            if(drug.x>=obstacle[i].l-obstacle[i].u-10&&drug.x<=obstacle[i].l+obstacle[i].u+10
               &&drug.y<=obstacle[i].r+obstacle[i].u+10&&drug.y>=obstacle[i].r-obstacle[i].u-10){
                drug.x=rand()%620+10;
                drug.y=rand()%460+10;
            }
        }
        drug.r=5;
    }

}

bool die(){
    bool live = true;
    //撞到蛇身
    for(int i=1;i<snake.size;i++) if(snake.coor[0].x==snake.coor[i].x&&snake.coor[0].y==snake.coor[i].y) live = false;
    //撞到障碍物
    //bar
    for(int i=0;i<11;i++){
        if(snake.coor[0].x>=obstacle[i].l&&snake.coor[0].x<=obstacle[i].r
           &&snake.coor[0].y<=obstacle[i].u&&snake.coor[0].y>=obstacle[i].d)
            live = false;
    }
    //circle
    for(int i=11;i<14;i++){
        if(snake.coor[0].x>=obstacle[i].l-obstacle[i].u&&snake.coor[0].x<=obstacle[i].l+obstacle[i].u
           &&snake.coor[0].y>=obstacle[i].r-obstacle[i].u&&snake.coor[0].y<=obstacle[i].r+obstacle[i].u)
            live = false;
    }
    return live;
}

//(x,y)处的死亡事件
bool diek(int x,int y){
    bool live = true;
    //撞到蛇身
    for(int i=1;i<snake.size;i++) if(x==snake.coor[i].x&&y==snake.coor[i].y) live = false;
    //撞到障碍物
    //bar
    for(int i=0;i<11;i++){
        if(x>=obstacle[i].l&&x<=obstacle[i].r
           &&y<=obstacle[i].u&&y>=obstacle[i].d)
            live = false;
    }
    //circle
    for(int i=11;i<14;i++){
        if(x>=obstacle[i].l-obstacle[i].u&&x<=obstacle[i].l+obstacle[i].u
           &&y>=obstacle[i].r-obstacle[i].u&&y<=obstacle[i].r+obstacle[i].u)
            live = false;
    }
    return live;
}

//检测dir方向上length单位有无死亡事件
bool detect(DIR dir,double length){
    bool empty = true;
    int x = snake.coor[0].x;
    int y = snake.coor[0].y;
    switch (dir) {
        case Left:

            empty= diek(x-length*7,y);
            break;
        case Right:

            empty= diek(x+length*7,y);
            //snake.coor[0].x-=length;
            break;
        case up:

            empty= diek(x,y+length*7);
            //snake.coor[0].y-=length;
            break;
        case down:

            empty= diek(x,y-length*7);
            //snake.coor[0].y+=length;
            break;
    }
    return empty;
}
//确定最短距离的方向
DIR decidex(){
    DIR dirx = Left;
    int disl,dism,disr;//穿过左界,不穿过地图,穿过右界的距离
    disl=food.x-(drug.x- ROW);
    dism=food.x- drug.x;
    disr=food.x-(drug.x + ROW);
    if(dism<0)dirx=(abs(disl)>abs(dism))?Left:Right;
    else if(dism==0)dirx = emp;
    else dirx = (abs(disr)>abs(dism))?Right:Left;
    return dirx;
}
DIR decidey(){
    DIR diry = up;
    int disu,dism,disd;//穿过上界,不穿过地图,穿过下界的距离
    disu=food.y-(drug.y- COL);
    dism=food.y- drug.y;
    disd=food.y-(drug.y + COL);
    if(dism<0)diry=(abs(disu)>abs(dism))?up:down;
    else if(dism==0)diry = emp;
    else diry = (abs(disd)>abs(dism))?down:up;
    return diry;
}
//返回前进的方向(优先级为dirx,diry,dirx反,diry反)
DIR detectall(DIR dirx=emp,DIR diry=emp){
    class direction dirt,Dirx,Diry ,Dirm,Dirn;
    Dirx.d=dirx;
    Diry.d=diry;
    Dirm.d=decidex();
    Dirn.d=decidey();
    dirt.d = Dirx.d;
    if(!(detect(dirt.d,1)&& detect(dirt.d,2))||dirt.d==emp&&Diry.reversed().d!=snake.dir) {
        dirt.d = Diry.d;
         if (!(detect(dirt.d, 1)&& detect(dirt.d,2)) || dirt.d == emp&&Dirx.d!=snake.dir) {
             dirt.d = Dirx.reversed().d;
             if (!(detect(dirt.d, 1)&& detect(dirt.d,2) ) || dirt.d == emp&&Diry.d!=snake.dir)
                 dirt.d = Diry.reversed().d;
         }
    }
    if(dirx == emp && !(detect(dirt.d, 1)&& detect(dirt.d,2) ))dirt.d = Dirm.reversed().d;
    if(diry == emp && !(detect(dirt.d, 1)&& detect(dirt.d,2) ))dirt.d = Dirn.reversed().d;
    return dirt.d;
}
//蛇的自动移动,一单位,targetdir方向
void automove(DIR targetdir){
    snake.dir = targetdir;
    //下基本同snakemove
    //身体的前进
    for(int i=snake.size-1;i>0;i--) snake.coor[i]=snake.coor[i-1];
    //头的前进
    switch(snake.dir){
        case up:
            snake.coor[0].y--;
            break;
        case down:
            snake.coor[0].y++;
            break;
        case Right:
            snake.coor[0].x++;
            break;
        case Left:
            snake.coor[0].x--;
            break;
    }
    //边界穿梭，不返回到边上的原因是避免穿模
    if(snake.coor[0].x<=0) snake.coor[0].x=630;
    else if(snake.coor[0].x>=640) snake.coor[0].x=10;
    else if(snake.coor[0].y<=0) snake.coor[0].y=470;
    else if(snake.coor[0].y>=480) snake.coor[0].y=10;
}
//抵达判断
bool getit(){
    return (vt!=v);
}
//自动寻线函数
void automatic(){
    DIR movex = decidex();
    DIR movey = decidey();
    if(food.flag&&snake.coor[0].x>=food.x-food.r&&snake.coor[0].x<=food.x+food.r) movex=emp;
    if(snake.coor[0].y>=food.y-food.r&&snake.coor[0].y<=food.y+food.r) movey=emp;
    DIR move = detectall(movex,movey);
    automove(move);
}
//开始界面
void startscreen();
//死亡界面
void gameover(){
    gametimes++;    //游戏次数+1
    highestscore=(highestscore>v)?highestscore:v;   //判断是否刷新最高分
    setbkcolor(BLACK);
    cleardevice();
    setbkmode(TRANSPARENT);
    settextcolor(RED);
    settextstyle(80,40,_T("AniMe Matrix Font"));
    outtextxy(210,60,_T("GAMEOVER"));
    settextcolor(WHITE);
    settextstyle(25,10,_T("Comic Sans MS"));
    outtextxy(150,180,_T("Congratulations! Your score this time is:"));
    outtextxy(150,220,_T("The number of times you played is:"));
    outtextxy(150,260,_T("Your current highest score is:"));
    settextcolor(YELLOW);
    settextstyle(25,10,_T("Castellar"));
    char a='0',b='0',c='0';
    b='0'+v%100/10;
    a='0'+v%1000/100;
    outtextxy(555,182,a);
    outtextxy(570,182,b);
    outtextxy(585,182,c);
    char d='0',e='0';
    d='0'+gametimes%10;
    e='0'+gametimes/10;
    outtextxy(505,222,e);
    outtextxy(520,222,d);
    b='0'+highestscore%100/10;
    a='0'+highestscore%1000/100;
    outtextxy(455,262,a);
    outtextxy(470,262,b);
    outtextxy(485,262,c);
    settextcolor(RGB(100,100,100));
    settextstyle(25,10,_T("Arial"));
    outtextxy(150,350,_T("Would you like to play once more?"));
    outtextxy(150,390,_T("Press \"C\" to continue or \"R\" to return (lose records)."));
    char flag;
    flag=_getch();
    while(1){
        switch(flag){
            case 'C':
            case 'c':
                GameInit();
                return;
            case 'R':
            case 'r':
                gametimes=0;
                highestscore=0;
                startscreen();
                GameInit();
                return;
            default:
                flag=_getch();
        }
    }
}
//难度设置界面
void Difficulty(){
    setbkcolor(BLACK);
    cleardevice();
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(40,20,_T("Segoe UI"));
    outtextxy(100,50,_T("Press \"E\" for EASY mode."));
    outtextxy(100,130,_T("Press \"N\" for NORMAL mode."));
    outtextxy(100,210,_T("Press \"H\" for HARD mode."));
    outtextxy(100,290,_T("Press \"D\" for DEADLY mode."));
    char difficulty;
    difficulty=_getch();
    switch(difficulty){
        case 'E':
        case 'e':
            n=100;
            startscreen();
            return;
        case 'N':
        case 'n':
            n=70;
            startscreen();
            return;
        case 'H':
        case 'h':
            n=50;
            startscreen();
            return;
        case 'D':
        case 'd':
            n=20;
            startscreen();
            return;
        default:
            Difficulty();
            return;
    }
    return;
}
//规则查看界面
void Rules(){
    setbkcolor(BLACK);
    cleardevice();
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(20,10,_T("Segoe UI"));
    outtextxy(50,40,_T("1. To control the direction of your snake by pressing the direction keys"));
    outtextxy(50,65,_T("    or the keys of \"W\",\"A\",\"S\" and \"D\"."));
    outtextxy(50,90,_T("2. To pause the game by pressing the space key."));
    outtextxy(50,115,_T("3. Food: 10 points."));
    outtextxy(50,140,_T("4. Sweet: 20 points (Appear when your score is divisible by 50)."));
    outtextxy(50,165,_T("5. Drug (Eat it and then your snake will find the next food itself):"));
    outtextxy(50,190,_T("    20 points (Appear when your score is divisible by 50)."));
    outtextxy(50,215,_T("6. To win the game by getting 500 points."));
    settextcolor(RGB(100,100,100));
    outtextxy(50,300,_T("Press any key to return."));
    _getch();
    startscreen();
    return;
}
void startscreen(){
    setbkcolor(BLACK);
    cleardevice();
    setbkmode(TRANSPARENT);
    settextcolor(GREEN);
    settextstyle(80,40,_T("Showcard Gothic"));
    outtextxy(300,80, _T("SNAKE"));
    settextcolor(WHITE);
    settextstyle(40,20,_T("Segoe UI"));
    outtextxy(360,200,_T("1 Start")); 
    outtextxy(335,240,_T("2 Difficulty"));
    outtextxy(360,310,_T("3 Rules"));
    outtextxy(370,350,_T("0 Exit"));
    settextcolor(RGB(100,100,100));
    settextstyle(30,15,_T("Arial"));
    if(n==70) outtextxy(280,280,_T("Present: NORMAL"));
    else if(n==100) outtextxy(310,280,_T("Present: EASY"));
    else if(n==50) outtextxy(310,280,_T("Present: HARD"));
    else if(n==20) outtextxy(290,280,_T("Present: DEADLY"));
    settextstyle(20,10,_T("Arial"));
    outtextxy(180,420,_T("Enter a number to select the corresponding option."));
    char choice;
    choice=_getch();
    switch(choice){
        case '0':
            exit(0);
        case '1':
            return;
        case '2':
            Difficulty();
            return;
        case '3':
            Rules();
            return;
        default:
            startscreen();
            return;
    }
    return;
}
//通关界面
void win(){
    gametimes++;
    setbkcolor(BLACK);
    cleardevice();
    setbkmode(TRANSPARENT);
    settextcolor(YELLOW);
    settextstyle(80,40,_T("Segoe Script"));
    outtextxy(230,70,_T("YOU WIN !"));
    settextcolor(WHITE);
    settextstyle(25,10,_T("Comic Sans MS"));
    outtextxy(90,200,_T("Unbelievable! You pass the game just by trying for"));
    outtextxy(630,200,_T("times."));
    settextcolor(RGB(115,251,253));
    settextstyle(25,10,_T("Castellar"));
    char d='0',e='0';
    d='0'+gametimes%10;
    e='0'+gametimes/10;
    outtextxy(600,202,e);
    outtextxy(615,202,d);
    settextcolor(RGB(100,100,100));
    settextstyle(25,10,_T("Arial"));
    outtextxy(200,350,_T("Would you like to start a new game?"));
    outtextxy(200,390,_T("Press \"Y\" for YES or \"N\" for NO."));
    char flag;
    flag=_getch();
    while(1){
        switch(flag){
            case 'Y':
            case 'y':
                gametimes=0;
                highestscore=0;
                startscreen();
                GameInit();
                return;
            case 'N':
            case 'n':
                exit(0);
            default:
                flag=_getch();
        }
    }
}
int main(){
    //绘制界面
    initgraph(800,480,SHOWCONSOLE);
    //将开机秒数作为随机数种子，保证生成食物的随机性
    srand(GetTickCount());
    startscreen();
    GameInit();
    while(1){
        GameDraw();
        snakemove();
        //高分食物存在倒计时
        int sweet_uneaten;
        if(sweet.flag){
            auto sweetcurrent=steady_clock::now();
            auto sweetelapse=duration_cast<seconds>(sweetcurrent-sweetstart).count();
            if(sweetelapse>=1){
                sweet.exist-=1;
                sweetstart=steady_clock::now();
            }
            if(sweet.exist==0){
                sweet.flag=false;
                sweet.eaten=false;
                sweet_uneaten=v;
            }
        }
        //如果高分食物没被吃，那么在吃到下一个普通食物后才改变被吃状态
        if(!sweet.eaten&&sweet_uneaten+10==v) sweet.eaten=true;
       //吃到智慧草则开始寻线
        KeyControl();
        eatfood();
        yourPoint();
        if(getit()) wise = false;
        if(wise) automatic();
        Sleep(n);
        if(!die()) gameover();
        if(v>=passscore) win();
    }
    return 0;
}
