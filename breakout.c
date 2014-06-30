//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    //speed of ball at start
    float yspeed = .035;
    float xspeed = (drand48() - .5) /5;
    
    //starting score and display
    int score = 0;
    updateScoreboard(window, label, score);
    
    // keep playing until game over
    waitForClick();
    while (lives > 0 && bricks > 0)
    {
        //this section moves the paddle with the mouse
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        if (event !=  NULL)             
        {
            if (getEventType(event) == MOUSE_MOVED)
            {
                double x = getX(event) - 30;
                setLocation(paddle, x, 550);
            }
        }
        
        move(ball,xspeed, yspeed);
        
        //reverses x direction when it hits side of window
        if (getX(ball) + getWidth(ball) >= WIDTH)  
            xspeed = -xspeed;
        
        else if (getX(ball) <= 0)
            xspeed = -xspeed;
        
        //reverses y direction when it hits top of window    
        if (getY(ball) <= 0)   
            yspeed = -yspeed;  
        
        //
        if ((getY(ball) + RADIUS * 2) >= HEIGHT)
        {
            removeGWindow(window, ball);
            ball = initBall(window);
            lives -= 1;
            waitForClick();
        }
        
        //determines if an object is hit
        GObject object = detectCollision(window, ball);
        
        if (object != NULL)
        {   
            if(object == paddle)
                yspeed = - yspeed;
            
            else if (strcmp(getType(object), "GRect") == 0)
            {
                removeGWindow(window, object);  //removes block
                yspeed = -yspeed;
                score += 1;
                bricks -=1;
            }
        }
        
        updateScoreboard(window, label, score);
            
         
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    int startx = 2;  //location of first block
    
    
    for (int i = 0; i < COLS; i++)
    {
        int starty = 50;
        for (int j = 0; j < ROWS; j++)
        {
            GRect target = newGRect(startx, starty, 36, 12);
            if (j == 0)
                setColor(target, "RED");   //picking color for each row
            
            if (j == 1)
                setColor(target, "BLUE");
                
            if (j == 2)
                setColor(target, "BLACK");
            
            if (j == 3)
                setColor(target, "GREEN");
                
            if (j == 4)
                setColor(target, "YELLOW");
                
            setFilled(target, true);
            add(window, target);
            starty += 16;                                
        }
        startx += 40;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval circle = newGOval(WIDTH/2 - RADIUS, HEIGHT/2 - RADIUS, RADIUS * 2, RADIUS *2);
    setColor(circle, "BLACK");
    setFilled(circle, true);
    add(window, circle);
    return circle;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(170, 550, 60, 10);
    setColor(paddle, "BLUE");
    setFilled(paddle, true);
    add(window, paddle);
    
    return paddle;
    
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel Label = newGLabel("0");
    int x = (WIDTH - getWidth(Label))/2;
    int y = (HEIGHT - getHeight(Label))/2;
    setFont(Label, "SansSerif-37");
    setColor(Label, "Black");
    setLocation(Label, x, y);
    add(window, Label);
    return Label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
