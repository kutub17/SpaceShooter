#include<windows.h>
#include<stdio.h>
#include<iostream>
#include<bits/stdc++.h>
#include<GL/glu.h>
#include<Gl/glut.h>

#define SPACEBAR 32
#define MAX_BULLET_ON_SCREEN 999
#define MAX_VELO_BULLET 5

#define bulletSpeed 20
#define FPS 30
#define playerSpeed 30


using namespace std;

int boss_speed=6;
int  bosslife=100;
int currenthighscore;
bool running = true;
int score = 0;
bool gameOver = false;
bool win=false;
bool final_boss = false;

string scoreFile = "D:\\score.txt";



float bgColor = 0;
float bgColorUpperLimit = 0.25f;
bool bgColorOp = 0; // 1 = increase , 0 = decrease
int position = 0; //player position
bool enemyLoaded = false;
int *enemyXpos;
int *enemyType;
int *enemyWidth;
int enemyYpos=350;
int enemySpeed=3;
int bossPos=0;

//collision
int cSIZE=150;
int characterYpos=-300;
GLboolean checkCollision(int x,int SIZE);
GLboolean bulletCollision(int x,int SIZE, int bx, int by);

int level = 1;
int enemyNum = 3;
int aliveEnemyCount;

int playerLife = 5;



void drawRandomStars();
void drawPlayer();
void drawEnemy(int);
void loadEnemies(int);
void new_level();
void boss_fight();
void draw_Boss();


typedef struct
{
    bool active=false;
    bool boss=false;
    double bx;
    double by;

    GLboolean bulletCollision(int x,int SIZE)
    {

        if(!active)
            return false;

        int bSize = 10;

        bool collisionX = x + SIZE >= bx &&
        bx + bSize >= x ;

        bool collisionY = enemyYpos + SIZE >= by &&
        by + bSize >= enemyYpos ;

        if(collisionX && collisionY)
        {
            active=false;
        }

        return collisionX && collisionY;

    }


    //boss bullet to player collision
    GLboolean bulletPlayerCollision()
    {

        if(!active || !boss)
            return false;

        int bSize = 10;

        bool collisionX = position + cSIZE - 80 >= bx &&
        bx + bSize >= position - 80 ;

        bool collisionY = characterYpos >= by &&
        by + bSize >= characterYpos - cSIZE;

        if(collisionX && collisionY)
        {
            active=false;
        }

        return collisionX && collisionY;

    }
}Bullet;


static int shoot=0;
static int boss_shoot=0;
static Bullet bullets[MAX_BULLET_ON_SCREEN];

void bullet(Bullet *b);
void moveBullet();



//For Display TEXT
const int font_large=(int)GLUT_BITMAP_TIMES_ROMAN_24;
const int font_medium=(int)GLUT_BITMAP_HELVETICA_18 ;
const int font_small=(int)GLUT_BITMAP_8_BY_13;


//methods for debugging only
void displayGrid();
void displayPlayerCollider();
void displayEnemyCollider(int);

void renderBitmapString(int x, int y, void *font, char *str)
{
    char *c;
    glRasterPos2d(x, y);
    for (c=str; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}



void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawRandomStars();

    drawPlayer();

    for (int i = 0; i < MAX_BULLET_ON_SCREEN; i++) {
        if (bullets[i].active) {
            bullet(&bullets[i]);
        }
    }


    if(final_boss)
    {
        boss_fight();
        loadEnemies(0);
    }
    else
        loadEnemies(enemyNum);

    //score display
    char score_text[15];
    sprintf (score_text, "SCORE: %d", score);
    glColor3f(1,1,1);
    renderBitmapString(475,425,(void *)font_small,score_text);

    if(final_boss)
    {
        char level_text[10];
        sprintf(level_text, "LEVEL: BOSS");
        glColor3f(1,1,1);
        renderBitmapString(-600,425,(void *)font_small,level_text);

        //boss life
        glPushMatrix();
        //outline
        glTranslated(bossPos-50,0,0);
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
            glVertex2d(0,450);
            glVertex2d(130,450);
            glVertex2d(130,440);
            glVertex2d(0,440);
        glEnd();
        //fill
        glPushMatrix();
        glScalef((float)bosslife/100,1,1);
        glBegin(GL_QUADS);
            glColor3ub(255,0,0);
            glVertex2d(0,450);
            glVertex2d(130,450);
            glVertex2d(130,440);
            glVertex2d(0,440);
        glEnd();
        glPopMatrix();
        glPopMatrix();


        //player Life
        glPushMatrix();
        //outline
        glColor3ub(255,255,255);
        glTranslated(position-50,0,0);
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
            glVertex2d(0,-460);
            glVertex2d(130,-460);
            glVertex2d(130,-450);
            glVertex2d(0,-450);
        glEnd();
        //fill
        glPushMatrix();
        if(gameOver) glScalef(0,1,1);
        else glScalef((float)playerLife*0.2f,1,1);
        glBegin(GL_QUADS);
            glColor3ub(0,255,0);
            glVertex2d(0,-460);
            glVertex2d(130,-460);
            glVertex2d(130,-450);
            glVertex2d(0,-450);
        glEnd();
        glPopMatrix();
        glPopMatrix();
    }
    else
    {
        //displaying level
        char level_text[10];
        sprintf (level_text, "LEVEL: %d", level);
        glColor3f(1,1,1);
        renderBitmapString(-600,425,(void *)font_small,level_text);
    }


    if(gameOver)
    {
        //game over display
        glColor3f(1,1,1);
        renderBitmapString(-100,50,(void *)font_large,"GAME OVER");

        /////high scoree
        ofstream myfile(scoreFile);

        if(myfile.is_open())
        {
            if(score>currenthighscore)
            {
                myfile<<score;
                glColor3f(1,1,1);
                renderBitmapString(-100,150,(void *)font_small,"New High score!");
            }
            else
                myfile<<currenthighscore;

        }
        else cout<<"Unable to open Score File"<<endl;
    }

    if(win)
    {
        //game over display
        char youWin[10];
        sprintf (youWin, "YOU WIN!");
        glColor3f(1,1,1);
        renderBitmapString(-80,0,(void *)font_large,youWin);
    }


    //for debugging
        //displayGrid();
        //displayPlayerCollider();
        //displayEnemyCollider(enemyNum);


    glutSwapBuffers();
}

void bullet( Bullet *b ){
    if(b->boss)
        glColor3ub(204,255,255);
    else
        glColor3ub(255,255,0);
    glPushMatrix();
    glTranslated(b->bx, b->by, 0.0);
    glBegin(GL_POLYGON);
    glVertex2i(10,10);
    glVertex2i(15,12);
    glVertex2i(20,10);
    glVertex2i(15,15);
    glVertex2i(20,30);
    glVertex2i(15,40);
    glVertex2i(10,30);
    glVertex2i(12,15);
    glEnd();
    glPopMatrix();
}


void loadEnemies(int num)
{
    if(!enemyLoaded)
    {
        aliveEnemyCount = num;

        enemyXpos = new int [num];
        enemyType = new int [num];
        enemyWidth = new int [num];

        for(int i=0; i<num; i++)
        {
            //getting the random position
            int r = (rand()%640) * ((-1) + 2*(rand()%2));
            if(r>500) r = 500-(rand()%9);
            enemyXpos[i] = r;

            //getting the random type (out of 3)
            r = rand()%3;
            enemyType[i] = r;
        }
        enemyLoaded = true;
    }


    for(int i=0; i<num; i++)
        drawEnemy(i); //draw the i-th enemy

}

void drawPlayer()
{
    glPushMatrix();
    glTranslatef(position,-50, 0);
    glBegin(GL_POLYGON);
    glColor3ub(0,204,204);
        glVertex2d(-5,-230);
        glVertex2d(-5,-250);
        glVertex2d(5,-250);
        glVertex2d(5,-230);
    glEnd();


    //glBegin(GL_FILL);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);

     glColor3ub(255,0,0);
        glVertex2d(-5,-250);
        glVertex2d(-5,-265);
        glVertex2d(-15,-265);
        glVertex2d(-15,-315);
        glVertex2d(-25,-315);
        glVertex2d(-25,-340);

        glVertex2d(-35,-340);
        glVertex2d(-35,-345);

        glVertex2d(-45,-345);
        glVertex2d(-45,-350);

        glVertex2d(-50,-350);
        glVertex2d(-50,-355);

        //glVertex2d(-55,-355);
        //glVertex2d(-55,-335);
        //glVertex2d(-60,-335);
        //glVertex2d(-60,-360);
        // glVertex2d(-60,-360); Nil COLOR

        glVertex2d(-65,-360);
        glVertex2d(-65,-365);

        glVertex2d(-70,-365);
        glVertex2d(-70,-370);

        glVertex2d(-75,-370);
        glVertex2d(-75,-375);

        glVertex2d(-60,-375);
        glVertex2d(-60,-380);
        glVertex2d(-55,-380);
        glVertex2d(-55,-375);
        glVertex2d(-50,-375);
        glVertex2d(-50,-370);
        //glVertex2d(-50,-370); LAL COLOR
        glVertex2d(-45,-365);
        glVertex2d(-45,-360);
        glVertex2d(-35,-360);
        glVertex2d(-35,-355);
        glVertex2d(-30,-355);
        glVertex2d(-27,-365);
        glVertex2d(-15,-365);
        glVertex2d(-5,-385);

        //reverse
        glVertex2d(5,-385);
        glVertex2d(15,-365);
        glVertex2d(27,-365);
        glVertex2d(30,-355);
        glVertex2d(35,-355);
        glVertex2d(35,-360);
        glVertex2d(45,-360);
        glVertex2d(45,-365);

        glVertex2d(50,-370);
        glVertex2d(50,-375);
        glVertex2d(55,-375);
        glVertex2d(55,-380);
        glVertex2d(60,-380);
        glVertex2d(60,-375);

        glVertex2d(75,-375);
        glVertex2d(75,-370);

        glVertex2d(70,-370);
        glVertex2d(70,-365);

        glVertex2d(65,-365);
        glVertex2d(65,-360);
        //glVertex2d(60,-360);
        //glVertex2d(60,-335);
        //glVertex2d(55,-335);
        //glVertex2d(55,-355);
        glVertex2d(50,-355);
        glVertex2d(50,-350);
        glVertex2d(45,-350);
        glVertex2d(45,-345);
        glVertex2d(35,-345);
        glVertex2d(35,-340);
        glVertex2d(25,-340);
        glVertex2d(25,-315);
        glVertex2d(15,-315);
        glVertex2d(15,-265);
        glVertex2d(5,-265);
        glVertex2d(5,-250);
    glEnd();



    glBegin(GL_POLYGON);
    glColor3ub(255,0,0);

      glVertex2d(0,-290);
      glVertex2d(-20,-355);
      glVertex2d(20,-355);

    glEnd();


    glBegin(GL_POLYGON);
    glColor3ub(255,0,0);
        glVertex2d(-55,-355);
        glVertex2d(-55,-335);
        glVertex2d(-60,-335);
        glVertex2d(-60,-360);
     glEnd();



     glBegin(GL_POLYGON);
    glColor3ub(255,0,0);
        glVertex2d(55,-355);
        glVertex2d(55,-335);
        glVertex2d(60,-335);
        glVertex2d(60,-360);
     glEnd();
     glPopMatrix();
}


void drawEnemy(int i)
{
    if(enemyType[i]==0){
        //setup the width
        enemyWidth[i] = 100;

        //actual drawing
        glPushMatrix();
        glTranslated(enemyXpos[i]-270, enemyYpos, 0);
         glBegin(GL_QUADS);
          glColor3ub(0,255,255);
          glVertex2i(300,0);
          glVertex2i(350,0);
          glColor3ub(0,180,255);
          glVertex2i(350,80);
          glVertex2i(300,80);
          glEnd();



          glBegin(GL_QUADS);
          glColor3ub(255,255,0);
          glVertex2i(270,50);
          glVertex2i(270,35);
          glColor3ub(255,50,50);
          glVertex2i(300,40);
          glVertex2i(300,65);
          glEnd();



         glBegin(GL_QUADS);
         glColor3ub(255,255,0);
         glVertex2i(350,40);
         glVertex2i(380,35);
         glColor3ub(255,50,50);
         glVertex2i(380,50);
         glVertex2i(350,65);
         glEnd();



        glBegin(GL_QUADS);
        glColor3ub(255,255,0);
        glVertex2i(280,20);
        glVertex2i(280,10);
        glColor3ub(255,50,50);
        glVertex2i(300,15);
        glVertex2i(300,30);
        glEnd();


        glBegin(GL_QUADS);
        glColor3ub(255,255,0);
        glVertex2i(350,15);
        glVertex2i(370,10);
        glColor3ub(255,50,50);
        glVertex2i(370,20);
        glVertex2i(350,30);
        glEnd();


        glBegin(GL_TRIANGLES);
        glColor3ub(255,0,0);
        glVertex2i(300,80);
        glVertex2i(310,80);
        glVertex2i(300,90);
        glEnd();

        glColor3ub(255,0,0);
        glBegin(GL_TRIANGLES);
        glVertex2i(350,80);
        glVertex2i(350,90);
        glVertex2i(340,80);
        glEnd();

        glColor3ub(255,0,0);
        glBegin(GL_TRIANGLES);
        glVertex2i(315,80);
        glVertex2i(335,80);
        glVertex2i(325,100);
        glEnd();

        glColor3ub(255,0,0);
        glBegin(GL_QUADS);
        glVertex2i(310,0);
        glVertex2i(310,-10);
        glVertex2i(322,-10);
        glVertex2i(322,0);
        glEnd();

        glColor3ub(255,0,0);
        glBegin(GL_QUADS);
        glVertex2i(328,0);
        glVertex2i(328,-10);
        glVertex2i(340,-10);
        glVertex2i(340,0);
        glEnd();
        glPopMatrix();
    }

    if(enemyType[i]==1){
        //setting up the width
        enemyWidth[i] = 180;

        glPushMatrix();
        glRotated(180,0,0,1);
        glTranslated(-enemyXpos[i] ,(enemyYpos*(-1))-160, 0);
        int tx = 280;
        glBegin(GL_QUADS);
          glColor3ub(0,255,255);
          glVertex2i(-400+tx,0);
          glVertex2i(-340+tx,0);
          glColor3ub(0,180,255);
          glVertex2i(-340+tx,150);
          glVertex2i(-400+tx,150);
          glEnd();


          glBegin(GL_QUADS);
          glColor3ub(255,255,0);
          glVertex2i(-460+tx,50);
          glVertex2i(-400+tx,65);
          glColor3ub(255,64,0);
          glVertex2i(-400+tx,100);
           glVertex2i(-460+tx,70);
          glEnd();


          glBegin(GL_QUADS);
          glColor3ub(255,255,0);
          glVertex2i(-340+tx,65);
          glVertex2i(-280+tx,50);
          glColor3ub(255,64,0);
          glVertex2i(-280+tx,70);
          glVertex2i(-340+tx,100);
          glEnd();


          glBegin(GL_QUADS);
          glColor3ub(255,255,0);
          glVertex2i(-440+tx,10);
          glVertex2i(-400+tx,15);
          glColor3ub(255,64,0);
          glVertex2i(-400+tx,45);
          glVertex2i(-440+tx,25);
          glEnd();


          glBegin(GL_QUADS);
          glColor3ub(255,255,0);
          glVertex2i(-340+tx,15);
          glVertex2i(-300+tx,10);
          glColor3ub(255,64,0);
          glVertex2i(-300+tx,25);
          glVertex2i(-340+tx,45);
          glEnd();

          glColor3ub(255,0,0);
          glBegin(GL_QUADS);
          glVertex2i(-400+tx,0);
          glVertex2i(-385+tx,-20);
          glVertex2i(-355+tx,-20);
          glVertex2i(-340+tx,0);
          glEnd();


        glBegin(GL_TRIANGLES);
        glColor3ub(255,70,0);
        glVertex2i(-385+tx,150);
        glVertex2i(-355+tx,150);
        glColor3ub(255+tx,0,0);
        glVertex2i(-370+tx,220);
        glEnd();
        glPopMatrix();
    }

    if(enemyType[i]==2){
        //setting up the width
        enemyWidth[i] = 180;

        glPushMatrix();
        glRotated(180,0,0,1);
        //glScaled(0.75,0.75,1);
        int tx2 = -50;
        glTranslated(-enemyXpos[i], (enemyYpos*(-1))-60, 0);

        glBegin(GL_QUADS);
          glColor3ub(0,255,255);
          glVertex2i(-100+tx2,-70);
          glVertex2i(20+tx2,-70);
          glColor3ub(0,180,255);
          glVertex2i(20+tx2,60);
          glVertex2i(-100+tx2,60);
          glEnd();



        glBegin(GL_TRIANGLES);
        glColor3ub(255,0,0);
        glVertex2i(-40+tx2,100);
        glColor3ub(255,180,0);
        glVertex2i(-80+tx2,60);
        glVertex2i(0+tx2,60);
        glEnd();


          glBegin(GL_QUADS);
          glColor3ub(0,0,255);
          glVertex2i(-140+tx2,-70);
          glVertex2i(-100+tx2,-70);
          glColor3ub(250,40,100);
          glVertex2i(-100+tx2,0);
          glVertex2i(-140+tx2,-40);
          glEnd();


          glBegin(GL_QUADS);
          glColor3ub(0,0,255);
          glVertex2i(20+tx2,-70);
          glVertex2i(60+tx2,-70);
          glColor3ub(250,40,100);
          glVertex2i(60+tx2,-40);
          glVertex2i(20+tx2,0);
          glEnd();


          glBegin(GL_QUADS);
          glColor3ub(0,0,255);
          glVertex2i(-80+tx2,-100);
          glVertex2i(0+tx2,-100);
          glColor3ub(250,40,100);
          glVertex2i(0+tx2,-70);
          glVertex2i(-80+tx2,-70);
          glEnd();



          glBegin(GL_QUADS);
          glColor3ub(255,255,0);
          glVertex2i(-60+tx2,-100);
          glVertex2i(-60+tx2,-120);
          glColor3ub(255,0,0);
          glVertex2i(-45+tx2,-120);
          glVertex2i(-45+tx2,-100);
          glEnd();


          glBegin(GL_QUADS);
          glColor3ub(255,255,0);
          glVertex2i(-40+tx2,-100);
          glVertex2i(-40+tx2,-120);
          glColor3ub(255,0,0);
          glVertex2i(-25+tx2,-120);
          glVertex2i(-25+tx2,-100);
          glEnd();
          glPopMatrix();
    }
}


void bgUpdate(int value)
{
    float changeRate = 0.02f;

    if(!running)
        return;

    if(bgColor<=0 || bgColor>=bgColorUpperLimit)
        bgColorOp = !bgColorOp;

    if(bgColorOp) // true === 1
        bgColor += changeRate;
    else
        bgColor -= changeRate;

    glClearColor(bgColor,bgColor,bgColor,0);

	glutPostRedisplay();
}

//all game logics here
void gameUpdate(int value)
{
    if(!running)
        return;

    //updating enemy position
    enemyYpos -= enemySpeed;

    moveBullet();

    //enemy coll check
    for(int i=0; i<enemyNum; i++)
    {
        if(checkCollision(enemyXpos[i],enemyWidth[i]))
        {
            running=false;
            gameOver = true;
        }

        for(int j=0; j<MAX_BULLET_ON_SCREEN; j++)
        {
            if(bullets[j].active && bullets[j].bulletCollision(enemyXpos[i],enemyWidth[i]))
            {
                enemyXpos[i] = 1000;
                score++;
                aliveEnemyCount--;
            }
        }
    }

    //boss coll check
    if(final_boss)
    {
        for(int j=0; j<MAX_BULLET_ON_SCREEN; j++)
        {
            if(bullets[j].active && bullets[j].bulletCollision(bossPos-130,260))
            {
                //cout<<"boss coll"<<endl;

                if(bosslife>0)
                {
                    bosslife-=20;
                    boss_speed+=3;
                    cout<<bosslife<<endl;
                }
                else
                {
                    bossPos=1000;
                    running=false;
                    gameOver=true;
                    win=true;
                }
            }
        }

        for(int j=0; j<MAX_BULLET_ON_SCREEN; j++)
        {
            if(bullets[j].active && bullets[j].bulletPlayerCollision())
            {
                //cout<<"player got shot"<<endl;
                playerLife--;

                if(playerLife<=0)
                    gameOver=true;
            }
        }
    }

    if(enemyYpos<-640)
    {
        new_level();
        score -= aliveEnemyCount; // -1 if an enemy is missed
    }

    if(aliveEnemyCount<=0)
        new_level();

    glutPostRedisplay();
	glutTimerFunc(1000/FPS, gameUpdate, 0);

}

void new_level()
{
    if(level>=4 && score>=20)
    {
        final_boss = true;
        level=0;
        enemyNum = 0;
        return;
    }

    //cout<<enemySpeed<<endl;
    level++;
    enemyLoaded = false;
    enemyNum = 3+level;
    enemyYpos = 350;
    if(level%3==0)
        enemySpeed++;
}

bool leftt=false;
bool rightt=true;

void boss_fight()
{
    if(gameOver)
        return;

    //movement
    if(rightt==true)
    {
        bossPos+=boss_speed;
        if(bossPos>=600)
        {
            rightt=false;
            leftt=true;
        }
    }

    if(leftt==true)
    {
        bossPos-=boss_speed;
        if(bossPos<=-600)
        {
            rightt=true;
            leftt=false;
        }
    }


    //boss shoot
    boss_shoot=1;

    draw_Boss();
}

void draw_Boss()
{
    glPushMatrix();
    glTranslatef(bossPos,0,0);
    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(0,380);
    glVertex2i(-50,380);
    glVertex2i(-50,400);
    glVertex2i(-70,400);
    glVertex2i(-70,420);
    glVertex2i(-90,420);
    glVertex2i(-90,400);
    glVertex2i(-70,400);
    glVertex2i(-70,380);
    glVertex2i(-90,380);
    glVertex2i(-90,360);
    glVertex2i(-110,360);
    glVertex2i(-110,340);
    glVertex2i(-130,340);
    glVertex2i(-130,280);
    glVertex2i(-110,280);
    glVertex2i(-110,320);
    glVertex2i(-90,320);
    glVertex2i(-90,280);
    glVertex2i(-70,280);
    glVertex2i(-70,260);
    glVertex2i(-20,260);
    glVertex2i(-20,280);
    glVertex2i(-70,280);
    glVertex2i(-70,300);
    glVertex2i(0,300);
    glVertex2i(70,300);
    glVertex2i(70,280);
    glVertex2i(20,280);
    glVertex2i(20,260);
    glVertex2i(70,260);
    glVertex2i(70,280);
    glVertex2i(90,280);
    glVertex2i(90,320);
    glVertex2i(110,320);
    glVertex2i(110,280);
    glVertex2i(130,280);
    glVertex2i(130,340);
    glVertex2i(110,340);
    glVertex2i(110,360);
    glVertex2i(90,360);
    glVertex2i(90,380);
    glVertex2i(70,380);
    glVertex2i(70,400);
    glVertex2i(90,400);
    glVertex2i(90,420);
    glVertex2i(70,420);
    glVertex2i(70,400);
    glVertex2i(50,400);
    glVertex2i(50,380);
    glVertex2i(0,380);
    glEnd();


    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_QUADS);
    glVertex2i(-50,360);
    glVertex2i(-50,340);
    glVertex2i(-30,340);
    glVertex2i(-30,360);
    glEnd();

    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_QUADS);
    glVertex2i(50,360);
    glVertex2i(50,340);
    glVertex2i(30,340);
    glVertex2i(30,360);
    glEnd();

    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_QUADS);
    glVertex2i(-70,400);
    glVertex2i(-70,420);
    glVertex2i(-90,420);
    glVertex2i(-90,400);

    glEnd();


    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_QUADS);
    glVertex2i(70,400);
    glVertex2i(70,420);
    glVertex2i(90,420);
    glVertex2i(90,400);

    glEnd();

    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_QUADS);
    glVertex2i(-70,280);
    glVertex2i(-70,260);
    glVertex2i(-20,260);
    glVertex2i(-20,280);
    glVertex2i(-70,280);


    glEnd();

    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_QUADS);
    glVertex2i(70,280);
    glVertex2i(70,260);
    glVertex2i(20,260);
    glVertex2i(20,280);
    glVertex2i(70,280);


    glEnd();

    glColor3ub(255,0,0);
    glPointSize(4.0);
    glBegin(GL_LINES);
    glVertex2i(-30,320);
    glVertex2i(30,320);

    glEnd();
    glPopMatrix();
}


void moveBullet()
{
    int i=0;

    if(shoot==1)
    {
        for(int i=0;i< MAX_BULLET_ON_SCREEN;i++)
        {
            if(bullets[i].active==false)
            {
                bullets[i].active=true;
                bullets[i].bx=position;
                bullets[i].by=-250;
                break;
            }
        }

        shoot=0;
    }

    if(boss_shoot==1)
    {
        for(int i=0;i<3;i++)
        {
            if(bullets[i].active==false)
            {
                bullets[i].active=true;
                bullets[i].boss=true;
                bullets[i].bx=bossPos; //+((rand()%50) * ((-1) + 2*(rand()%2)));
                bullets[i].by=250;
                break;
            }
        }

        boss_shoot=0;
    }

    for(int i=0; i<MAX_BULLET_ON_SCREEN; i++)
    {
        //bullets[i].by+=50;
        if(bullets[i].boss)
            bullets[i].by -= bulletSpeed; //bullet speed
        else
            bullets[i].by += bulletSpeed; //bullet speed

        if(bullets[i].by>480 || bullets[i].by<-480)
            bullets[i].active=false;
    }
}


void handleKeypress(unsigned char key, int x, int y) {

	if(!running || gameOver)
        return;

	switch (key) {
        case 'a':
            if(position > -550)
            position-=playerSpeed;
            break;
        case 'd':
            if(position < 560)
            position+=playerSpeed;
            break;
        case 32:
            shoot=1;
            break;
	}

	glutPostRedisplay();
}


void drawRandomStars()
{
    int maxNum = rand()%50 + 25;
    for(int i=0; i<maxNum; i++)
    {
        int qx = (-1) + 2*(rand()%2); //determines x will be positive or negative
        int qy = (-1) + 2*(rand()%2); //determines y will be positive or negative

        int x = rand()%640 * qx;
        int y = rand()%480 * qy;

        glPointSize(rand()%4);
        glBegin(GL_POINTS);
            glColor3ub(255,255,255);
            glVertex2d(x,y);
        glEnd();
    }
}


GLboolean checkCollision(int x,int SIZE)
{
    bool collisionX = x + SIZE >= position-80 &&
    position +cSIZE - 80 >= x ;

    bool collisionY = enemyYpos + SIZE >= characterYpos - cSIZE &&
    characterYpos >= enemyYpos ;

    return collisionX && collisionY;
}


void myInit(){
    glClearColor(bgColor,bgColor,bgColor,0);
    glColor3f(1,1,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-640,640,-480,480);
}


int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(640,480);
    glutInitWindowPosition(100,150);
    glutCreateWindow("Space Shooter");

    srand(time(0));
    //high  score
    ifstream myfile(scoreFile);
    string s;

    if(myfile.is_open())
    {
       while(getline(myfile,s))
       {
        stringstream geek(s);
        geek>>currenthighscore;
       }
    }
    else cout<<"Unable to open"<<endl;

    cout<<"High Score: "<<currenthighscore<<endl;





    glutDisplayFunc(display);
    myInit();

    glutKeyboardFunc(handleKeypress);
    glutTimerFunc(100, bgUpdate, 0);
    glutTimerFunc(100, gameUpdate, 0);

    glutMainLoop();
}







//// Some Methods written for debugging only
void displayGrid()
{
    //grid - x
    for(int i=0; i<=13; i++)
    {
        glColor3ub(128,128,128);
        glLineWidth(1);
        glBegin(GL_LINES);
            glVertex2d(i*50 , 480);
            glVertex2d(i*50 , -480);

            glVertex2d(i*-50 , 480);
            glVertex2d(i*-50 , -480);
        glEnd();
    }
    //grid - y
    for(int i=0; i<=10; i++)
    {
        glLineWidth(1.0f);
        glBegin(GL_LINES);
            glVertex2d(640 , i*50);
            glVertex2d(-640 , i*50);

            glVertex2d(640 , i*-50);
            glVertex2d(-640 , i*-50);
        glEnd();
    }

    //axis
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2d(0 , 480);
        glVertex2d(0, -480);

        glVertex2d(640, 0);
        glVertex2d(-640, 0);
    glEnd();
}

void displayPlayerCollider(){
        glBegin(GL_LINE_LOOP);
            glVertex2d(position-80,characterYpos-cSIZE);
            glVertex2d(position+cSIZE-80,characterYpos-cSIZE);
            glVertex2d(position+cSIZE-80,characterYpos);
            glVertex2d(position-80,characterYpos);
        glEnd();
}

void displayEnemyCollider(int num){
    for(int i=0; i<num; i++)
    {
        //drawing obj1 (enemy)
        glBegin(GL_LINE_LOOP);
            glVertex2d(enemyXpos[i],enemyYpos);
            glVertex2d(enemyXpos[i]+enemyWidth[i],enemyYpos);
            glVertex2d(enemyXpos[i]+enemyWidth[i],enemyYpos+enemyWidth[i]);
            glVertex2d(enemyXpos[i],enemyYpos+enemyWidth[i]);
        glEnd();
    }
}
