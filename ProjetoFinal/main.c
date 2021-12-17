#include <proc/pic18f4520.h>
#include "config.h"
#include "lcd.h"
#include "ssd.h"
#include "keypad.h"
#include "timer.h"
#include "adc.h"
#include "io.h"
#include "rgb.h"
#include "so.h"

#define DISP_4_PIN    PIN_D3
#define DISP_3_PIN    PIN_D2
#define DISP_2_PIN    PIN_D1
#define DISP_1_PIN    PIN_D0 

#define LOW 0
#define HIGH 1

unsigned char op[7][8] = {"FULL", "LEGS", "BACK", "ARMS", "RETURN", "POWER"};
int potencia=0;
char selecionado=0;
int funcaoAndamento=-1;

void escolheFunc(void)
{
    char caso=0;
    char tecla=0;
    char i=0;
    char vert=0, hori=0;
    
    //Limpando o display
    lcdCommand(0x01);
    
    //Desligando qualquer opcao de massagem
    funcaoAndamento=-1;
    
    //Deixando a potencia em 50%
    potencia=50;
    
    //Selecionar a opcao de massagem
    while(selecionado==0)
    {   

        switch(caso)
        {
            //Impressao das opcoes
            case 0:             
                rgbColor(1);
                lcdCommand(0x80);
                lcdString(op[0]);
                lcdPosition(0, 9);    
                lcdString(op[1]);

                lcdCommand(0xC0);
                lcdString(op[2]);
                lcdPosition(1, 9);    
                lcdString(op[3]);
                caso=1;
                break;

            //Leitura do keypad para a selecao do modo
            case 1:
                kpDebounce();
                tecla=kpReadKey();
                kpDebounce();

                if(tecla!=kpReadKey())
                {
                    switch(tecla)
                    {
                        case 'U':
                            vert=0;
                            break;
                        case 'L':
                            hori=0;
                            break;
                        case 'D':
                            vert=1;
                            break;
                        case 'R':
                            hori=1;
                            break;
                        case 'S':
                            selecionado=1;
                            break;
                    }
                }
                tecla=0;
                //Marcando na string a opcao selecionada
                if(vert==0)
                {
                    if(hori==1)
                    {
                        op[1][4]='<';
                        funcaoAndamento=1;
                    }
                    else
                    {
                        op[0][4]='<';
                        funcaoAndamento=0;
                    }
                }
                else
                {
                    if(hori==1)
                    {
                        op[3][4]='<';
                        funcaoAndamento=3;
                    }
                    else
                    {
                        op[2][4]='<';
                        funcaoAndamento=2;
                    }
                }

                for(i=0;i<4;i++)
                {
                    if(i!=funcaoAndamento)
                        op[i][4]=' ';
                }

                caso=0;
                break;
            default:
                caso=0;
                break;
        }
        digitalWrite(DISP_1_PIN,LOW);
        digitalWrite(DISP_2_PIN,LOW);
        digitalWrite(DISP_3_PIN,LOW);
        digitalWrite(DISP_4_PIN,LOW);
    }
}

void segMenu(void)
{
    char caso=0;
    char tecla=0;
    char hori=0;
    
    
    //Limpando o display
    lcdCommand(0x01);
    op[funcaoAndamento][4]=' ';

    //Menu para voltar / mudar a intencidade da massagem
    op[4][6]=' ';
    caso = 0;
    while(selecionado==1)
    {
        switch(caso)
        {
            case 0:
                rgbColor(2);
                //Imprimindo no lcd a opcao escolhida
                lcdPosition(0, 5);
                lcdString(op[funcaoAndamento]);
                lcdCommand(0xC0);
                lcdString(op[5]);
                lcdPosition(1, 9);
                lcdString(op[4]);

                caso=1;
                break;

            case 1:
                tecla=0;
                kpDebounce();
                tecla=kpReadKey();
                kpDebounce();

                switch(tecla)
                {
                    case 'L':
                        hori=0;
                        break;
                    case 'R':
                        hori=1;
                        break;
                    case 'S':
                        if(tecla!=kpReadKey())
                        {
                            if(op[4][6]=='<')
                                selecionado=0;
                           
                            else if(op[5][5]=='<')
                                selecionado=2;
                        }   
                        break;

                    default:
                        break;
                }
                tecla=0;
                if(hori==0)
                {
                    op[4][6]=' ';
                    op[5][5]='<';
                }
                else
                {
                    op[5][5]=' ';
                    op[4][6]='<';
                }

                caso=0;
                break;

            default:
                caso=0;
                break;
        }
        digitalWrite(DISP_1_PIN,LOW);
        digitalWrite(DISP_2_PIN,LOW);
        digitalWrite(DISP_3_PIN,LOW);
        digitalWrite(DISP_4_PIN,LOW);
    }
}

void mudaPot(void)
{
    char caso=0;
    char tecla=0;
    
    
    //Limpando o display
    lcdCommand(0x01);
    
    //Mudanca de potencia
    caso = 0;
    op[4][6]='<';
    lcdCommand(0x80);
    lcdString(op[4]);
    while(selecionado==2)
    {
        timerReset(20000);
        switch(caso)
        {                 
            case 0:
                //Lendo o keypad 
                tecla=kpReadKey();
                kpDebounce();
                if(tecla!=kpReadKey())
                {
                    if(tecla=='S')
                        selecionado=1;
                }
                caso=1;
                break;

            case 1:
                potencia = adcRead(0);
                potencia/=10;
                ssdDigit((potencia%10),3);
                ssdDigit(((potencia/10)%10),2);
                ssdDigit(((potencia/100)%10),1);
                ssdDigit(0x00, 0);

                caso=0;
                break;

            default:
                caso=0;
                break;
        }
        ssdUpdate();
        timerWait();
        tecla=0;
    }

    digitalWrite(DISP_1_PIN,LOW);
    digitalWrite(DISP_2_PIN,LOW);
    digitalWrite(DISP_3_PIN,LOW);
    digitalWrite(DISP_4_PIN,LOW);

    //Limpando o display
    lcdCommand(0x01);
}

void main(void) 
{
    lcdInit();
    kpInit();
    ssdInit();
    adcInit();
    timerInit();
    for(;;)
    {
        switch(selecionado)
        {
            case 0:
                escolheFunc();
                break;
            case 1:
                segMenu();  
                break;
            case 2:
                mudaPot();
                break;
            default:
                break;
        }
    }
    return;
}
