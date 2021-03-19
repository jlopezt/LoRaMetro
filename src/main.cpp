/*************************************************************************************************
 Informacion sobre modos de bajo consumo ESP32
 https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html
 https://github.com/espressif/esp-idf/blob/c49d03b8eb9cb2070960927cbd7194645c44a874/examples/system/deep_sleep/main/deep_sleep_example_main.c
 https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
*************************************************************************************************/

/***************************** Defines *****************************/
#define NUM_CORES       2       /* numero de cores */
#define uS_TO_S_FACTOR  1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP   30      /* Time ESP32 will go to sleep (in seconds) */
//#define DEEPSLEEP_RESET 5     /* Reincio por despertar de deep sleep */
/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Arduino.h>
#include <rom/rtc.h>

#include <Ficheros.h>
#include <Ordenes.h>
#include <Sensores.h>
#include <LoRa.h>
/***************************** Includes *****************************/

/*************************** Tipos *******************************/
typedef enum  {
  HARDRESET = 0,
  SOFTRESET,
  DESPERTAR
} tipoReset_t;
/*************************** Tipos *******************************/

/*************************** Prototipos *******************************/
boolean inicializaConfiguracion(boolean debug);
boolean parseaConfiguracionGlobal(String contenido);
void print_wakeup_reason(void);
const char* reset_reason(RESET_REASON reason);

void setupTotal(void);
void setupDespertar(void);

//void cargarConfigNVR(void);
//void salvarConfigNVR(void);
/*************************** Prototipos *******************************/

/***************************** variables globales *****************************/
//RTC_DATA_ATTR String nombre_dispositivo;//Nombre del dispositivo, por defecto el de la familia
int debugGlobal; //por defecto desabilitado

RTC_DATA_ATTR int bootCount; //RTC_DATA_ATTR memoria que se mantiene activa durante un deep_sleep
RTC_DATA_ATTR datos_t datos[MAX_DATOS];
RTC_DATA_ATTR configuracion_t configuracion;
/***************************** variables globales *****************************/

void setup() {
  //Init serial, lo unico que hago fuera de los tipos de setup
  Serial.begin(115200);  

  Serial.printf("milis init: %lu\nNombre dispositivo: %s\n",millis(),configuracion.nombre_dispositivo.c_str());

  //1.-Despierta
  //**********************************
  //switch en funcion del reset reason
  //**********************************
  tipoReset_t tipoReset=HARDRESET;
  for(int8_t core=0;core<NUM_CORES;core++) {
    //Traza.mensaje("Motivo del reinicio (%i): %s\n",core,reset_reason(rtc_get_reset_reason(core)));  
    if(rtc_get_reset_reason(core)==DEEPSLEEP_RESET) {
      tipoReset=DESPERTAR;
      core=NUM_CORES;
    }
  }

  switch (tipoReset){
    case HARDRESET:
    case SOFTRESET: 
      Serial.printf("Hard o Soft reset\n");
      setupTotal();   
      break;
    case DESPERTAR:
      //Print the wakeup reason for ESP32
      Serial.printf("Wakeup reset: ");      
      print_wakeup_reason();    
      //Carga la config de la memoria no volatil
      setupDespertar();
      ++bootCount;
      break;    
  }
  Serial.println("Boot number: " + String(bootCount));
  delay(2);


  Serial.printf("milis mid: %lu\n",millis());
  //2.-mide
  sensores.lee(false);

  //3.-procesa la medida
  String cad=sensores.generaJsonEstado();

  //4.-comunica
  Serial.printf("%s\n",cad.c_str());
  Lora.envia(cad);
  //5.-procesa respuesta
  //To Do....

  //6.-duerme
  //Si no esta conectado por puerto serie, se duerme
  if(!Serial || 1){
    //salvarConfigNVR();
    Serial.printf("milis fin: %lu\n",millis());
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    delay(100);
    esp_deep_sleep_start();
    Serial.println("This will never be printed");  
  }
}

void loop() {
  //solo se  ejecuta si no hay conectado algo en el puerto serie
  Serial.printf("tic...");
  delay(5000);
  Serial.printf("tac...\n");
  delay(5000);
}

/********************************** SETUPs ********************************************/
void setupTotal(void){
  //Init ficheros
  inicializaFicheros(false);
  //Init general (nombre, tiempos de ivernacion,... )
  inicializaConfiguracion(false);
  //Init Ordenes
  inicializaOrden();
  //Init metro
  sensores.inicializa(false);
  //init Lora
  Lora.inicializa();

  //salvarConfigNVR();
  bootCount=0;
}  
  
void setupDespertar(void){
  //cargarConfigNVR();
  //init Lora
  Lora.inicializa();
}
/*
void cargarConfigNVR(void){
  Serial.printf("Recuperando configuracion:\n");
  Serial.printf("Nombre de dispositivo: %s\n",configuracion.nombre_dispositivo.c_str());
  //nombre_dispositivo=configuracion.nombre_dispositivo;
}
void salvarConfigNVR(void){
  configuracion.nombre_dispositivo=nombre_dispositivo;
  Serial.printf("Salvando configuracion:\n");
  Serial.printf("Nombre de dispositivo: %s\n",configuracion.nombre_dispositivo.c_str());
}
*/
/********************************** Fin SETUPs ********************************************/
/********************************** Funciones de configuracion global **************************************/
/************************************************/
/* Recupera los datos de configuracion          */
/* del archivo global                           */
/************************************************/
boolean inicializaConfiguracion(boolean debug)
  {
  String cad="";
  if (debug) Traza.mensaje("Recupero configuracion de archivo...\n");

  //cargo el valores por defecto
  configuracion.nombre_dispositivo=String(NOMBRE_FAMILIA); //Nombre del dispositivo, por defecto el de la familia
  
  if(!leeFichero(GLOBAL_CONFIG_FILE, cad))
    {
    Traza.mensaje("No existe fichero de configuracion global\n");
    cad="{\"nombre_dispositivo\": \"" + String(NOMBRE_FAMILIA) + "\"}"; //config por defecto    
    //salvo la config por defecto
    if(salvaFichero(GLOBAL_CONFIG_FILE, GLOBAL_CONFIG_BAK_FILE, cad)) Traza.mensaje("Fichero de configuracion global creado por defecto\n"); 
    }

  return parseaConfiguracionGlobal(cad);
  }

/*********************************************/
/* Parsea el json leido del fichero de       */
/* configuracio global                       */
/*  auto date = obj.get<char*>("date");
    auto high = obj.get<int>("high");
    auto low = obj.get<int>("low");
    auto text = obj.get<char*>("text");
 *********************************************/
boolean parseaConfiguracionGlobal(String contenido)
  {  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(contenido.c_str());
  
  if (json.success()) 
    {
    Traza.mensaje("parsed json\n");
//******************************Parte especifica del json a leer********************************
    if (json.containsKey("nombre_dispositivo")) configuracion.nombre_dispositivo=((const char *)json["nombre_dispositivo"]);    
    if(configuracion.nombre_dispositivo==NULL) configuracion.nombre_dispositivo=String(NOMBRE_FAMILIA);
    
    Traza.mensaje("Configuracion leida:\nNombre dispositivo: %s\n",configuracion.nombre_dispositivo.c_str());
//************************************************************************************************
    return true;
    }
  return false;  
  }
/********************************** Funciones de configuracion global **************************************/
/********************************** Utilidades **************************************/
/*********************************************************************/
/*Method to print the reason by which ESP32 has been awake from sleep*/
/*********************************************************************/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  :
    {
      Serial.println("Wakeup caused by external signal using RTC_IO");
      delay(2);
    } break;
    case 2  :
    {
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      delay(2);
    } break;
    case 3  :
    {
      Serial.println("Wakeup caused by timer");
      delay(2);
    } break;
    case 4  :
    {
      Serial.println("Wakeup caused by touchpad");
      delay(2);
    } break;
    case 5  :
    {
      Serial.println("Wakeup caused by ULP program");
      delay(2);
    } break;
    default :
    {
      Serial.println("Wakeup was not caused by deep sleep");
      delay(2);
    } break;
  }
}

/***************************************************************/
/*  Decodifica el motivo del ultimo reset                      */
/***************************************************************/
const char* reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : return ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
    case 3 : return ("SW_RESET");break;               /**<3,  Software reset digital core*/
    case 4 : return ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : return ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : return ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : return ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : return ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : return ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : return ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : return ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : return ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : return ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : return ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : return ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : return ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : return ("NO_MEAN");
  }
}
/********************************** Utilidades **************************************/