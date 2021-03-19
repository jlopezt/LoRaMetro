/*******************************************/
/*                                         */
/*  Include general para todo el proyecto  */
/*                                         */
/*******************************************/

/***************************** Defines *****************************/
#ifndef _GLOBAL_
#define _GLOBAL_

//Defines generales
#define NOMBRE_FAMILIA   "LoRaMetro"
#define VERSION          "Heltec wifi lora 32 - ver 0.0.1"

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

#define MAX_DATOS       24      // MAximo numero de datos almacenados
/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Arduino.h>
#include <Traza.h>
#include <ArduinoJson.h>
/***************************** Includes *****************************/

/***************************** Tipos *****************************/
typedef struct {
  String nombre_dispositivo;    
} configuracion_t;

typedef struct {
  float temperatura;
  float presion;
  float humedad;
} datos_t;
/***************************** Tipos *****************************/

/***************************** Variables *****************************/
//extern String nombre_dispositivo;//Nombre del dispositivo, por defecto el de la familia
extern int debugGlobal; //por defecto desabilitado
extern int nivelActivo;

extern int bootCount; //RTC_DATA_ATTR memoria que se mantiene activa durante un deep_sleep
extern datos_t datos[MAX_DATOS];
extern configuracion_t configuracion;
/***************************** Variables *****************************/

/************************* Utilidades ***************************/
void configuraLed(void);
void enciendeLed(void);
void apagaLed(void);
void parpadeaLed(uint8_t veces, uint16_t espera=100);
void parpadeaLed(uint8_t veces, uint16_t espera);
/************************* Utilidades ***************************/

#endif