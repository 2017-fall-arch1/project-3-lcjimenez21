/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "buzzer.h"
#include "movementPads.h"

#define GREEN_LED BIT6
static char P1 ='0';
static char P2 ='0';
int swSD1, swSD2, swSD3, swSD4;
int colorBall , colorScoreBoard, bg;
Region player1Fence;
Region player2Fence;

AbRect rect10 = {abRectGetBounds, abRectCheck, {1,13}}; /**< 1x13 rectangle */

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 3, screenHeight/2 - 3}
};




Layer gameBall = {		/**< Layer for the gameBall */
  (AbShape *)&circle2,
  {(screenWidth/2)+10, (screenHeight/2)+5}, 
  {0,0}, {0,0},				    
  COLOR_GREEN,
  0,
};


Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    
  COLOR_WHITE,
  &gameBall
};


Layer player2 = {		/**< Layer of paddle right */
  //(AbShape *)&layer1Outline,
  (AbShape *)&rect10,
  {(screenWidth/2)-57, (screenHeight/2)+10}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};

Layer player1 = {		/**< Layer of paddle left */
  (AbShape *)&rect10,
  {(screenWidth/2)+57, (screenHeight/2)-10}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &player2,
};


/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */

MovLayer ml3 = { &gameBall, {2,1}, 0 };

MovLayer uPly2 = {&player2, {0,-3}, 0};
MovLayer dPly2 = {&player2, {0,3}, 0};

MovLayer uPly1 = {&player1, {0,-3}, 0};
MovLayer dPly1 = {&player1, {0,3}, 0};

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    
    for (axis = 0; axis < 2; axis ++) {
      if(shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]){
        newPos.axes[axis] += 3;
      }	/**< if outside of fence */
      
      if(shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]){
        newPos.axes[axis] -= 3;
      }
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}

void mlAdvanceNew(MovLayer *ml, MovLayer *mlP1, MovLayer *mlP2,  Region *fence)
{
  Vec2 newPos;
  Vec2 posPlayer2;
  Vec2 posPlayer1;
  u_char axis;
  Region shapeBoundary; 
 
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    
    vec2Add(&posPlayer2, &mlP2 ->layer->posNext, &mlP2 -> velocity);
    abShapeGetBounds(mlP2->layer->abShape, &posPlayer2, &player2Fence);
     
    vec2Add(&posPlayer1, &mlP1 ->layer->posNext, &mlP1 -> velocity);
    abShapeGetBounds(mlP1->layer->abShape, &posPlayer1, &player1Fence);
    
    for (axis = 0; axis < 2; axis ++) {

      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) || //gameBall bounce
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ){
        int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
        newPos.axes[axis] += (2*velocity);
        if (shapeBoundary.topLeft.axes[0] < fence->topLeft.axes[0]){
            P1++;
            if(P1 == '8'){
                clearScreen(COLOR_WHITE);    
                drawString5x7(25,50, "Winner P2", COLOR_BLACK, COLOR_WHITE);
                __delay_cycles(10000000);
                P1 = '0';
                WDTCTL =0;
            }
            goalPlay();
            drawString5x7(50,50, "GOAL", colorScoreBoard, bg);
            __delay_cycles(15000000);
            break;
      }
      
      if(shapeBoundary.botRight.axes[0] > fence->botRight.axes[0]){
          P2++;  
          if(P2 == '8'){
              clearScreen(COLOR_WHITE);    
              drawString5x7(25,50, "Winner P1", COLOR_BLACK, COLOR_WHITE);
              __delay_cycles(10000000);
              P2 = '0';
              WDTCTL=0;
          }
          goalPlay();
          drawString5x7(50,50, "GOAL", colorScoreBoard, bg);
          __delay_cycles(15000000);
          break;
      }
      break;
      }	/**< if outside of fence */

      //Player 2(Left side) bounce
      if((shapeBoundary.topLeft.axes[0] < player2Fence.botRight.axes[0]) &&
        (shapeBoundary.botRight.axes[1]-11 < player2Fence.botRight.axes[1])
        && (shapeBoundary.topLeft.axes[1]+11 > player2Fence.topLeft.axes[1])){
        play();
        int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
        newPos.axes[axis] += (2*velocity);
        break;
      }

      //Player 1(Right side) bounce
      if((shapeBoundary.botRight.axes[0] > player1Fence.topLeft.axes[0]) &&
        (shapeBoundary.botRight.axes[1]-11 < player1Fence.botRight.axes[1])
        && (shapeBoundary.topLeft.axes[1]+11 > player1Fence.topLeft.axes[1])){
        int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
        newPos.axes[axis] += (2*velocity);
        play();
        break;
      }
      
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}




u_int bgColor = COLOR_BLACK;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */



void mainMenu(){
    int a = 1;
    
    clearScreen(COLOR_WHITE);
    drawString5x7(35,5, "ARCH PONG", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(25,25, "P1 controls", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(15,35, "S1 Up, S2 Down", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(25,60, "P2 controls", COLOR_BLACK, COLOR_WHITE); 
    drawString5x7(15,70, "S3 Up, S4 Down", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(5,90, "Select Theme:", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(5,110, "S1(B/W) S2(UTEP)", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(5,125, "S3(Soccer)", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(15,140, "Press S4 to Start", COLOR_BLACK, COLOR_WHITE);
    
    while(a){
        swSD1 = (P2IN & BIT0) ? 0 : 1;
        swSD2 = (P2IN & BIT1) ? 0 : 1;
        swSD3 = (P2IN & BIT2) ? 0 : 1;
        swSD4 = (P2IN & BIT3) ? 0 : 1;
        if(swSD1){
            bgColor = COLOR_WHITE;
            colorBall = COLOR_BLACK;
            colorScoreBoard = COLOR_BLACK;
            bg = COLOR_WHITE;
            ml3.layer->color=COLOR_BLACK;
            fieldLayer.color=COLOR_BLACK;
        }
        
        if(swSD2){
            bgColor = COLOR_ORANGE;
            colorBall = COLOR_BLUE;
            colorScoreBoard = COLOR_BLUE;
            bg = COLOR_ORANGE;
            ml3.layer->color=COLOR_BLUE;
            fieldLayer.color=COLOR_BLUE;
        }
        
        if(swSD3){
            bgColor = COLOR_GREEN;
            colorBall = COLOR_BLACK;
            colorScoreBoard = COLOR_BLACK;
            bg = COLOR_GREEN;
            ml3.layer->color=COLOR_BLACK;
            fieldLayer.color=COLOR_WHITE;
            
        }
        
        if(swSD4)
            a =0;
    }
    
    
}

void gameScore()
{
  drawString5x7(50,5, "SCORE", colorScoreBoard, bg);
  drawChar5x7(25, 5, P2 , colorScoreBoard, bg);
  drawChar5x7(100, 5, P1 , colorScoreBoard, bg); 
}

void P1MovUp(){ //function to mov up P1 pad
    movLayerDraw(&uPly2, &player2);
    mlAdvance(&uPly2, &fieldFence);
    
}

void P1MovDown(){ //function to mov down P1 pad
    movLayerDraw(&dPly2, &player2);
    mlAdvance(&dPly2, &fieldFence);
}

void P2MovUp(){ //function to mov down P2 pad
    movLayerDraw(&uPly1, &player1);
    mlAdvance(&uPly1, &fieldFence);
}

void P2MovDown(){ //function to mov down P2 pad
    movLayerDraw(&dPly1, &player1);
    mlAdvance(&dPly1, &fieldFence);
}

void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;
  
  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(1);
  
  buzzer_init();
  shapeInit();
  
  mainMenu();

  begin: layerInit(&player1);
  layerDraw(&player1);

  layerGetBounds(&fieldLayer, &fieldFence);
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */


  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml3, &player1);

    //movePlayer();
    swSD1 = (P2IN & BIT0) ? 0 : 1;
    swSD2 = (P2IN & BIT1) ? 0 : 1;
    swSD3 = (P2IN & BIT2) ? 0 : 1;
    swSD4 = (P2IN & BIT3) ? 0 : 1; 
    
    if(swSD1) 
        P1MovUp();

    if(swSD2) 
        P1MovDown(); 
    
    if(swSD3)
        P2MovUp();
    
    if(swSD4)
        P2MovDown();
    

    
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 4) {
    mlAdvanceNew(&ml3, &uPly1, &uPly2, &fieldFence);
    redrawScreen = 1;
    count = 0;
    buzzer_set_period(0);
    gameScore();
  } 
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
