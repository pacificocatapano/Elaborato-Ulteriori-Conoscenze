
#include "shell.h"
#include "chprintf.h"
#include "ssd1306.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

static uint8_t msgindex = 0;
static char msgbuffer[SHELL_MAX_ARGUMENTS][SHELL_MAX_LINE_LENGTH];


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
          for(int i = 0; i < argc; i++){
                //chprintf(chp, "%s\r\n",argv[i]);
                //copia parola messaggio nel buffer
                sprintf(msgbuffer[msgindex++],"%s",argv[i]);
                for(unsigned int j = 0; j < strlen(argv[i]); j++)
                {
                    chprintf(chp, "%s ",charToMorse(argv[i][j]));
                    //copiare nel morsebuffer la stringa in codice morse riferita al singolo carattere
                }
                chprintf(chp, "   ");
            }
          chprintf(chp, "\r\n");
        }
    }
}


#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
const int MAX=1000;


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


  for(unsigned int j = 0; j < strlen(stringa1); j++)
  {
     chprintf(chp, "%s ",charToMorse(stringa1[j]));
     //copiare nel morsebuffer la stringa in codice morse riferita al singolo carattere
  }
  sprintf(buffer, "testOne");
  ssd1306Puts(&SSD1306D1, buffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
  ssd1306UpdateScreen(&SSD1306D1);
}
static void cmd_testTwo(BaseSequentialStream *chp,int argc,char*argv[]){


}


static const ShellCommand commands[] = {
  {"encode", cmd_encode},
  {"readkey", cmd_readkey},
  {"readbut", cmd_readbut},
  {"testOne", cmd_testOne},
  {"testTwo", cmd_testTwo},
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





