///
/// Pong para Arduino (1 jugador)
///
/// by RafaG
///

#include "LedMax.h"
#include "SimpleTimer.h"

///
/// Definición de pines
//
// Pines de matriz LED
const int PIN_led_load = 12;
const int PIN_led_clk = 11;
const int PIN_led_cs = 10;
// Pines de mando
const int PIN_arribaL = 2;
const int PIN_abajoL = 3;
const int PIN_arribaR = 4;
const int PIN_abajoR = 5;
//
/// Fin de definición de pines
///


///
/// Otras constantes
///
const int FREQ_LECTURA_MANDO = 125;
const int FREQ_MOVIMIENTO_PALAS = 250;
const int FREQ_MOVIMIENTO_BOLA = 125;

const int limSup = 0;
const int limInf = 5;
const int tamPala = 3;

const int xL = 0;
const int xR = 7;
///
/// Fin Otras constantes
///


///
/// Instancias a clases
//
LedMax lc(PIN_led_load, PIN_led_clk, PIN_led_cs);
SimpleTimer timer;
//
/// Fin de instancias a clases
///


///
/// Variables globales
//
int timer1;
int timer2;
int timer3;

int posL;
int posR;
int posBx, posBy;

int movCandidatoL = 0;
int movCandidatoR = 0;

int trayectoria;
bool gol = false;
//
/// Fin variables globales
///


///
/// Funciones básicas Arduino
//
void setup()
{
    Serial.begin(9600);
    Serial.println("Inicio de sistema");

    // Inicializa matriz LED
    lc.initMax72xx();

    // Inicializa pines del mando
    pinMode(PIN_arribaL, INPUT);
    pinMode(PIN_abajoL, INPUT);
    pinMode(PIN_arribaR, INPUT);
    pinMode(PIN_abajoR, INPUT);

    // Inicializa timers
    timer1 = timer.setInterval(FREQ_LECTURA_MANDO, gestionaMandos);
    timer1 = timer.setInterval(FREQ_MOVIMIENTO_PALAS, gestionaPalas);
    timer3 = timer.setInterval(FREQ_MOVIMIENTO_BOLA, gestionaBola);  

    // Pintado inicial
    pintadoInicial();
}


void loop()
{
    timer.run();
}
//
/// Fin funciones básicas
///


///
/// Funciones periódicas
//
void gestionaMandos()
{
    int valArribaL = digitalRead(PIN_arribaL);
    int valAbajoL = digitalRead(PIN_arribaL);

    movCandidatoL = movPala(valArribaL, valAbajoL);

    int valArribaR = digitalRead(PIN_arribaR);
    int valAbajoR = digitalRead(PIN_arribaR);

    movCandidatoR = movPala(valArribaR, valAbajoR);

#ifdef MOVAUTO
    // Gestiona mov. automático
    int signo = (trayectoria > 0) ? 1 : -1;

    if(posR > posBy && signo == 1) movCandidatoR = -1;
    else if(posR < posBy && signo == 1) movCandidatoR = +1;

    if(posL > posBy && signo == -1) movCandidatoL = -1;
    else if(posL < posBy && signo == -1) movCandidatoL = +1;
#endif
}


void gestionaPalas()
{
    if(movCandidatoL != 0) 
    {
        posL = nuevaPosPala(movCandidatoL, posL);
        lc.clearDisplayCol(xL);
        lc.drawColumna(xL, posL, tamPala);
        movCandidatoL = 0;
    }
    if(movCandidatoR != 0)
    {
        posR = nuevaPosPala(movCandidatoR, posR);
        lc.clearDisplayCol(xR);
        lc.drawColumna(xR, posR, tamPala);
        movCandidatoR = 0;
    }
}


void gestionaBola()
{
    if(gol == true)
    {
        timer.disable(timer1);
        timer.disable(timer2);
        timer.disable(timer3);
        
        lc.invert();
        delay(500);
        lc.invert();
        delay(500);
        lc.clearAll();
        delay(500);
        gol = false;

        // Pintado inicial
        pintadoInicial();

        timer.enable(timer1);
        timer.enable(timer2);
        timer.enable(timer3);

        timer.restartTimer(timer1);
        timer.restartTimer(timer2);
        timer.restartTimer(timer3);

        return;
    }

    int signo = (trayectoria > 0) ? 1 : -1;
    int mov = abs(trayectoria);

    int xcand, ycand;

    xcand = posBx + signo;

    switch(mov)
    {
        case 1:
            if( (posBy - 1) >= 0 )
            {
                ycand = posBy - 1;
            }
            else
            {
                ycand = 1;
                trayectoria = signo * 3;
            }

            break;
        case 3:
            if( (posBy + 1) <= 7 )
            {
                ycand = posBy + 1;
            }
            else
            {
                ycand = 6;
                trayectoria = signo * 1;
            }
            break;
    }

    if(xcand == 0 || xcand == 7)
    {
        if( ( xcand == 0 && (ycand - posL) <= (tamPala - 1) && (ycand - posL) >= 0 ) || 
            ( xcand == 7 && (ycand - posR) <= (tamPala - 1) && (ycand - posR) >= 0 ) )// hay pala
        {
            trayectoria = -1 * trayectoria;
            signo = signo * -1;
            xcand = xcand + 2 * signo;
        }
        else
        {
            gol = true;
        }
    }

    lc.clearDisplayCol(posBx);
    lc.plot(xcand, ycand);

    posBx = xcand;
    posBy = ycand;
}
//
/// Fin funciones periódicas
///


int movPala(int valArriba, int valAbajo)
{
    int movCandidato;
    
    if(valArriba == HIGH && valAbajo == HIGH) //
    {
        // Do nothing
    }
    else if(valArriba == HIGH)
    {
        movCandidato = -1;
    }
    else if(valAbajo == HIGH)
    {
        movCandidato = +1;
    }

    return movCandidato;
}


int nuevaPosPala(int movCandidato, int pos)
{
    pos += movCandidato;
    if(pos < limSup) pos = limSup;
    if(pos > limInf) pos = limInf;

    return pos;
}


void pintadoInicial()
{
    // Pintado inicial
    int mitad = (limInf - limSup) / 2;
    posL = mitad;
    posR = mitad;
    posBx = 5; posBy = random(0, 7);

    trayectoria = -3;

    lc.drawColumna(xL, posL, tamPala);
    lc.drawColumna(xR, posR, tamPala);
    lc.plot(posBx, posBy);

    movCandidatoL = 0;
    movCandidatoR = 0;
}
