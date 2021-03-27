/*****************************************************/
/*                                                   */
/* Medida de temperatura con LM35 para el termostato */
/*                                                   */
/*                                                   */
/*****************************************************/
/***************************** Defines *****************************/
#define MAX_INTENTOS_MEDIDA    5  //Numero de intentos maximos de leer del Dallas
#define HDC_DIRECCION_I2C    0x40 //Direccion I2C del HDC1080
#define BME280_DIRECCION_I2C 0x76 //Direccion I2C del HDC1080
#define SEALEVELPRESSURE_HPA 1024 //Presion a nivel del mar
#define BH1750_FONDO_ESCALA  3800 //Fondo de escala del sensor BH1750

//Tipos de sensores
#define TIPO_NULO    "NULO"
#define TIPO_DS18B20 "DS18B20"  //Temperatura
#define TIPO_HDC1080 "HDC1080"  //Temperatura, Humedad
#define TIPO_DHT22   "DHT22"    //Temperatura, Humedad
#define TIPO_BME280  "BME280"   //Temperatura, Humedad
#define TIPO_GL5539  "GL5539"   //Luz
#define TIPO_BH1750  "BH1750"   //Luz
/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Global.h>
#include <Sensores.h>
#include <Ficheros.h>
/***************************** Includes *****************************/

/******************* Variables ***********************/
struct datos_s{
  float temperatura;
  float presion;
  float humedad;
  float luz;
};
RTC_DATA_ATTR struct datos_s datos[LECTURAS_POR_ENVIO];
RTC_DATA_ATTR int16_t indice=0;

Sensores sensores(DHTPIN,ONE_WIRE_BUS);

Sensores::Sensores(uint8_t _dth_pin, uint8_t _one_wire_pin):
    dht(_dth_pin, DHT22),
    oneWire(_one_wire_pin),
    DS18B20(&oneWire) { 
    //Valores pòr defecto
    tipoSensorTemperatura=TIPO_NULO;
    tipoSensorHumedad=TIPO_NULO;
    tipoSensorLuz=TIPO_NULO;
    tipoSensorPresion=TIPO_NULO; 

    tempC=-100; //se declara global 
    humedad=-1;
    luz=-1;
    presion=-1;
    altitud=-1;
}
/******************* Variables ***********************/

boolean Sensores::inicializa(boolean configFichero) {
/*
  //recupero datos del fichero de configuracion
  String cad="";
  if (configFichero){
     Serial.println("Recupero configuracion de archivo...");
  
    if(leeFichero(SENSORES_CONFIG_FILE, cad)){
      if (!parseaConfiguracion(cad)) {
        //Algo salio mal, confgiguracion por defecto
        Serial.printf("No existe fichero de configuracion de Sensores o esta corrupto\n");
        return false;
      }
    }
    else{
      Serial.printf("No existe fichero de configuracion de Sensores o esta corrupto\n");
      return false;
    }
  }

  //Cargo en las variable locales con lo leido del fichero en el primer aranque

  tipoSensorTemperatura=String(configuracion.tipoSensorTemperatura);
  tipoSensorHumedad=String(configuracion.tipoSensorHumedad);
  tipoSensorPresion=String(configuracion.tipoSensorPresion);
  tipoSensorLuz=String(configuracion.tipoSensorLuz);
  */
  tipoSensorTemperatura=configuracion.getTipoSensorTemperatura();
  tipoSensorHumedad=configuracion.getTipoSensorHumedad();
  tipoSensorPresion=configuracion.getTipoSensorPresion();
  tipoSensorLuz=configuracion.getTipoSensorLuz();

  //Inicializo los sensores ¿TODOS?
  //Temperatura
  Serial.printf("Inicializa temperatura...\n");
  if(tipoSensorTemperatura==TIPO_NULO);
  else if(tipoSensorHumedad==TIPO_DHT22) dht.begin();                             //Temperatura y Humedad DHT22
  else if(tipoSensorTemperatura==TIPO_HDC1080) hdc1080.begin(HDC_DIRECCION_I2C);  //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorTemperatura==TIPO_DS18B20) DS18B20.begin();                   //Temperatura Dallas DS18B20
  else if(tipoSensorTemperatura==TIPO_BME280) bme280.begin(BME280_DIRECCION_I2C); //Temperatura bme280
  //Humedad
  Serial.printf("Inicializa humedad...\n");  
  if(tipoSensorHumedad==TIPO_NULO);
  else if(tipoSensorHumedad==TIPO_DHT22  ) dht.begin();                      //Humedad DHT22
  else if(tipoSensorHumedad==TIPO_HDC1080) hdc1080.begin(HDC_DIRECCION_I2C); //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorHumedad==TIPO_BME280) bme280.begin(BME280_DIRECCION_I2C); //Humedad bme280
  //Luz
  //No es necesaria la inicialización
  Serial.printf("Inicializa luz...\n");  
  if(tipoSensorLuz==TIPO_NULO);
  else if(tipoSensorLuz==TIPO_GL5539); //LDR, no se inicializa. Lectura analogica
  else if(tipoSensorLuz==TIPO_BH1750) bh1750.begin(BH1750::CONTINUOUS_LOW_RES_MODE ); //I2C luz bh1750
  //Presion
  Serial.printf("Inicializa presion...\n");
  if(tipoSensorPresion==TIPO_NULO);
  else if(tipoSensorPresion==TIPO_BME280) bme280.begin(BME280_DIRECCION_I2C); //Humedad bme280
  //Altitud
  //No es necesaria la inicialización, si hay es porque hay presion  
  Serial.printf("Fin de la inicailzacion de los sensores\n");  
  return true;
}
    
/*********************************************/
/* Parsea el json leido del fichero de       */
/* configuracio de sensores                  */
/*********************************************
boolean Sensores::parseaConfiguracion(String contenido)
  {  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(contenido.c_str());
  //json.printTo(Serial);
  if (json.success()) 
    {
    Serial.println("parsed json");
    String cad="";
// ******************************Parte especifica del json a leer********************************
    if (json.containsKey("tipoSensorTemperatura")){
      Serial.println("Temperatura\n");
      cad=json.get<String>("tipoSensorTemperatura");
      strncpy(configuracion.tipoSensorTemperatura,cad.c_str(),LONG_TIPO_SENSOR_TEMPERATURA-1);
    }
    else configuracion.tipoSensorTemperatura[0]=0;

    if (json.containsKey("tipoSensorHumedad")){
      Serial.println("Humedad\n");
      cad=json.get<String>("tipoSensorHumedad");
      strncpy(configuracion.tipoSensorHumedad,cad.c_str(),LONG_TIPO_SENSOR_HUMEDAD-1);
    }
    else configuracion.tipoSensorHumedad[0]=0;

    if (json.containsKey("tipoSensorPresion")){
      Serial.println("Presion\n");
      cad=json.get<String>("tipoSensorPresion");
      strncpy( configuracion.tipoSensorPresion,cad.c_str(),LONG_TIPO_SENSOR_PRESION-1);
    }
    else configuracion.tipoSensorPresion[0]=0;

    if (json.containsKey("tipoSensorLuz")){
      Serial.println("Luz\n");
      cad=json.get<String>("tipoSensorLuz");
      strncpy(configuracion.tipoSensorLuz,cad.c_str(),LONG_TIPO_SENSOR_LUZ-1);
    }
    else configuracion.tipoSensorLuz[0]=0;

    Serial.printf("Configuracion leida:\ntipo sensor temperatura: %s\ntipo sensor humedad: %s\ntipo sensor luz: %s\ntipo sensor presion: %s\n",tipoSensorTemperatura.c_str(),tipoSensorHumedad.c_str(),tipoSensorLuz.c_str(),tipoSensorPresion.c_str());
// ************************************************************************************************
    return true;
    }
  return false;
  }
*/
/*********************************************** Lectura de las medidas de los sensores *****************************************************/
/**************************************/
/* Lee los sensores                   */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::lee(void)
  { 
  //Leo los sensores  
  //Temperatura
  if(tipoSensorTemperatura==TIPO_NULO);
  else if(tipoSensorHumedad==TIPO_DHT22) leeTemperaturaDHT22();       //Humedad DHT22
  else if(tipoSensorTemperatura==TIPO_HDC1080) leeTemperaturaHDC1080(); //I2C Temperatura HDC1080
  else if(tipoSensorTemperatura==TIPO_DS18B20) leeTemperaturaDS18B20(); //Temperatura Dallas DS18B20
  else if(tipoSensorTemperatura==TIPO_BME280 ) leeTemperaturaBME280(); //Temperatura BME280
  //Humedad
  if(tipoSensorHumedad==TIPO_NULO);
  else if(tipoSensorHumedad==TIPO_HDC1080) leeHumedadHDC1080(); //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorHumedad==TIPO_DHT22  ) leeHumedadDHT22();   //Humedad DHT22
  else if(tipoSensorHumedad==TIPO_BME280 ) leeHumedadBME280();  //Temperatura BME280
  //Luz
  if(tipoSensorLuz==TIPO_NULO);
  else if(tipoSensorLuz==TIPO_GL5539) leeLuzGL5539(); //LDR
  else if(tipoSensorLuz==TIPO_BH1750) leeLuzBH1750(); //I2C Luz BH1750
  //Presion
  if(tipoSensorPresion==TIPO_NULO);
  else if(tipoSensorPresion==TIPO_BME280) leePresionBME280(); //I2C Temperatura y Humedad HDC1080

  datos[indice].temperatura=getTemperatura();
  datos[indice].humedad=getHumedad();
  datos[indice].presion=getPresion();
  datos[indice].luz=getLuz();

  indice=(indice+1) % LECTURAS_POR_ENVIO;

  Serial.printf("T: %s; H: %s, P: %s, L: %s\n",getTemperaturaString().c_str(),getHumedadString().c_str(),getPresionString().c_str(), getLuzString().c_str());
  }

/********************************* funciones de lectura *********************************/
/**************************************/
/* Lee el sensor de Tª DS18B20        */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leeTemperaturaDS18B20(void)
  { 
  int8_t i=0;//hago como mucho MAX_INTENTOS_MEDIDA
  
  do 
    {
    DS18B20.requestTemperatures(); 
    tempC = DS18B20.getTempCByIndex(0);
    if(tempC != 85.0 && tempC != (-127.0)) break;
    delay(100);
    } while (i++<MAX_INTENTOS_MEDIDA);
  }


/**************************************/
/* Lee el sensor de Tª DTH22          */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leeTemperaturaDHT22(void)
  {
  float t;
  
  t = dht.readTemperature();  //leo el sensor
  if(!isnan(t)) tempC=t;  //si no es nan lo guardo
  }
  
/**************************************/
/* Lee el sensor de Tª HDC1080        */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leeTemperaturaHDC1080(void)
  { 
  tempC = hdc1080.readTemperature();  
  }
  
/**************************************/
/* Lee el sensor de Tª BME280         */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leeTemperaturaBME280(void)
  { 
  tempC = bme280.readTemperature();  
  }

/**************************************/
/* Lee el sensor de Humedad           */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leeHumedadDHT22(void)
  { 
  // Lee el valor desde el sensor
  float h;
  
  h = dht.readHumidity();  //leo el sensor
  if(!isnan(h)) humedad=h;  //si no es nan lo guardo
  }

/**************************************/
/* Lee el sensor de Humedad HDC1080   */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leeHumedadHDC1080(void)
  { 
  humedad = hdc1080.readHumidity();
  }

/**************************************/
/* Lee el sensor de Humedad BME280    */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leeHumedadBME280(void)
  { 
  humedad = bme280.readHumidity();
  }

/**************************************/
/* Lee el sensor de luz GL5539        */
/* y almnacena el valor leido         */
/* valor entre 0 y 100.               */
/* 100 luz intensa 0 oscuridad        */
/**************************************/
void Sensores::leeLuzGL5539(void)
  { 
  // Lee el valor desde el sensor
  luz=(analogRead(LDR_PIN)*100/1024);//valor entre 0 y 100. 100 luz intensa 0 oscuridad
  }
  
/*****************************************/
/* Lee el sensor de luz GY-302 - BH1750 */
/* y almnacena el valor leido           */
/* valor entre 0 y 100.                 */
/* 100 luz intensa 0 oscuridad          */
/****************************************/
void Sensores::leeLuzBH1750(void)
  { 
  // Lee el valor desde el sensor
  luz = bh1750.readLightLevel()*100.0/BH1750_FONDO_ESCALA;//fondo de escala tomo BH1750_FONDO_ESCALA, es aun mayor pero eso es mucha luz. responde entre 0 y 100
  if(luz>100) luz=100; //si es mayor topo 100
  //valor entre 0 y 100. 100 luz intensa 0 oscuridad
  }

/**************************************/
/* Lee el sensor de Presion BME280    */
/* y almnacena el valor leido         */
/**************************************/
void Sensores::leePresionBME280(void)
  { 
  presion = bme280.readPressure()/100.0;
  altitud = bme280.readAltitude(SEALEVELPRESSURE_HPA);
  }
/********************************* Fin funciones de lectura *********************************/
/********************************* funciones get *********************************/
/**************************************/
/* Publica el valor de la Tª medida   */
/**************************************/
float Sensores::getTemperatura(void) {return tempC;}
/********************************************/
/* Publica el valor de la humedada medida   */
/********************************************/
float Sensores::getHumedad(void){return humedad;}
/**************************************/
/* Publica el valor de la luz medida  */
/**************************************/
float Sensores::getLuz(void){return luz;}

/******************************************/
/* Publica el valor de la presion medida  */
/******************************************/
float Sensores::getPresion(void){return presion;}

/******************************************/
/* Publica el valor de la altitud medida  */
/******************************************/
float Sensores::getAltitud(void){return altitud;}

/**************************************/
/* Publica el valor de la Tª medida   */
/**************************************/
String Sensores::getTemperaturaString(void){
  char salida[7];//"-999.9
  dtostrf(tempC, 2, 1, salida);  

  return String(salida);
}
  
/********************************************/
/* Publica el valor de la humedada medida   */
/********************************************/
String Sensores::getHumedadString(void) {
  char salida[7];//"-999.9
  dtostrf(humedad, 2, 1, salida);  

  return String(salida);
}

/**************************************/
/* Publica el valor de la luz medida  */
/**************************************/
String Sensores::getLuzString(void) {
  char salida[7];//"-999.9
  dtostrf(luz, 2, 1, salida);  

  return String(salida);
}

/******************************************/
/* Publica el valor de la presion medida  */
/******************************************/
String Sensores::getPresionString(void) {
  char salida[7];//"-999.9
  dtostrf(presion, 2, 1, salida);  

  return String(salida);
}

/******************************************/
/* Publica el valor de la altitud medida  */
/******************************************/
String Sensores::getAltitudString(void) {
  char salida[7];//"-999.9
  dtostrf(presion, 2, 1, salida);  

  return String(salida);
}

/***************************************/
/* Genera el json con las medidas      */
/***************************************/
String Sensores::generaJsonEstado(void) {
  String cad="";
  
  //genero el json con las medidas--> {"id": 1, "temperatura": 22.5, "Humedad": 63, "Luz": 12, "Presion": 1036.2, "Altitud": 645.2}
  cad = "{\"titulo\": \"";
  cad += String(configuracion.getNombre_dispositivo());
  cad += "\"";
  cad += ",\"Temperatura\": ";
  cad += String(promediaTemperatura(),1);
  cad += ",\"Humedad\": ";
  cad += String(promediaHumedad(),1);
  cad += ",\"Luz\": ";
  cad += String(promediaLuz(),1);
  cad += ",\"Presion\": ";
  cad += String(promediaPresion(),1);
  cad += "}";  

  return cad;
}

float Sensores::promediaTemperatura(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].temperatura;
  }
  return promedio/LECTURAS_POR_ENVIO;
}

float Sensores::promediaHumedad(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].humedad;
  }
  return promedio/LECTURAS_POR_ENVIO;
}

float Sensores::promediaPresion(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].presion;
  }
  return promedio/LECTURAS_POR_ENVIO;
}

float Sensores::promediaLuz(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].luz;
  }
  return promedio/LECTURAS_POR_ENVIO;
}
