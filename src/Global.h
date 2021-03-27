/*******************************************/
/*                                         */
/*  Include general para todo el proyecto  */
/*                                         */
/*******************************************/

/***************************** Defines *****************************/
#ifndef _GLOBAL_
#define _GLOBAL_

//Defines generales
//#define NOMBRE_FAMILIA   "LoRaMetro"
#define VERSION          "Heltec wifi lora 32 - ver 0.3.0"

#define TRUE              1
#define FALSE             0
#define NO_CONFIGURADO   -1
#define CONFIGURADO       1

#define SEPARADOR        '|'
#define SUBSEPARADOR     '#'

//Ficheros de configuracion
#define GLOBAL_CONFIG_FILE         "/Config.json"
#define GLOBAL_CONFIG_BAK_FILE     "/Config.json.bak"
#define SENSORES_CONFIG_FILE       "/SensoresConfig.json"
#define SENSORES_CONFIG_BAK_FILE   "/SensoresConfig.json.bak"

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
#include <Configuracion.h>
/***************************** Includes *****************************/

/***************************** Variables *****************************/
const char NOMBRE_FAMILIA[]="LoRaMetro";

const uint32_t LECTURAS_AL_DIA = 24*60*4;  //Cuantas veces me despierto para leer al dia. Ej: 24*12 es una vez cada 5 min
const uint8_t LECTURAS_POR_ENVIO = 4;  //Cada cuantas medidas hago un envio

/*
struct configuracion_s{
  char nombre_dispositivo[16];  
  char tipoSensorTemperatura[LONG_TIPO_SENSOR_TEMPERATURA];
  char tipoSensorHumedad[LONG_TIPO_SENSOR_HUMEDAD];
  char tipoSensorPresion[LONG_TIPO_SENSOR_PRESION];
  char tipoSensorLuz[LONG_TIPO_SENSOR_LUZ];
};
extern struct configuracion_s configuracion;
*/

extern int debugGlobal; //por defecto desabilitado
extern int nivelActivo;
/***************************** Variables *****************************/

/************************* Utilidades ***************************/
void configuraLed(void);
void enciendeLed(void);
void apagaLed(void);
void parpadeaLed(uint8_t veces, uint16_t espera=100);
void parpadeaLed(uint8_t veces, uint16_t espera);
/************************* Utilidades ***************************/

#endif