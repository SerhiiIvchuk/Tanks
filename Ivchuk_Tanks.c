#include <stdio.h>
#include <windows.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>

/*
Управління танком гравця:
w-вгору
d-праворуч
s-донизу
a-ліворуч
f-вистріл
e-вихід з гри
*/

//розмір ігрового поля
#define X 115
#define Y 60
#define T 300  //кількість мілісекунд в затримці
#define TS 3  //швидкість танка
#define BS 5 //швидкість кулі

void gotoxy(float x, float y);  // функція переміщення курсора
void imgTank( int x, int y, int trn); //малюємо танк на заданій координаті відповідно до його напрямку
void hideTank( int x, int y, int trn); //замальовуємо танк на заданій координаті відповідно до його напрямку
void hitedTank(int x, int y, int trn); //зображення підбитого танка
void hidehitedTank(int x, int y, int trn); //замальовуємо підбитий танк
void Start();                              //вивід на екран заставки
void End(int hits, int mehits);        //вивід на екран результатів
void moveBullet(int *fBullet, int *xBullet,int *yBullet, int *oldxBullet,int *oldyBullet, int *dirBullet,int *x,int *y, int *fTank);//забезпечує переміщення кулі та контролює влучання
void backTurn(int *x,int *y, int *cpx,int *cpy, int *c); //розвертає танк при під'їзді до межігрового поля
void avoidCollision(int *x,int *y,int *mx,int *my,int *cpx,int *cpy,int *c);//запобігає накладання зображення танка противника на наш танк чи інший обєкт в грі
void enemyShoot(int *c, int *mx, int *my, int *y, int *x, int *fEnemBullet, int *xEnemBullet, int *yEnemBullet, int *dirEnemBullet);//вчасний вистріл ворожого танка
void renderTank(int *fTank, int *x, int *y, int *cpx, int *cpy, int *c, int *oldc, int *fhit, int *hits);//замальовує попереднє положення танка, малює його на поточних координатах

int main(){
	int x,y,cpx,cpy, mx,my, mcpx,mcpy;//поточні та попередні координати ворожого танка та танка гравця
	int c, oldc ; //  поточний та попередній напрямок танка компютера
	int intKey, intCpkey,cpk; //напрямок танка гравця
	char key,cpkey;//поточне та попереднє значення натиснутої клавіші
	int fBullet=0,dirBullet,xBullet,yBullet, oldxBullet, oldyBullet ;// гравець - прапорець кулі(є/немає), напрям кулі, поточні координати кулі, попередні координати кулі.
	
	int fEnemBullet=1, xEnemBullet, yEnemBullet, oldxEnemBullet=0, oldyEnemBullet, dirEnemBullet, fisEnemBullet;//компютер -прапорець кулі(є/немає), напрям кулі, поточні координати кулі, попередні координати кулі.
	
	int  fTank=1, fmyTank=1;//прапорці непідбитий/підбитий танк компютера/гравця
	
	int fShift=1;// прапорець чергування ходу в циклі компютер/гравець
	int fhit=7, fmehit=7;// лічильник часу відображення решток підбитого танка
	int i,f;
	int hits=0, mehits=0;// лічильники кількості підбитих танків
	

	
	//gotoxy(x,y);
	//printf("%c",'#');
	Start();//виводимо закладку на екран
	for(i=0;i<=X;i++){
		gotoxy(i,0);
		printf("#");
		gotoxy(i,Y);
		printf("#");
	}
	
	for(i=1;i<=Y;i++){
		gotoxy(0,i);
		printf("#");
		gotoxy(X,i);
		printf("#");
	}
	
	mx=X/2;
	my=Y/2;
	x=mx+4;
	y=my;
	c=2;
	
	cpx=cpy=10;
	f=0;
	
	xBullet=yBullet=oldxBullet=oldyBullet=0;
	mcpx=mcpy=10;
	intKey=0;
	do{
		
		renderTank(&fTank,&x,&y,&cpx,&cpy,&c,&oldc,&fhit,&hits);
		
		renderTank(&fmyTank,&mx,&my,&mcpx,&mcpy,&intKey,&intCpkey,&fmehit,&mehits);
		
		
		
		oldc=c;//попередньому напрямку танка противника присвоюємо його поточний напрямок
	    cpkey=key;//попередньому напрямку танка гравця присвоюємо його поточний напрямок
	    
	    //*****************
	    if(oldxBullet!=0){        // замальовуємо зображення гулі гравця на попередніх координатах
	      gotoxy(oldxBullet,oldyBullet);
		  printf(" ");	
	    }
		
		
		if(fBullet==1){            //даємо зображення кулі гравця на поточних координатах
	    	gotoxy(xBullet,yBullet);
		    printf("#");
		}
		//*****************
		
		//******************
		    if(oldxEnemBullet!=0){  //замальовуємо кулю компютера на попередньому ході
	      gotoxy(oldxEnemBullet,oldyEnemBullet);
		  printf(" ");	
	    }
		
		
		if(fEnemBullet==1){ //якщо був вистріл, то малюємо кулю противника на поточних координатах
	    	gotoxy(xEnemBullet,yEnemBullet);
		    printf("#");
		}
		//******************
		Sleep(T);	//затримуємо виток циклу на вказану кількість мілісекунд
		mcpx=mx;     // запамятовуємо поточні координати центру танка
	 	mcpy=my;
		if((!kbhit())||(fShift==0)){  //якщо клавіша не була нажата або якщо на попередньоу витку циклу  хід був за гравцем - віддаємо подальше виконання витка компютеру
		   //*******Bullets*****************
           if(fBullet==1){   //рахуємо нові стани кулі гравця та її можливу дію на противника
              moveBullet(&fBullet, &xBullet, &yBullet, &oldxBullet, &oldyBullet, &dirBullet, &x, &y, &fTank);
  	            
			}
    //*******BulletsEnd**************
    //********EnemyBullets***********
    if(fEnemBullet==1){ // робимо обрахунки нових данних для кулі противника
         moveBullet(&fEnemBullet,&xEnemBullet,&yEnemBullet,&oldxEnemBullet,&oldyEnemBullet,&dirEnemBullet,&mx,&my,&fmyTank);
    	
	}
    
    //*****EndEnemyBullets***********
       cpx=x;
	   cpy=y;
       if(fTank==1){ // поки танк противника не підбитий готуємо йому данні для наступного витка циклу
		
	if(f<=10){  // танк компютера робить 10 ходів, а потім довільно змінює напрямок руху
		switch(c){
			
			case 0: y=y-TS; break;
			case 1: x=x+TS; break;
			case 2: y=y+TS; break;
			case 3: x=x-TS; break;
		}
	} else{  
		 f=0;
		 c=rand()%4;
	}
	//якщо танк дійшов до краю ігрового поля - напрямок його руху змінюємо на протележний	
	if((x<=1)||(x>=X-1)||(y<=1)||(y>=Y-1)){ 
		backTurn(&x,&y,&cpx,&cpy,&c);
		
	} else {
		    avoidCollision(&x,&y,&mx,&my,&cpx,&cpy,&c);
		
	}
	
    ++f;
	}
	//*********EnemyShoot**************
	if(fEnemBullet==0){ //якщо на лінії руху танка противника зявився танк граця, готуємо вистріл
		enemyShoot(&c,&mx,&my,&y,&x,&fEnemBullet,&xEnemBullet,&yEnemBullet,&dirEnemBullet);
		
	}
	//**********EndEnemyShoot****************
		   fShift=1; //наступний хід віддаємо гравцю,якщо він натисне клавішу
		} else { //обробка ходу гравця
	    	
	    	key=getch();
	    	
		//mcpx=mx;     // запамятовуємо поточні координати центру танка
	 	//mcpy=my;
	    intCpkey=intKey;
	
		
		if(key=='d') {
			intKey=1;
			mx=mx+TS;
		
		}
		if(key=='w') {
			intKey=0;
			my=my-TS;
		}
		if(key=='s'){
			intKey=2;
			my=my+TS;
	 	} 
		if(key=='a'){
			intKey=3;
			mx=mx-TS;
		} 
		
	 if((mx<=1)||(mx>=X-1)||(my<=1)||(my>=Y-1)) {//if(GamField[mx][my]=='#'){  //якщо танк дійшов до краю ігрового поля - його координати в цьому напрямку не змінюємо ()
			mx=mcpx;                                                          // дана умова повністю спрацьовує при кроці координат танка ==1, але наразі у грі встановлено ==3
			my=mcpy;
		} 
		
		if(key=='f'){ //
			fBullet=1;
			dirBullet=intCpkey;
			xBullet=mcpx;
			yBullet=mcpy;
			
			
			
		}
		fShift=0; //
		} 
	}while (key!='e');
	
	system("cls");
	End(hits,mehits); // виведення результатів.
	return 0;
}

void gotoxy(float x, float y) {
 HANDLE StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
 COORD coord = { x, y };
 SetConsoleCursorPosition(StdOut, coord);
}

void imgTank( int x, int y, int trn)
{
    switch(trn){
		case 3: gotoxy(x,y); printf("%c",'#'); gotoxy(x,y-1); printf("%c",'#'); gotoxy(x,y+1); printf("%c",'#'); gotoxy(x-1,y);printf("%c",'#');  gotoxy(x+1,y-1); printf("%c",'#'); gotoxy(x+1,y+1); printf("%c",'#');  break;
		case 2: gotoxy(x,y); printf("%c",'#'); gotoxy(x-1,y); printf("%c",'#'); gotoxy(x+1,y); printf("%c",'#'); gotoxy(x,y+1); printf("%c",'#'); gotoxy(x+1,y-1); printf("%c",'#'); gotoxy(x-1,y-1); printf("%c",'#'); break;
		case 1:gotoxy(x,y); printf("%c",'#'); gotoxy(x,y-1); printf("%c",'#'); gotoxy(x,y+1); printf("%c",'#'); gotoxy(x-1,y+1); printf("%c",'#'); gotoxy(x-1,y-1); printf("%c",'#'); gotoxy(x+1,y); printf("%c",'#'); break;
		case 0:gotoxy(x,y); printf("%c",'#'); gotoxy(x-1,y); printf("%c",'#');  gotoxy(x+1,y); printf("%c",'#'); gotoxy(x,y-1); printf("%c",'#'); gotoxy(x+1,y+1); printf("%c",'#'); gotoxy(x-1,y+1); printf("%c",'#'); 	break;
	}
	
}

void hideTank( int x, int y, int trn)
{
    switch(trn){
		case 3: gotoxy(x,y); printf(" "); gotoxy(x,y-1); printf(" "); gotoxy(x,y+1); printf(" "); gotoxy(x-1,y); printf(" ");  gotoxy(x+1,y-1); printf(" "); gotoxy(x+1,y+1); printf(" ");  break;
		case 2: gotoxy(x,y); printf(" "); gotoxy(x-1,y); printf(" "); gotoxy(x+1,y); printf(" "); gotoxy(x,y+1); printf(" "); gotoxy(x+1,y-1); printf(" "); gotoxy(x-1,y-1); printf(" "); break;
		case 1:gotoxy(x,y); printf(" "); gotoxy(x,y-1); printf(" "); gotoxy(x,y+1); printf(" "); gotoxy(x-1,y+1); printf(" "); gotoxy(x-1,y-1); printf(" "); gotoxy(x+1,y); printf(" "); break;
		case 0:gotoxy(x,y); printf(" "); gotoxy(x-1,y); printf(" ");  gotoxy(x+1,y); printf(" "); gotoxy(x,y-1); printf(" "); gotoxy(x+1,y+1); printf(" "); gotoxy(x-1,y+1); printf(" "); 	break;
	}
	
}


void hitedTank(int x, int y, int trn)
{
	switch(trn){
		case 3: gotoxy(x-2,y+2); printf("#"); gotoxy(x-1,y+1); printf("#"); gotoxy(x-1,y-1); printf("#"); gotoxy(x+1,y+1); printf("#"); gotoxy(x+2,y); printf("#"); gotoxy(x-2,y+2); printf("#"); break;
		case 2: gotoxy(x-2,y); printf("#");gotoxy(x-1,y-2); printf("#"); gotoxy(x,y-1); printf("#"); gotoxy(x,y+2);printf("#"); gotoxy(x+2,y+1); printf("#"); gotoxy(x+2,y-1); printf("#"); break;
		case 1: gotoxy(x-2,y+1); printf("#"); gotoxy(x-1,y-1); printf("#"); gotoxy(x-1,y+2); printf("#"); gotoxy(x+1,y-2); printf("#"); gotoxy(x+1,y+2); printf("#"); gotoxy(x+2,y); printf("#"); break;
		case 0: gotoxy(x-2,y-1); printf("#"); gotoxy(x-1,y-2); printf("#"); gotoxy(x-1,y+1); printf("#"); gotoxy(x,y+2); printf("#"); gotoxy(x+2,y-2); printf("#"); gotoxy(x+2,y+1); printf("#");	break;
		
	}
	
}
void hidehitedTank(int x, int y, int trn)
{
	switch(trn){
		case 3: gotoxy(x-2,y+2); printf(" "); gotoxy(x-1,y+1); printf(" "); gotoxy(x-1,y-1); printf(" "); gotoxy(x+1,y+1); printf(" "); gotoxy(x+2,y); printf(" "); gotoxy(x-2,y+2); printf(" "); break;
		case 2: gotoxy(x-2,y); printf(" ");gotoxy(x-1,y-2); printf(" "); gotoxy(x,y-1); printf(" "); gotoxy(x,y+2);printf(" "); gotoxy(x+2,y+1); printf(" "); gotoxy(x+2,y-1); printf(" "); break;
		case 1: gotoxy(x-2,y+1); printf(" "); gotoxy(x-1,y-1); printf(" "); gotoxy(x-1,y+2); printf(" "); gotoxy(x+1,y-2); printf(" "); gotoxy(x+1,y+2); printf(" "); gotoxy(x+2,y); printf(" "); break;
		case 0: gotoxy(x-2,y-1); printf(" "); gotoxy(x-1,y-2); printf(" "); gotoxy(x-1,y+1); printf(" "); gotoxy(x,y+2); printf(" "); gotoxy(x+2,y-2); printf(" "); gotoxy(x+2,y+1); printf(" ");	break;
		
	}
	
}

void Start()
{
	int i,j1,j2,j3;
    char key; 
    for(i=0;i<=X;i++){
		gotoxy(i,0);
		printf("%c",'#');
		gotoxy(i,60);
		printf("%c",'#');
	}
	
	for(i=1;i<=Y;i++){
		gotoxy(0,i);
		printf("%c",'#');
		gotoxy(115,i);
		printf("%c",'#');
	}
	
	//******T**************
	gotoxy(30,20);
	printf("##########");
	for(i=20;i<=25;i++){
		gotoxy(34,i);
		printf("#");
		gotoxy(35,i);
		printf("#");
	}
	//*****TEnd************
	
	//******A**************
	gotoxy(45,20);
	j1=j2=45;
	for(i=20;i<=25;i++){
		gotoxy(j1,i);
		printf("#");
		gotoxy(j2,i);
		printf("#");
		if(i==23){
		  for(j3=j1;j3<=j2;j3++){
		  	gotoxy(j3,i);
	     	printf("#");
		  }	
		}
	    --j1;
	    ++j2;
		
	}
	//******AEnd***********
	
	//********N*************
	j2=52;
	  for(i=20;i<=25;i++){
	  	    gotoxy(52,i);
		    printf("#");
		    gotoxy(62,i);
		    printf("#");
		    gotoxy(j2,i);
		    printf("#");
		    j2=j2+2;
	 }
	//********NEnd**********
	
	//********K***********
	j1=64;
	j2=70;
	 for(i=20;i<=25;i++){
	 	gotoxy(64,i);
	 	printf("#");
	 	if(i>=22){
	 		gotoxy(j1,i);
	 	    printf("#");
	 	    //gotoxy(72,i);
	 	     //printf("#");
	 	    j1=j1+2;
	 	    
		 } else{
		 	gotoxy(j2,i);
	 	    printf("#");
	 	    //gotoxy(72,i);
	 	     //printf("#");
	 	    j2=j2-2;
		 }
		 
	 }
	 gotoxy(66,22);
	 printf("#");
	//********KEnd********
	
	
	//********S************
	    i=74;
		gotoxy(i,20);
		printf("########");
		gotoxy(i,21);
		printf("#");
		gotoxy(i,22);
		printf("########");
		gotoxy(i,23);
		printf("########");
		gotoxy(i,24);
		printf("       #");
		gotoxy(i,25);
		printf("########");
	
	
	//********SEnd*********
	gotoxy(30,48);
	printf("Objective: Destroy enemy tanks as much as possible.  ");
	gotoxy(30,50);
	printf("Instructions:");
	gotoxy(30,52);
	printf("Press key W to move up, key D to move left, S to move down, A to move left.");
	gotoxy(30,54);
	printf("Press key F to launch a projectile. ");
	gotoxy(30,56);
	printf("Press key E to move to the next stage. ");
	
	do{
	   	gotoxy(50,58);
	    printf("2021");
	   if(kbhit()) key=getch();	
	}while(key!='e');
	system("cls");
}
void End(int hits, int mehits)  //виведення результатів гри
{
  	int i,j1,j2,j3;
	char key;
	
		for(i=0;i<=X;i++){
		gotoxy(i,0);
		printf("%c",'#');
		gotoxy(i,60);
		printf("%c",'#');
	}
	
	for(i=1;i<=Y;i++){
		gotoxy(0,i);
		printf("%c",'#');
		gotoxy(115,i);
		printf("%c",'#');
	}
	
	for(i=40;i<=70;i++){
		gotoxy(i,20);
		printf("%c",'#');
		gotoxy(i,30);
		printf("%c",'#');
	}
	
	for(i=20;i<=30;i++){
		gotoxy(40,i);
		printf("%c",'#');
		gotoxy(70,i);
		printf("%c",'#');
	}
	gotoxy(43,22);
	printf("Results:");
	gotoxy(43,24);
	printf("You have hited %d tanks.", hits);
	gotoxy(43,26);
	printf("You have missed %d tanks.", mehits);
	do{
	   	gotoxy(43,28);
	    printf(" ");
	   if(kbhit()) key=getch();	
	}while(key!='e');
	system("cls");
}
 
 	
 
 void moveBullet(int *fBullet,int *xBullet,int *yBullet, int *oldxBullet,int *oldyBullet, int *dirBullet,int *x,int *y, int *fTank)
 {
    *oldxBullet=*xBullet;
    *oldyBullet=*yBullet;
    switch(*dirBullet){ 
				
    	case 0:*yBullet=*yBullet-BS; break;
    	case 1:*xBullet=*xBullet+BS; break;
    	case 2:*yBullet=*yBullet+BS; break;
    	case 3:*xBullet=*xBullet-BS; break;
	}
	//якщо танк противника на одній координатній вісі зі снарядом, а по другій координатній вісі координати танка потрапляють між попереднім та новим положенням снаряда,
	// то такий танк ми вважаємо підбитим.
	switch(*dirBullet){ //перевіяємо, чи не влучила вона в противника
		case 3: if(((*y-1==*oldyBullet)||(*y+1==*oldyBullet)||(*y==*oldyBullet))&&(*x<*oldxBullet)&&(*x>=*xBullet)) *fTank=0; /*fBullet=0;*/ break;
	    case 2: if(((*x-1==*oldxBullet)||(*x+1==*oldxBullet)||(*x==*oldxBullet))&&(*y>*oldyBullet)&&(*y<=*yBullet)) *fTank=0; /*fBullet=0;*/ break;
		case 1: if(((*y-1==*oldyBullet)||(*y+1==*oldyBullet)||(*y==*oldyBullet))&&(*x>*oldxBullet)&&(*x<=*xBullet)) *fTank=0; /*fBullet=0;*/ break;
		case 0: if(((*x-1==*oldxBullet)||(*x+1==*oldxBullet)||(*x==*oldxBullet))&&(*y<*oldyBullet)&&(*y>=*yBullet)) *fTank=0; /*fBullet=0;*/ break;		
	}
				//
	if((*xBullet<1)||(*xBullet>X-1)||(*yBullet<1)||(*yBullet>Y-1)) *fBullet=0; //якщо куля потрапляє за межі ігрового поля - вона зникає	
 }
 
 void backTurn(int *x,int *y, int *cpx,int *cpy, int *c)
 {
 	    *x=*cpx;
		*y=*cpy;
		switch(*c){
			case 0: *c=2; break;
			case 1: *c=3; break;
			case 2: *c=0; break;
			case 3:	*c=1; break;
		}
 }
 
void avoidCollision(int *x,int *y,int *mx,int *my,int *cpx,int *cpy,int *c)
{
	if  (((abs(*x-*mx)<=3)&&((*y==*my)||(*y==*my+1)||(*y==*my+2)||(*y==*my+3)||(*y==*my-1)||(*y==*my-2)||(*y==*my-3)))||((abs(*y-*my)<=3)&&((*x==*mx)||(*x==*mx+1)||(*x==*mx+2)||(*x==*mx+3)||(*x==*mx-1)||(*x==*mx-2)||(*x==*mx-3)))){ //  запобігаємо наїзду на танк гравця                     //(GamField[x][y]=='#'){              (((abs(x-mx)<=4)&&(y==my))||((abs(y-my)<=4)&&(x==mx)))
			*x=*cpx;
			*y=*cpy;
			*c=abs(3-*c); //псевдовипадково змінюємо напрямок руху танка противника
		}
} 
void enemyShoot(int *c, int *mx, int *my, int *y, int *x, int *fEnemBullet, int *xEnemBullet, int *yEnemBullet, int *dirEnemBullet)
{

        switch(*c){
		  	case 0: if(((*mx-1==*x)||(*mx+1==*x)||(*mx==*x))&&(*my<*y)&&((*y-*my)<=25)) *fEnemBullet=1; break;
			case 1: if(((*my-1==*y)||(*my+1==*y)||(*my==*y))&&(*mx>*x)&&((*mx-*x)<=25)) *fEnemBullet=1; break;
			case 2: if(((*mx-1==*x)||(*mx+1==*x)||(*mx==*x))&&(*my>*y)&&((*my-*y)<=25)) *fEnemBullet=1; break;
			case 3:	if(((*my-1==*y)||(*my+1==*y)||(*my==*y))&&(*mx<*x)&&((*mx-*x)<=25)) *fEnemBullet=1; break;
		}
		*xEnemBullet=*x;
		*yEnemBullet=*y;
		*dirEnemBullet=*c;
}
void renderTank(int *fTank, int *x, int *y, int *cpx, int *cpy, int *c, int *oldc, int *fhit, int *hits)
{
	hideTank(*cpx,*cpy,*oldc); 
	if(*fTank==1) imgTank(*x,*y,*c);
	  		else {
	  			if(*fhit>0) {
	  				hitedTank(*x,*y,*c);
	  			    --*fhit;
				  }
	  	}
		if(*fhit==0) {
			++*hits;    
		    hidehitedTank(*x,*y,*c);
			*fTank=1; 
			*fhit=7;
			*x=3+rand()%105;
			*y=3+rand()%50;
			//c=2;
		} 
}
