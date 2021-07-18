/*******************************************/
/*                                         */
/*  Include general para todo el proyecto  */
/*                                         */
/*******************************************/

/***************************** Defines *****************************/
#ifndef _GLOBAL_
#define _GLOBAL_

//Defines generales
#define VERSION          "LoRaMetro - ver 1.1.0"

#define TRUE              1
#define FALSE             0
#define NO_CONFIGURADO   -1
#define CONFIGURADO       1

#define SEPARADOR        '|'
#define SUBSEPARADOR     '#'

//Ficheros de configuracion
#define GLOBAL_CONFIG_FILE         "/Config.json"
#define GLOBAL_CONFIG_BAK_FILE     "/Config.json.bak"

//Longitud de los tipos de sensores
#define LONG_TIPO_SENSOR_TEMPERATURA 12
#define LONG_TIPO_SENSOR_HUMEDAD     12
#define LONG_TIPO_SENSOR_PRESION     12
#define LONG_TIPO_SENSOR_LUZ         12
/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Arduino.h>
#include <Traza.h>
#include <ArduinoJson.h>
/***************************** Includes *****************************/

/***************************** Variables *****************************/
const char NOMBRE_FAMILIA[]="LoRaMetro";

const uint32_t LECTURAS_AL_DIA = 24*60*4;  //Cuantas veces me despierto para leer al dia. Ej: 24*12 es una vez cada 5 min
const uint8_t LECTURAS_POR_ENVIO = 1;  //Cada cuantas medidas hago un envio
/***************************** Variables *****************************/

/************************* Utilidades ***************************/
void configuraLed(void);
void enciendeLed(void);
void apagaLed(void);
void parpadeaLed(uint8_t veces, uint16_t espera=100);
void parpadeaLed(uint8_t veces, uint16_t espera);
/************************* Utilidades ***************************/

#endif