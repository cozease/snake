#include<graphics.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string.h>
#include<ctime>
#include<windows.h>
#include<chrono>
#include<thread>
#define SNAKE_NUM 500
using namespace std;
using namespace std::chrono;
//调用graphics图形库；键盘字符、系统时间的读取；定义贪吃蛇的最大长度
int n=0;                        //游戏难度
int v=0;                        //当前得分
int gametimes=0;                //游戏次数
int highestscore=0;             //最高分
int passscore=300;              //通关分数
enum DIR{Left,Right,up,down};   //枚举方向
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
    int dir;                //运动方向
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
    //绘制界面
    initgraph(800,480,SHOWCONSOLE);
    //将开机秒数作为随机数种子，保证生成食物的随机性
    srand(GetTickCount());
    //初始化蛇
    snake.size=5;
    snake.speed=10;
    snake.dir=Right;
    for(int i=0;i<snake.size;i++){
        snake.coor[i].x=60-10*i;
        snake.coor[i].y=20;
    }
    //初始化食物
    food.x=rand()%620+10;
    food.y=rand()%460+10;
    food.color=RGB(0,0,0);
    food.r=5;
    food.flag=true;
    //确保食物生成在障碍物外
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
    if(snake.coor[0].x==0) snake.coor[0].x=630;
    else if(snake.coor[0].x==640) snake.coor[0].x=10;
    else if(snake.coor[0].y==0) snake.coor[0].y=470;
    else if(snake.coor[0].y==480) snake.coor[0].y=10;
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
}
//死亡界面
void gameover(){
    system("cls");
    gametimes++;    //游戏次数+1
    highestscore=(highestscore>v)?highestscore:v;   //判断是否刷新最高分
    cout<<"\033[1;31m"<<"GAMEOVER"<<"\033[0m"<<endl;
    cout<<"Congratulations! You got "<<"\033[1;33m"<<v<<"\033[0m"<<" points this time."<<endl;
    cout<<endl;
    cout<<"You have played for "<<"\033[1;34m"<<gametimes<<"\033[0m"<<" time(s)."<<endl;
    cout<<"Your current highest score is "<<"\033[1;33m"<<highestscore<<"\033[0m"<<'.'<<endl;
    cout<<endl;
    cout<<"Play once more?"<<endl;
    cout<<"Please input YES or NO.\nATTENTION: Any other words will quit the game and lose your records!"<<endl;
    //是否继续游戏
    string a;
    cin>>a;
    if(a=="YES"){
        //选择难度
        cout<<"Please select difficulty (from easy to difficult): C, B, A, S."<<endl;
        char b;
        cin>>b;
        if(b=='A') n=50;
        else if(b=='S') n=10;
        else if(b=='C') n=100;
        else n=70;
        GameInit();
    }
    else{
        cout<<"Goodbye!\nPress any key to exit.";
        _getch();
        exit(0);
    }
}
//死亡条件
void die(int &n){
    //撞到蛇身
    for(int i=1;i<snake.size;i++) if(snake.coor[0].x==snake.coor[i].x&&snake.coor[0].y==snake.coor[i].y) gameover();
    //撞到障碍物
    for(int i=0;i<11;i++){
        if(snake.coor[0].x>=obstacle[i].l&&snake.coor[0].x<=obstacle[i].r
        &&snake.coor[0].y<=obstacle[i].u&&snake.coor[0].y>=obstacle[i].d)
            gameover();
    }
    for(int i=11;i<14;i++){
        if(snake.coor[0].x>=obstacle[i].l-obstacle[i].u&&snake.coor[0].x<=obstacle[i].l+obstacle[i].u
        &&snake.coor[0].y>=obstacle[i].r-obstacle[i].u&&snake.coor[0].y<=obstacle[i].r+obstacle[i].u)
            gameover();
    }
}
int main(){ 
    cout<<"Welcome to "<<"\033[1;32m"<<"SNAKE"<<"\033[0m"<<"!"<<endl;
    cout<<"To control the direction of your snake by pressing the direction keys or the keys of \"W\",\"A\",\"S\" and \"D\"."<<endl;
    cout<<"To pause the game by pressing the space key."<<endl;
    cout<<"\033[1;33m"<<"Have fun!"<<"\033[0m"<<endl;
    cout<<endl;
    cout<<"Please select difficulty (from easy to difficult): C, B, A, S."<<endl;
    char a;
    cin>>a;
    if(a=='A') n=50;
    else if(a=='S') n=10;
    else if(a=='C') n=100;
    else n=70;
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
        KeyControl();
        eatfood();
        Sleep(n);
        die(n);
        //通关界面
        if(v==passscore){
            gametimes++;
            system("cls");
            cout<<"Unbelievable! You pass the game just by trying for "<<"\033[1;33m"<<gametimes<<"\033[0m"<<" time(s)!"<<endl;
            cout<<"Would you like to start a new game?"<<endl;
            cout<<"Please input YES or NO.\nATTENTION: Any other words will quit the game."<<endl;
            string a;
            cin>>a;
            if(a=="YES"){
                gametimes=0;
                highestscore=0;
                cout<<"Please select difficulty (from easy to difficult): C, B, A, S."<<endl;
                char b;
                cin>>b;
                if(b=='A') n=50;
                else if(b=='S') n=10;
                else if(b=='C') n=100;
                else n=70;
                GameInit();
            }
            else{
                cout<<"Goodbye!\nPress any key to exit.";
                _getch();
                exit(0);
            }
        }
	}
	return 0;
}