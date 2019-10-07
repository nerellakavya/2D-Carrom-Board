#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <GL/glut.h>
#include "defs.h"

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define num_coins 7

using namespace std;

Box Out_box, In_box, inner;
Circle inner_circle, holes[4], redCircles[4];
Striker striker;
Coin coins[num_coins];
ScoreDisplay score_display;

int score = 30;
int mouseButton= -1;
int windowWidth;
int windowHeight;
int count = 0;
int coincheck = 0;
float box_len_out = 5.4f;
float box_len_in = 5.0f;

void initRendering() 
{
    glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
    glEnable(GL_COLOR_MATERIAL);    // Enable coloring
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Setting a background color
}

void handleResize(int w, int h) 
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void handleKeypress2(int key, int x, int y) 
{
    if (striker.state == 1)
        return ;
    float temp;
    float side = inner.getSide();
    if (key == GLUT_KEY_UP)
    {
        striker.line.updateLength(0.05f);
        return ;
    }
    if (key == GLUT_KEY_DOWN)
    {
        striker.line.updateLength(-0.05f);
        return ;
    }
    if (key == GLUT_KEY_LEFT)
    {
        temp = striker.x - 0.02f;
    }
    if (key == GLUT_KEY_RIGHT)
    {
        temp = striker.x + 0.02f;
    }
    if ((temp >= -side/2 + striker.r*3/4)&&(temp <= side/2 - striker.r*3/4))
    {
        striker.set_coordinates(temp, striker.y);
        striker.line.set_coordinates1(striker.x,striker.y);
        
    }

}

void handleKeypress1(unsigned char key, int x, int y) 
{
    
    if (key == 27) {
        exit(0);     // escape key is pressed
    }
    if (striker.state == 1)
        return ;
    if ((key == 'a')||(key == 'A'))
    {
        striker.line.updateAngle(-5.0);
    }
    if ((key == 'c')||(key == 'C'))
    {
        striker.line.updateAngle(5.0);
    }
    if (key == 32) //spacebar
    {
        striker.startMove();
    }
}

void default_board() // function to set all components variables (like radius , position etc), after drawscene , it appears on the screen
{
    Color colour_corner_holes, colour_inner_circle;

    Out_box.set_variables(box_len_out,1); 
    Out_box.col.setColor(0.647f, 0.164f, 0.164f);

    In_box.set_variables(box_len_in,1);
    In_box.col.setColor(1.0f, 0.9f, 0.8f);

    inner.set_variables(box_len_in *5/6,0);
    inner.col.setColor(1.0f, 0.0f, 0.0f);

    colour_corner_holes.setColor(0.8f, 0.8f, 0.8f);
    setCircles(0.18f, In_box, holes, 1, colour_corner_holes);

    colour_inner_circle.setColor(1.0f, 0.0f, 0.0f);
    setCircles(0.12f, inner, redCircles, 0, colour_inner_circle);

    float side = inner.getSide();
    striker.setRad(0.16f);
    striker.resetStriker(side);
    striker.col.setColor(0.0f, 0.0f, 0.596f);

    inner_circle.col.setColor(0.75f, 0.45f, 0.45f);
    inner_circle.setFill(0);
    inner_circle.setRad(0.5f);

    arrange_coins(); //coins in game

    striker.line.set_coordinates1(striker.x,striker.y);
}

void setCircles(float rad, Box square, Circle hole[4], int fill, Color c)
{
    float side = square.getSide();
    hole[0].set_coordinates(-side/2 + rad, -side/2 + rad);
    hole[1].set_coordinates(-side/2 + rad, side/2 - rad);
    hole[2].set_coordinates(side/2 - rad, -side/2 + rad);
    hole[3].set_coordinates(side/2 - rad, side/2 - rad);
    for(int i=0;i<4;i++)
    {
        hole[i].setRad(rad);
        hole[i].setFill(fill);
        hole[i].col.setColor(c.red, c.green, c.blue);
    }
}

void arrange_coins()
{ 
    // 0th is red coin and all others white and black
    int i;
    coins[0].type = 2;
    coins[0].col.setColor(1.0f, 0.0f, 0.0f);
    float angle = 360.0 / (num_coins - 1);
    float arc = inner_circle.r*6/10;
    float x, y;
    for(i = 1;i < num_coins ; i++)
    {
        coins[i].type = i % 2;
        if (i % 2 == 1)
            coins[i].col.setColor(1.0f, 1.0f, 1.0f);
        else
            coins[i].col.setColor(0.0f, 0.0f, 0.0f);

        x = arc * cos(DEG2RAD((float)(i-1) * angle));
        y = arc * sin(DEG2RAD((float)(i-1) * angle));

        coins[i].set_coordinates(x,y);
        coins[i].setOriginalCood(x,y);
    }

}

void drawScene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -8.0f); 
	
    // Actual drawing

    //Boxes
    Out_box.drawBox();
    In_box.drawBox();
    inner.drawBox();

    //corner circles   
    for(int i=0;i<4;i++)
        holes[i].drawCircle();           //make will call draw with previously set raidus

    //red bordered circles
    for(int i=0;i<4;i++)
        redCircles[i].drawCircle();
     
    //circle around coins
    inner_circle.drawCircle();
    //striker circle
    striker.drawCircle();

    // All coins with black borders
    Circle dummy; //just for black border 
    for(int i = 0; i<num_coins; i++)
    {   
        dummy.set_coordinates(coins[i].x , coins[i].y);
        dummy.setRad(coins[i].r+0.010f);
        dummy.setFill(1);
        dummy.drawCircle();
        coins[i].drawCircle();
    }
	
    //striker's line representing velocity and direction
    striker.line.drawLine(striker.x,striker.y);

    //display score
    score_display.printScore(score);

    glPopMatrix();
    glutSwapBuffers(); 
}

//striker coin collisions
void update1(int value) 
{
    striker.updateProperties(In_box,inner,coincheck);
    int check;
    for(int i=0;i< num_coins;i++)
    {
        check = striker.checkCollision(coins[i]);
        if (check == 1)
        {
            striker.collisions(&(coins[i]));
            coincheck = 1;       
        }
    }

    glutTimerFunc(10, update1, 0);
}

// coins coins colisions
void update2(int value)
{
    int check;
    int count1 = 0;
    for(int i=0;i<num_coins;i++)
    {
        for (int j=i+1;j<num_coins;j++)
        {
            check = coins[i].checkCollision(coins[j]);
            if (check == 1)
            {
                coins[i].collisions(&(coins[j]));
                coincheck = 1;
            }

        }
        coins[i].updateProperties(In_box,&count1);
        float vx = coins[i].vx;
        float vy = coins[i].vy;

    }
    if (count1 == num_coins)
    {
        coincheck = 0;
    }
    glutTimerFunc(10, update2, 0);
}

//Coins and striker in holes
void update3(int value)
{
    int check;
    for(int i=0;i<num_coins;i++)
    {
        for (int j=0; j<4;j++)
        {
            check = coins[i].checkHole(holes[j]);
            if (check == 1)
            {
                coins[i].enterHole(&score);
                count ++; 
                if (count == num_coins)
			score_display.s = strdup("Final Score");            
            }
        }
    }
    for (int j=0; j<4;j++)
    {
        check = striker.checkHole(holes[j]);
            if ((check == 1))
            {
                striker.resetStriker(inner.getSide());
                break;
            }
    }
    glutTimerFunc(10, update3, 0);
}

void timer(int value)
{   
    if(count<num_coins)
    score--;
    glutTimerFunc(1000, timer, 0);
}

int main(int argc, char **argv) {

    default_board();

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = w * 2 / 3;
    windowHeight = h * 2 / 3;

    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

    glutCreateWindow("Carroms");  // Setup the window
    initRendering();

    // Register callbacks
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene); //if no event continue drawing the scene
    glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleKeypress2);
    glutReshapeFunc(handleResize);
    glutTimerFunc(10, update1, 0); //striker coin collions 
    glutTimerFunc(10, update2, 0); //coin coin collisions
    glutTimerFunc(10, update3, 0); //coin and striker hole checking
    glutTimerFunc(1000, timer, 0); //every second costs a 1 point score
    glutMainLoop();
    return 0;
}
