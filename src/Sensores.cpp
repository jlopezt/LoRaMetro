/*****************************************************/
/*                                                   */
/* Medida de temperatura con LM35 para el termostato */
/*                                                   */
/*                                                   */
/*****************************************************/
/***************************** Defines *****************************/
#define MAX_INTENTOS_MEDIDA    5  //Numero de intentos maximos de leer del Dallas
#define HDC_DIRECCION_I2C    0x40 //Direccion I2C del HDC1080
#define BME280_DIRECCION_I2C 0x76 //Direccion I2C del BME280
#define BMP280_DIRECCION_I2C 0x76 //Direccion I2C del BMP280
#define SEALEVELPRESSURE_HPA 1024 //Presion a nivel del mar
#define BH1750_FONDO_ESCALA  3800 //Fondo de escala del sensor BH1750
#define SECO                  250 //Lectura del sensor en vacio
#define MOJADO                750 //Lectura del sensor en vaso de agua

//Tipos de sensores
#define TIPO_NULO                     "NULO"
#define TIPO_DS18B20                  "DS18B20"  //Temperatura
#define TIPO_HDC1080                  "HDC1080"  //Temperatura, Humedad
#define TIPO_DHT22                    "DHT22"    //Temperatura, Humedad
#define TIPO_BME280                   "BME280"   //Temperatura, Humedad y Presion
#define TIPO_BMP280                   "BMP280"   //Temperatura, Presion
#define TIPO_GL5539                   "GL5539"   //Luz
#define TIPO_BH1750                   "BH1750"   //Luz
#define TIPO_SOILMOISTURECAPACITIVEV2 "SOILMOISTUREV2" //Humedad del suelo   
/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Global.h>
#include <Sensores.h>
#include <Configuracion.h>
/***************************** Includes *****************************/

/******************* Variables ***********************/
struct datos_s{
  float temperaturaAire;
  float presion;
  float humedad;
  float luz;
  float altitud;
  float temperaturaSuelo;
  float humedadSuelo;
};
RTC_DATA_ATTR struct datos_s datos[LECTURAS_POR_ENVIO];
RTC_DATA_ATTR int16_t indice=0;

Sensores sensores(DHTPIN,ONE_WIRE_BUS);

Sensores::Sensores(uint8_t _dth_pin, uint8_t _one_wire_pin):
    dht(_dth_pin, DHT22),
    oneWire(_one_wire_pin),
    DS18B20(&oneWire) { 
    //Valores por defecto
    tipoSensorTemperaturaAire=TIPO_NULO;
    tipoSensorHumedad=TIPO_NULO;
    tipoSensorLuz=TIPO_NULO;
    tipoSensorPresion=TIPO_NULO; 
    tipoSensorAltitud=TIPO_NULO;
    tipoSensorTemperaturaSuelo=TIPO_NULO;
    tipoSensorHumedadSuelo=TIPO_NULO;

    tempAire=-100; //se declara global 
    humedad=-1;
    luz=-1;
    presion=-1;
    altitud=-1;
    tempSuelo=-101; //se declara global 
    humedadSuelo=-1;
}
/******************* Variables ***********************/

boolean Sensores::inicializa(boolean configFichero) {
  //Cargo los valores de la memoria persistente
  tipoSensorTemperaturaAire=configuracion.getTipoSensorTemperaturaAire();
  tipoSensorHumedad=configuracion.getTipoSensorHumedad();
  tipoSensorPresion=configuracion.getTipoSensorPresion();
  tipoSensorLuz=configuracion.getTipoSensorLuz();
  tipoSensorAltitud=configuracion.getTipoSensorAltitud();
  tipoSensorTemperaturaSuelo=configuracion.getTipoSensorTemperaturaSuelo();
  tipoSensorHumedadSuelo=configuracion.getTipoSensorHumedadSuelo();

  //Inicializo los sensores
  //Temperatura
  //Serial.printf("Inicializa temperatura...\n");
  if(tipoSensorTemperaturaAire==TIPO_NULO);
  else if(tipoSensorTemperaturaAire==TIPO_DHT22) dht.begin();                             //Temperatura y Humedad DHT22
  else if(tipoSensorTemperaturaAire==TIPO_HDC1080) hdc1080.begin(HDC_DIRECCION_I2C);  //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorTemperaturaAire==TIPO_DS18B20) DS18B20.begin();                   //Temperatura Dallas DS18B20
  else if(tipoSensorTemperaturaAire==TIPO_BME280) bme280.begin(BME280_DIRECCION_I2C); //Temperatura bme280
  else if(tipoSensorTemperaturaAire==TIPO_BMP280) bmp280.begin(BMP280_DIRECCION_I2C); //Temperatura bme280
  
  //Humedad
  //Serial.printf("Inicializa humedad...\n");  
  if(tipoSensorHumedad==TIPO_NULO);
  else if(tipoSensorHumedad==TIPO_DHT22  ) dht.begin();                      //Humedad DHT22
  else if(tipoSensorHumedad==TIPO_HDC1080) hdc1080.begin(HDC_DIRECCION_I2C); //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorHumedad==TIPO_BME280) bme280.begin(BME280_DIRECCION_I2C); //Humedad bme280
  //Luz
  //No es necesaria la inicialización
  //Serial.printf("Inicializa luz...\n");  
  if(tipoSensorLuz==TIPO_NULO);
  else if(tipoSensorLuz==TIPO_GL5539); //LDR, no se inicializa. Lectura analogica
  else if(tipoSensorLuz==TIPO_BH1750) bh1750.begin(BH1750::CONTINUOUS_LOW_RES_MODE ); //I2C luz bh1750
  //Presion
  //Serial.printf("Inicializa presion...\n");
  if(tipoSensorPresion==TIPO_NULO);
  else if(tipoSensorPresion==TIPO_BME280) bme280.begin(BME280_DIRECCION_I2C); //Humedad bme280
  else if(tipoSensorPresion==TIPO_BMP280) bmp280.begin(BMP280_DIRECCION_I2C); //Humedad bme280
  //Altitud
  //No es necesaria la inicialización, si hay es porque hay presion  
  //Serial.printf("Fin de la inicailzacion de los sensores\n");  
  if(tipoSensorTemperaturaSuelo==TIPO_NULO);
  else if(tipoSensorTemperaturaSuelo==TIPO_DHT22) dht.begin();                             //Temperatura y Humedad DHT22
  else if(tipoSensorTemperaturaSuelo==TIPO_HDC1080) hdc1080.begin(HDC_DIRECCION_I2C);  //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorTemperaturaSuelo==TIPO_DS18B20) DS18B20.begin();                   //Temperatura Dallas DS18B20
  else if(tipoSensorTemperaturaSuelo==TIPO_BME280) bme280.begin(BME280_DIRECCION_I2C); //Temperatura bme280
  else if(tipoSensorTemperaturaSuelo==TIPO_BMP280) bmp280.begin(BMP280_DIRECCION_I2C); //Temperatura bme280
  //Humedad del suelo
  if (tipoSensorHumedadSuelo==TIPO_SOILMOISTURECAPACITIVEV2); //Sensor capacitivo V2 no requiere inicializacion

  return true;
}
    
/*********************************************** Lectura de las medidas de los sensores *****************************************************/
/***********************************************/
/* Lee los sensores y almnacena el valor leido */
/***********************************************/
void Sensores::lee(void)
  { 
  //Leo los sensores  
  //Temperatura
  if(tipoSensorTemperaturaAire==TIPO_NULO);
  else if(tipoSensorTemperaturaAire==TIPO_DHT22) tempAire = leeTemperaturaDHT22();       //Humedad DHT22
  else if(tipoSensorTemperaturaAire==TIPO_HDC1080) tempAire = leeTemperaturaHDC1080(); //I2C Temperatura HDC1080
  else if(tipoSensorTemperaturaAire==TIPO_DS18B20) tempAire = leeTemperaturaDS18B20(); //Temperatura Dallas DS18B20
  else if(tipoSensorTemperaturaAire==TIPO_BME280 ) tempAire = leeTemperaturaBME280(); //Temperatura BME280
  else if(tipoSensorTemperaturaAire==TIPO_BMP280 ) tempAire = leeTemperaturaBMP280(); //Temperatura BME280
  //Humedad
  if(tipoSensorHumedad==TIPO_NULO);
  else if(tipoSensorHumedad==TIPO_HDC1080) humedad = leeHumedadHDC1080(); //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorHumedad==TIPO_DHT22  ) humedad = leeHumedadDHT22();   //Humedad DHT22
  else if(tipoSensorHumedad==TIPO_BME280 ) humedad = leeHumedadBME280();  //Temperatura BME280
  //Luz
  if(tipoSensorLuz==TIPO_NULO);
  else if(tipoSensorLuz==TIPO_GL5539) luz= leeLuzGL5539(); //LDR
  else if(tipoSensorLuz==TIPO_BH1750) luz = leeLuzBH1750(); //I2C Luz BH1750
  //Presion
  if(tipoSensorPresion==TIPO_NULO);
  else if(tipoSensorPresion==TIPO_BME280) presion = leePresionBME280(); //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorPresion==TIPO_BMP280) presion = leePresionBMP280(); //I2C Temperatura y Humedad HDC1080
  //Altitud
  if(tipoSensorAltitud==TIPO_NULO);
  else if(tipoSensorAltitud==TIPO_BME280) altitud = leeAltitudBME280(); //I2C Temperatura y Humedad HDC1080
  else if(tipoSensorAltitud==TIPO_BMP280) altitud = leeAltitudBMP280(); //I2C Temperatura y Humedad HDC1080
  //Temperatura Suelo
  if(tipoSensorTemperaturaSuelo==TIPO_NULO);
  else if(tipoSensorTemperaturaSuelo==TIPO_DHT22) tempSuelo = leeTemperaturaDHT22();       //Humedad DHT22
  else if(tipoSensorTemperaturaSuelo==TIPO_HDC1080) tempSuelo = leeTemperaturaHDC1080(); //I2C Temperatura HDC1080
  else if(tipoSensorTemperaturaSuelo==TIPO_DS18B20) tempSuelo = leeTemperaturaDS18B20(); //Temperatura Dallas DS18B20
  else if(tipoSensorTemperaturaSuelo==TIPO_BME280 ) tempSuelo = leeTemperaturaBME280(); //Temperatura BME280
  else if(tipoSensorTemperaturaSuelo==TIPO_BMP280 ) tempSuelo = leeTemperaturaBMP280(); //Temperatura BME280  
  //Humedad del suelo
  if(tipoSensorHumedadSuelo==TIPO_SOILMOISTURECAPACITIVEV2) humedadSuelo = leeHumedadSueloCapacitivo();//Sensor capacitivo V2

  datos[indice].temperaturaAire=getTemperaturaAire();
  datos[indice].humedad=getHumedad();
  datos[indice].presion=getPresion();
  datos[indice].luz=getLuz();
  datos[indice].altitud=getAltitud();
  datos[indice].temperaturaSuelo=getTemperaturaSuelo();
  datos[indice].humedadSuelo=getHumedadSuelo();

  indice=(indice+1) % LECTURAS_POR_ENVIO;

  Serial.printf("T: %s; H: %s, P: %s, L: %s, A: %s, HS: %s\n",getTemperaturaAireString().c_str(),getHumedadString().c_str(),getPresionString().c_str(), getLuzString().c_str(),getAltitudString().c_str(),getHumedadSueloString().c_str());
  }

/********************************* funciones de lectura *********************************/
/**************************************/
/* Lee el sensor de Tª DS18B20        */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeTemperaturaDS18B20(void)
  { 
  int8_t i=0;//hago como mucho MAX_INTENTOS_MEDIDA
  float temporal;
  do 
    {
    DS18B20.requestTemperatures(); 
    temporal = DS18B20.getTempCByIndex(0);
    if(temporal != 85.0 && temporal != (-127.0)) break;
    delay(100);
    } while (i++<MAX_INTENTOS_MEDIDA);

  return temporal;
  }


/**************************************/
/* Lee el sensor de Tª DTH22          */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeTemperaturaDHT22(void){
  float t;
  
  t = dht.readTemperature();  //leo el sensor
  if(!isnan(t)) return t;  //si no es nan lo guardo

  return -100;
}
  
/**************************************/
/* Lee el sensor de Tª HDC1080        */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeTemperaturaHDC1080(void){ return hdc1080.readTemperature();}
  
/**************************************/
/* Lee el sensor de Tª BME280         */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeTemperaturaBME280(void){return bme280.readTemperature();}

/**************************************/
/* Lee el sensor de Tª BMP280         */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeTemperaturaBMP280(void){return bmp280.readTemperature();}

/**************************************/
/* Lee el sensor de Humedad           */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeHumedadDHT22(void){ 
  // Lee el valor desde el sensor
  float h;
  
  h = dht.readHumidity();  //leo el sensor
  if(!isnan(h)) return h;  //si no es nan lo guardo

  return -1;
}

/**************************************/
/* Lee el sensor de Humedad HDC1080   */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeHumedadHDC1080(void){return hdc1080.readHumidity();}

/**************************************/
/* Lee el sensor de Humedad BME280    */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeHumedadBME280(void){return bme280.readHumidity();}

/**************************************/
/* Lee el sensor de luz GL5539        */
/* y almnacena el valor leido         */
/* valor entre 0 y 100.               */
/* 100 luz intensa 0 oscuridad        */
/**************************************/
float Sensores::leeLuzGL5539(void){ return (analogRead(LDR_PIN)*100/1024);}
  
/*****************************************/
/* Lee el sensor de luz GY-302 - BH1750 */
/* y almnacena el valor leido           */
/* valor entre 0 y 100.                 */
/* 100 luz intensa 0 oscuridad          */
/****************************************/
float Sensores::leeLuzBH1750(void){ 
  // Lee el valor desde el sensor
  float temporal = bh1750.readLightLevel()*100.0/BH1750_FONDO_ESCALA;//fondo de escala tomo BH1750_FONDO_ESCALA, es aun mayor pero eso es mucha luz. responde entre 0 y 100
  if(temporal>100) temporal=100; //si es mayor topo 100
  //valor entre 0 y 100. 100 luz intensa 0 oscuridad

  return temporal;
}

/**************************************/
/* Lee el sensor de Presion BME280    */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leePresionBME280(void){ return bme280.readPressure()/100.0;}

/**************************************/
/* Lee el sensor de Presion BMP280    */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leePresionBMP280(void){ return bmp280.readPressure()/100.0;}

/**************************************/
/* Lee el sensor de Presion BME280    */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeAltitudBME280(void){ return bme280.readAltitude(SEALEVELPRESSURE_HPA);}

/**************************************/
/* Lee el sensor de Presion BMP280    */
/* y almnacena el valor leido         */
/**************************************/
float Sensores::leeAltitudBMP280(void){ return bmp280.readAltitude(SEALEVELPRESSURE_HPA);}

/*******************************************/
/* Lee el sensor capacitivo de humedad     */
/* del suelo, la mapea de 0 100% segun la  */
/* calibracion y almacena el valor leido   */
/*******************************************/
float Sensores::leeHumedadSueloCapacitivo(void){
  int sensorVal = analogRead(PIN_SENSOR_HUMEDAD_SUELO);
  return map(sensorVal, MOJADO, SECO, 100, 0);   
}
/********************************* Fin funciones de lectura *********************************/
/********************************* funciones get *********************************/
/**************************************/
/* Publica el valor de la Tª medida   */
/**************************************/
float Sensores::getTemperaturaAire(void) {return tempAire;}

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
float Sensores::getTemperaturaSuelo(void) {return tempSuelo;}

/******************************************/
/* Publica el valor de la altitud medida  */
/******************************************/
float Sensores::getHumedadSuelo(void){return humedadSuelo;}


/**************************************/
/* Publica el valor de la Tª medida   */
/**************************************/
String Sensores::getTemperaturaAireString(void){
  char salida[7];//"-999.9
  dtostrf(tempAire, 2, 1, salida);  

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

/**************************************/
/* Publica el valor de la Tª medida   */
/**************************************/
String Sensores::getTemperaturaSueloString(void){
  char salida[7];//"-999.9
  dtostrf(tempSuelo, 2, 1, salida);  

  return String(salida);
}

/*****************************************************/
/* Publica el valor de la humedad del suelo  medida  */
/*****************************************************/
String Sensores::getHumedadSueloString(void) {
  char salida[7];//"-999.9
  dtostrf(humedadSuelo, 2, 1, salida);  

  return String(salida);
}

/***************************************/
/* Genera el json con las medidas      */
/***************************************/
String Sensores::generaJsonEstado(void) {
  String cad="";
  
  //genero el json con las medidas--> {"id": 1, "temperatura": 22.5, "Humedad": 63, "Luz": 12, "Presion": 1036.2, "Altitud": 645.2}
  cad = "{\"titulo\": \"";
  cad += String(configuracion.getNombreDispositivo());
  cad += "\"";
  cad += ",\"Temperatura aire\": ";
  cad += String(promediaTemperaturaAire(),1);
  cad += ",\"Humedad\": ";
  cad += String(promediaHumedad(),1);
  cad += ",\"Luz\": ";
  cad += String(promediaLuz(),1);
  cad += ",\"Presion\": ";
  cad += String(promediaPresion(),1);
  cad += ",\"Altitud\": ";
  cad += String(promediaAltitud(),1);
  cad += ",\"Temperatura suelo\": ";
  cad += String(promediaTemperaturaSuelo(),1);
  cad += ",\"HumedadSuelo\": ";
  cad += String(promediaHumedadSuelo(),1);
  cad += "}";  

  return cad;
}

float Sensores::promediaTemperaturaAire(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].temperaturaAire;
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

float Sensores::promediaAltitud(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].altitud;
  }
  return promedio/LECTURAS_POR_ENVIO;
}

float Sensores::promediaTemperaturaSuelo(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].temperaturaSuelo;
  }
  return promedio/LECTURAS_POR_ENVIO;
}

float Sensores::promediaHumedadSuelo(void){
  float promedio=0;
  for(uint8_t medida=0; medida<LECTURAS_POR_ENVIO;medida++){
    promedio+=datos[medida].humedadSuelo;
  }
  return promedio/LECTURAS_POR_ENVIO;
}