/*
 * Versione conclusiva del progetto 3
 *
 *
 */


/*   |---------------------------------------------------------------|
 *   |                         PIN-MAP                               |
 *   |-----|---------|----------|--------------------|---------------|
 *   | PIN |  CON    |   TO     |        MODE        |  PERIPHERAL   |
 *   |-----|---------|----------|--------------------|---------------|
 *   | PB4 | ARD-D4  | BUZZER   |   ALTERNATE        |   PWM_OUT1    |
 *   | PB5 | ARD-D5  | BUTTON   |   INPUT            |   GPIOB5      |
 *   | PB8 | ARD-D15 | OLED     |   ALTERNATE        |   I2C SCL     |
 *   | PB9 | ARD-D14 | OLED     |   ALTERNATE        |   I2C SDA     |
 *   | PA6 | ARD-D12 | TX       |   OUTPUT           |   GPIOA6      |
 *   | PA7 | ARD-D11 | RX       |   INPUT            |   GPIOA7      |
 *   ---------------------------------------------------------------
 *
 */

#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "chprintf.h"
#include "ssd1306.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)


#define TX_LED      PAL_LINE(GPIOA, 6U)
#define RX_LED      PAL_LINE(GPIOA, 7U)

#define MAX_LENGTH_MORSE 5

static char* morse[] = {"-----",".----","..---","...--","....-", ".....", "-....","--...","---..","----.",
                      ".-","-...","-.-.","-..",".","..-.","--.","....","..",".---", "-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-", "...-",".--","-..-","-.--","--.."};char * tx_buffer =
                          ".-.-...--....         \0";

static uint8_t msgindex = 0;
//static uint8_t morseindex = 0;
static char msgbuffer[SHELL_MAX_ARGUMENTS][SHELL_MAX_LINE_LENGTH];
//static char morsebuffer[SHELL_MAX_ARGUMENTS][MAX_LENGTH_MORSE * SHELL_MAX_LINE_LENGTH];




  char* supp[50];
  int start=0; //Variabile per tenere conto dell'inizio
    int attesa=0; //Attesa necessaria per lo spazio tra i caratteri
    int attesa2=0; //Attesa necessaria per la fine della parola
    char stringa[100]; //Vettore in cui si memorizza la frase
    //Sono le righe del display

    char riga2[10];



    int i=0; //Variabile utilizzata per incrementare la dim. del vett. stringa
    //Variabili booleane utilizzate per evitare che si stampi all'infinito
    bool stampa=false;
    bool spazio=false;




char* charToMorse(char c){

  if (c > 47 && c < 58)
    return morse[c - '0'];
  else if(c > 64 && c < 91)
    return morse[c - 'A' + 10];
  else if(c > 96 && c < 123)
      return morse[c - 'a' + 10];
  else
    return "?";
}

typedef struct {
  char alfa;
  char morse[5];
}convert;

convert conv[26] = {
                      {'a',{'.','-','\0'}},
                      {'b',{'-','.','.','.','\0'}},
                      {'c',{'-','.','-','.','\0'}},
                      {'d',{'-','.','.','\0'}},
                      {'e',{'.','\0'}},
                      {'f',{'.','.','-','.','\0'}},
                      {'g',{'-','-','.','\0'}},
                      {'h',{'.','.','.','.','\0'}},
                      {'i',{'.','.','\0'}},
                      {'j',{'.','-','-','-','\0'}},
                      {'k',{'-','.','-','\0'}},
                      {'l',{'.','-','.','.','\0'}},
                      {'m',{'-','-','\0'}},
                      {'n',{'-','.','\0'}},
                      {'o',{'-','-','-','\0'}},
                      {'p',{'.','-','-','.','\0'}},
                      {'q',{'-','-','.','-','\0'}},
                      {'r',{'.','-','.','\0'}},
                      {'s',{'.','.','.','\0'}},
                      {'t',{'-','\0'}},
                      {'u',{'.','.','-','\0'}},
                      {'v',{'.','.','.','-','\0'}},
                      {'w',{'.','-','-','\0'}},
                      {'x',{'-','.','.','-','\0'}},
                      {'y',{'-','.','-','-','\0'}},
                      {'z',{'-','-','.','.','\0'}}
  };


/*
 * PWM
 *
 */

#define C4 1911

char buffer[50];
char parola[100];
int contaparole=0;
static SSD1306Driver SSD1306D1;

static const I2CConfig i2ccfg = {
  OPMODE_I2C,
  400000,
  FAST_DUTY_CYCLE_2,
};

static const SSD1306Config ssd1306cfg = {
  &I2CD1,
  &i2ccfg,
  SSD1306_SAD_0X78,
};

void pwmWidtchCb(PWMDriver *pwmp){
  (void)pwmp;
  chprintf((BaseSequentialStream*)&SD2, "START PERIOD\n\r");
}
//Callback 1
static void pwmpcb(PWMDriver *pwmp) {
  (void)pwmp;
  palSetPad(GPIOB,4);
}
//Callback 2
static void pwmc1cb(PWMDriver *pwmp) {
  (void)pwmp;
  palClearPad(GPIOB,4);
}
//Configurazione PWM
static PWMConfig pwmcfg = {
  1000000,                                    /* 1MHZ PWM clock frequency.   */
  100,                                    /* Initial PWM period 1ms.       */
  pwmpcb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
};
/*
 *
 * FINE PWM CONF
 */

/*
 * IMPORT SEZIONE LED IR
 */
bool spazio1 = false;



//THREAD - WORKING AREA
static THD_WORKING_AREA(waTx, 128);



//THREAD - FUNCTION
static THD_FUNCTION(thdTx, arg) {
  (void) arg;



  int index = 0;



   while(true){



    index = index%strlen(tx_buffer);


    if(tx_buffer[index] == '.'){
      palSetLine(TX_LED);
      spazio1 = false;
      chThdSleepMilliseconds(200);
    }else if(tx_buffer[index] == '-'){
      palClearLine(TX_LED);
      spazio1 = false;
      chThdSleepMilliseconds(200);
    }else{
      palClearLine(TX_LED);
      spazio1 = true;
      chThdSleepMilliseconds(200);
    }
    index++;
   }

}
/*
 * FINE IMPORT THREAD LED INFRAROSSI
 *
 */


/*
 * DEFINIZIONE COMANDI SHELL
 *
 */

static void cmd_encode(BaseSequentialStream *chp, int argc, char *argv[]) {

    switch (argc){
        case 0:
          //Quando scrivi solo write  e poi invio
          chprintf(chp, "\r\n\n Devi inserire qualcosa!!!! \r\n");
        break;
        //Lettere inserite
        case 1:{
          /*
          if(strcmp(argv[1],"?"))
            chprintf(chp, "\r\n\n Descrizione utilizzo comando\r\n");
            */
        }
        default :{

            //Qui entra quando l'utente ha inserito almeno una parola
          msgindex = 0;
          int lunghezza_parola=0;
          for(int i = 0; i < argc; i++){
                //chprintf(chp, "%s\r\n",argv[i]);
                //copia parola messaggio nel buffer
                sprintf(msgbuffer[msgindex++],"%s",argv[i]);
                for(unsigned int j = 0; j < strlen(argv[i]); j++)
                {
                    chprintf(chp, "%s ",charToMorse(argv[i][j]));
                    //copiare nel morsebuffer la stringa in codice morse riferita al singolo carattere
                    lunghezza_parola=j;
                }
                chprintf(chp, "   ");
            }


          char stringaRX[100];
          int k = 0;
          while(k<lunghezza_parola){
              if(palReadLine(RX_LED) == PAL_HIGH){
                palSetPad(GPIOA, 5U);
                stringaRX[k] = '.';
              }else{
                palClearPad(GPIOA, 5U);
                if(spazio){
                  stringaRX[k] = ' ';
                }else{
                  stringaRX[k] = '-';
                }
              }
              chprintf(chp, "%c ",stringaRX[k]);
              k++;
              chThdSleepMilliseconds(200);
            }



          chprintf(chp, "\r\n");
        }
    }
}

char morseToChar(char buffer[], convert conv[], BaseSequentialStream *chp){
  int notFound = 0;
  for(int n =0;n<26;++n){
    if (strcmp(buffer,conv[n].morse)==0){
      //chprintf(chp, "%c",conv[i].alfa);
      notFound=1;
      return conv[n].alfa;
    }
  }

  if(notFound ==0){
    chprintf(chp,"%s","Questa stringa non codifica nessuna parola in morse \n");
    return '\0';

  }

}



#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
const int MAX=1000;


//This command reads from keyboard


//This command reads from button
static void cmd_readkey(BaseSequentialStream *chp,int argc,char*argv[]){
  char buffer[MAX];
  (void)argc;
  (void)argv;
  for( int j=0;j<argc;j++)
  {
        strcpy(buffer,argv[j]);
        morseToChar(buffer,conv, chp);

  }
  chprintf(chp,"\r\n",buffer);


}
static void cmd_readbut(BaseSequentialStream *chp,int argc,char*argv[])
{
  char buffer[MAX];
  (void)argc;
  (void)argv;

  int contatore=0;

while(palReadLine(LINE_BUTTON)==PAL_HIGH)
{

  while(palReadPad(GPIOB, 5)==PAL_LOW)
      {
        //Inizializzazione variabili a ogni ciclo
         attesa=0;
         attesa2=0;
         contatore=0;
         start=1;


         // Finchè il pulsante è premuto,incrementa la variabile contatore
           while(palReadPad(GPIOB, 5)==PAL_LOW)
           {
             chThdSleepMilliseconds(500);
             contatore++;
           }
          chThdSleepMilliseconds(100);
      }

      //Stampa il punto

      if(contatore==1)
      {
        chprintf(chp, ".");
        contatore=0;
        attesa=0;
        attesa2=0;
        stringa[i]='.';
        //Aggiunta la stampa a schermo





        i++;

        stampa=false;
        spazio=false;
      }

      //Stampa il trattino
      if(contatore>=2)
      {
         chprintf(chp, "-");
         contatore=0;
         attesa=0;
         attesa2=0;
         stringa[i]='-';

         i++;
         stampa=false;
         spazio=false;
     }
      //Attesa per lo spazio di fine carattere


      if(spazio==false && attesa>30  && start ==1)
      {

        chprintf(chp, " ");
        attesa=0;
        stringa[i]='\0';

        parola[contaparole]=morseToChar(stringa,conv,chp);
        contaparole++;
        i=0;



        spazio=true;
      }



      if(attesa2>100 && start ==1)
          {
            while(stampa==false){

            //Stampa della stringa
           // chprintf(chp,"[stampo stringa]");







            attesa=0;
            attesa2=0;

              stampa=true;
            }
        }
       //Attesa per lo spazio di fine parola. Incremento dei contatori
      attesa++;
      attesa2++;
      chThdSleepMilliseconds(25);
    }
   chprintf(chp,"Parola tradotta: %s \n",parola);ssd1306GotoXy(&SSD1306D1, 0, 1);
   sprintf(buffer, "%s", parola);
   ssd1306Puts(&SSD1306D1, buffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
   ssd1306UpdateScreen(&SSD1306D1);

// chprintf(chp,"\r\n",buffer);

}




static void cmd_testOne(BaseSequentialStream *chp,int argc,char*argv[]){

  char stringa1[]="HELLO";

  (void)argv;

  if(argc==0)
  {
  for(unsigned int j = 0; j < strlen(stringa1); j++)
  {
     chprintf(chp, "%s ",charToMorse(stringa1[j]));
     //copiare nel morsebuffer la stringa in codice morse riferita al singolo carattere
  }

  sprintf(buffer, "testOne");
  ssd1306Puts(&SSD1306D1, buffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
  ssd1306UpdateScreen(&SSD1306D1);
  }
}





//This command transmits via IR led
static void cmd_transmit(BaseSequentialStream *chp,int argc,char*argv[]){
  (void)argc;
  (void)argv;
  (void)chp;






  //NEEDS EVERYTHING
}

//This command receives via IR led
static void cmd_receive(BaseSequentialStream *chp,int argc,char*argv[]){
  (void)argc;
  (void)argv;
  (void)chp;


  //NEEDS EVERYTHING
}

static const ShellCommand commands[] = {
  {"encode", cmd_encode},
  {"readkey", cmd_readkey},
  {"readbut", cmd_readbut},
  {"testOne", cmd_testOne},
  {"transmit",cmd_transmit},
  {"receive", cmd_receive},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SD2,
  commands
};

/*
 * FINE DEFINIZIONE COMANDI SHELL
 *
 */





int main(void) {
  halInit();
  chSysInit();

  shellInit();

  sdStart(&SD2, NULL);
  palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(2)); //Settaggio del PIN PWM
  palSetPadMode(GPIOB, 5, PAL_MODE_INPUT); //Settaggio del PIN per leggere button



    //Configurazione importata TX RX
    palSetLineMode(TX_LED, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(RX_LED, PAL_MODE_INPUT);
    chThdCreateStatic(waTx, sizeof(waTx), NORMALPRIO + 1, thdTx, NULL);


    //Configurazione PWM
    pwmStart(&PWMD3, &pwmcfg);
    chThdSleepMilliseconds(10);
    pwmEnablePeriodicNotification(&PWMD3);
    pwmEnableChannelNotification(&PWMD3, 0);
    pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, (uint32_t)7000 ));
    pwmChangePeriod(&PWMD3, C4);
    chThdSleepMilliseconds(10);

    //Configurazione I2C
    /* Configuring I2C related PINs */
    palSetLineMode(LINE_ARD_D15, PAL_MODE_ALTERNATE(4) |
                      PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                      PAL_STM32_PUPDR_PULLUP);
    palSetLineMode(LINE_ARD_D14, PAL_MODE_ALTERNATE(4) |
                      PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                      PAL_STM32_PUPDR_PULLUP);


    //Configurazione Display SCHERMO

    ssd1306ObjectInit(&SSD1306D1);
    ssd1306Start(&SSD1306D1, &ssd1306cfg);
    ssd1306FillScreen(&SSD1306D1, 0x00);



  while (true) {
   thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE, "shell", NORMALPRIO + 1,
                                            shellThread, (void *)&shell_cfg1);

   chThdWait(shelltp);

}
  pwmDisableChannel(&PWMD3, 0);
    pwmStop(&PWMD3);

}
