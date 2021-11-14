#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <string.h>
#include "EPD_Test.h"   //Examples
#include "EPD_5in65f.h"

void  Handler(int signo)
{
  //System Exit
  printf("\r\nHandler:exit\r\n");
  DEV_Module_Exit();

  exit(0);
}

int initialize() {
  if(DEV_Module_Init()!=0){
    return -1;
  }

  printf("e-Paper Init and Clear...\r\n");
  EPD_5IN65F_Init();
  return 0;
}

int finalize() {
  printf("e-Paper Sleep...\r\n");
  EPD_5IN65F_Sleep();

  DEV_Delay_ms(2000);//important, at least 2s
  // close 5V
  printf("close 5V, Module enters 0 power consumption ...\r\n");
  DEV_Module_Exit();
}

int display_image(char* path) {
  UBYTE *img;
  /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
  UDOUBLE imageSize = ((EPD_5IN65F_WIDTH % 2 == 0)? (EPD_5IN65F_WIDTH / 2 ): (EPD_5IN65F_WIDTH / 2 + 1)) * EPD_5IN65F_HEIGHT;
  if((img = (UBYTE *)malloc(imageSize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    return -1;
  }
  Paint_NewImage(img, EPD_5IN65F_WIDTH, EPD_5IN65F_HEIGHT, 0, EPD_5IN65F_WHITE);
  Paint_SetScale(7);

  printf("show image for array\r\n");
  Paint_Clear(EPD_5IN65F_WHITE);
  GUI_ReadBmp_RGB_7Color(path, 0, 0);
  EPD_5IN65F_Display(img);
  DEV_Delay_ms(4000);
}

int display_clear() {
  printf("e-Paper Clear...\r\n");
  EPD_5IN65F_Clear(EPD_5IN65F_WHITE);
  DEV_Delay_ms(1000);
}

int display_text(char* text) {
  UDOUBLE Imagesize = ((EPD_5IN65F_WIDTH % 2 == 0)? (EPD_5IN65F_WIDTH / 2 ): (EPD_5IN65F_WIDTH / 2 + 1)) * EPD_5IN65F_HEIGHT;
  UBYTE *BlackImage;
  if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    return -1;
  }
  Paint_NewImage(BlackImage, EPD_5IN65F_WIDTH, EPD_5IN65F_HEIGHT, 0, EPD_5IN65F_WHITE);
  Paint_SetScale(7);
  Paint_Clear(EPD_5IN65F_WHITE);
  Paint_DrawString_EN(50, 74, text, &Font24, EPD_5IN65F_WHITE, EPD_5IN65F_BLACK);
  printf("e-Paper draw text\n");
  EPD_5IN65F_Display(BlackImage);
  free(BlackImage);
  BlackImage = NULL;
  DEV_Delay_ms(4000);
}

int main(int argc, char *argv[])
{
  if (argc <= 1 ){
    return 0;
  }
  if (strcmp(argv[1], "-c") == 0) {
    signal(SIGINT, Handler);
    if (initialize() == -1) {
      return -1;
    }
    display_clear();
    finalize();
  }
  if (strcmp(argv[1], "-i") == 0 && argc == 3) {
    signal(SIGINT, Handler);
    if (initialize() == -1) {
      return -1;
    }
    display_image(argv[2]);
    finalize();
  }

  if (strcmp(argv[1], "-t") == 0 && argc == 3) {
    signal(SIGINT, Handler);
    if (initialize() == -1) {
      return -1;
    }
    display_text(argv[2]);
    finalize();
  }
  return 0;
}
