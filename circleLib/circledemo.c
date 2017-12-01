#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include "abCircle.h"

AbRect rect10 = {abRectGetBounds, abRectCheck, {10,10}};; /**< 10x10 rectangle */

u_int bgColor = COLOR_BLACK;


Layer layer1 = {		/**< Layer with a red square */
  (AbShape *)&circle10,
  {screenWidth+2, screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* next & last pos */
  COLOR_WHITE,
  0
};

Layer layer0 = {		/**< Layer with an orange circle */
  (AbShape *)&circle14,
  {(screenWidth/2)+10, (screenHeight/2)+5}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* next & last pos */
  COLOR_ORANGE,
  &layer1,
};

int
main()
{
  configureClocks();
  lcd_init();

  clearScreen(COLOR_BLACK);
  drawString5x7(20,20, "hello", COLOR_GREEN, COLOR_RED);

  layerDraw(&layer0);

}
