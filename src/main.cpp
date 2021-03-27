/*************************************************************************************************
 Informacion sobre modos de bajo consumo ESP32
 https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html
 https://github.com/espressif/esp-idf/blob/c49d03b8eb9cb2070960927cbd7194645c44a874/examples/system/deep_sleep/main/deep_sleep_example_main.c
 https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
*************************************************************************************************/

/***************************** Defines *****************************/
#define NUM_CORES       2       /* numero de cores */
#define PIN_DETECTOR   23       /* Pin que determina si arranca en modo cargador o en modo normal */
#define DETECTOR_ACTIVADO LOW
#define PARTICION_LOADER  String("app0")
/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Arduino.h>
#include <esp_timer.h>
#include <rom/rtc.h>
#include <esp_ota_ops.h>

#include <Global.h>
#include <Configuracion.h>
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

/***************************** variables globales *****************************/
const uint64_t uS_TO_S_FACTOR = 1000000; /* Conversion factor for micro seconds to seconds */
const uint64_t mS_TO_S_FACTOR = 1000;    /* Conversion factor for mili seconds to seconds */

const uint64_t HORAS_EN_DIA = 24;
const uint64_t SEGUNDOS_EN_HORA = 3600;

const uint64_t AJUSTE_RELOJ = 150000;     /* Una ñapa digna para ajustar el reloj..., en us */
const uint64_t timeToSleep=((HORAS_EN_DIA * SEGUNDOS_EN_HORA * uS_TO_S_FACTOR)/(LECTURAS_AL_DIA))-AJUSTE_RELOJ;

RTC_DATA_ATTR unsigned int bootCount; //RTC_DATA_ATTR memoria que se mantiene activa durante un deep_sleep
/***************************** variables globales *****************************/

/********************************** Utilidades **************************************/
/*********************************************************************/
/*Method to print the reason by which ESP32 has been awake from sleep*/
/*********************************************************************/
const char* wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : return ("Wakeup caused by external signal using RTC_IO");break;
    case 2  : return ("Wakeup caused by external signal using RTC_CNTL");break;
    case 3  : return ("Wakeup caused by timer");break;
    case 4  : return ("Wakeup caused by touchpad");break;
    case 5  : return ("Wakeup caused by ULP program");break;
    default : return ("Wakeup was not caused by deep sleep");break;
  }
}

/***************************************************************/
/*  Decodifica el motivo del ultimo reset                      */
/***************************************************************/
const char* reset_reason(RESET_REASON reason)
{
  switch (reason)
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
/********************************** Inicializacion ********************************************/
void inicializaDetector(boolean debug){pinMode(PIN_DETECTOR, INPUT);}

boolean modoDetector(boolean debug){
  if(digitalRead(PIN_DETECTOR)==DETECTOR_ACTIVADO) return true;
  return false;
}

boolean setParticionProximoArranque(String nombre)
  {
  esp_partition_iterator_t iterador=esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY,nombre.c_str()); 
  const esp_partition_t *particion=esp_partition_get(iterador);
  esp_partition_iterator_release(iterador);  

  if(esp_ota_set_boot_partition(particion)==ESP_OK) return true;
  return false;
  }

void arranqueLoader(void){
  inicializaDetector(true);
  if(modoDetector(true)){
    Serial.printf("Modo loader activado.\n");
    if (setParticionProximoArranque(PARTICION_LOADER)) {
      Serial.printf("Reiniciando.......\n");
      delay(100);
      ESP.restart();
    }
    else Serial.printf("No se puede activar la particion del loader (%s)\n",PARTICION_LOADER.c_str());
  }
}

boolean primerArranque(void){
  //Identifico el motivo del arranque
  tipoReset_t tipoReset=HARDRESET;
  for(int8_t core=0;core<NUM_CORES;core++) {
    //Serial.printf("Motivo del reinicio (%i): %s\n",core,reset_reason(rtc_get_reset_reason(core)));  
    if(rtc_get_reset_reason(core)==DEEPSLEEP_RESET) {
      tipoReset=DESPERTAR;
      break;
    }
  }

  //Segun el tipo de reinicio, si es el primer inicio devuelve true, sino false
  switch (tipoReset){
    case DESPERTAR:
      //Print the wakeup reason for ESP32
      Serial.printf("Wakeup reset: %s\n",wakeup_reason());
      ++bootCount;
      return false;
      break;
    case HARDRESET:
    case SOFTRESET: 
      Serial.printf("Hard o Soft reset\n");
      bootCount=1;
      return true;
      break;
    default:
      return true; //por defecto que lea del fichero
  }
}

void inicializa(boolean deFichero){
  //Init configuracion
  configuracion.leeConfiguracion(deFichero);  
  //Init sensonica
  sensores.inicializa(false);  
  //init Lora
  Lora.inicializa();  
}
/********************************** Fin Inicializacion ********************************************/

void setup() {
  //Init serial, lo unico que hago fuera de los tipos de setup
  Serial.begin(115200); 

  //0.-Arranque loader. Reviso si hay que arrancar en modo loader. Si es asi, se reinicia
  arranqueLoader();

  //1.-Despierta
  inicializa(primerArranque());

  Serial.println("Boot number: " + String(bootCount));

  //2.-mide
  sensores.lee();

  if(bootCount % LECTURAS_POR_ENVIO==0){
    bootCount=0;//Aqui es cero porque el siguiente arranque lo pondra a uno. Lo inicializo a 1 en el setupTotal, porque es ya el primer arranque
    //3.-procesa la medida
    String cad=sensores.generaJsonEstado();

    //4.-comunica
    Serial.printf("%s\n",cad.c_str());
    Lora.envia(cad);

    //5.-procesa respuesta
    //To Do....
  }
  else{Serial.printf("No hay envio (bootCount: %i | LECTURAS_POR_ENVIO: %i)\n",bootCount,LECTURAS_POR_ENVIO);}

  //6.-duerme
  Serial.printf("milis fin: %lu ms\ntimeToSleep: %lld us\ntiempo consumido: %lld us",millis(),timeToSleep,timeToSleep-esp_timer_get_time());    

  esp_deep_sleep(timeToSleep-esp_timer_get_time());

  Serial.println("This will never be printed");  
}

void loop() {}

