/********************************************************************
 * HDC1080 
 * -Sensor de humedad y temperatura 
 * -Interfaz I2C
 * 
 * BMP280 
 * -Sensor de presión y humedad 
 * -Interfaz I2C y SPI (compatible con 3 cables, 4-cables SPI)
 * 
 * BH1750
 * -Sensor de luz
 * -Interfaz I2C
 * 
 * GY-302
 * -Sensor de luz
 *******************************************************************/

/***************************** Defines *****************************/
#ifndef _SENSORES_
#define _SENSORES_

/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Global.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ClosedCube_HDC1080.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <BH1750.h>
/***************************** Includes *****************************/

//Defines de pines de los captadores
static const uint8_t ONE_WIRE_BUS = 17; //Pin donde esta el DS18B20
static const uint8_t DHTPIN = 23; //Pin de datos del DTH22
static const uint8_t PIN_SENSOR_HUMEDAD_SUELO = 36; //GPIO36 == ADC1_0
static const uint8_t LDR_PIN = 13; //Pin de la LDR para medir luz

class Sensores{
    private:
        //Sensores
        DHT dht;
        OneWire oneWire;
        DallasTemperature DS18B20;       
        ClosedCube_HDC1080 hdc1080;
        Adafruit_BME280 bme280; // I2C
        Adafruit_BMP280 bmp280; // I2C
        BH1750 bh1750; //I2C direccion por defecto 0x23

        //Configuracion
        String tipoSensorTemperaturaAire;
        String tipoSensorHumedad;
        String tipoSensorLuz;
        String tipoSensorPresion;
        String tipoSensorAltitud;
        String tipoSensorTemperaturaSuelo;
        String tipoSensorHumedadSuelo;

        //Valores
        float tempAire=-100; //se declara global 
        float humedad=-1;
        float luz=-1;
        float presion=-1;
        float altitud=-1;
        float tempSuelo=-100; //se declara global 
        float humedadSuelo=-1;

        boolean parseaConfiguracion(String contenido);

        /********************************* funciones de lectura *********************************/
        float leeTemperaturaDS18B20(void);
        float leeTemperaturaDHT22(void);
        float leeTemperaturaHDC1080(void);
        float leeTemperaturaBME280(void);
        float leeTemperaturaBMP280(void);
        float leeHumedadDHT22(void);
        float leeHumedadHDC1080(void);
        float leeHumedadBME280(void);
        float leeLuzGL5539(void);
        float leeLuzBH1750(void);
        float leePresionBME280(void);
        float leePresionBMP280(void);
        float leeAltitudBME280(void);
        float leeAltitudBMP280(void);
        float leeHumedadSueloCapacitivo(void);
        /********************************* Fin funciones de lectura *********************************/
        
        float promediaTemperaturaAire(void);
        float promediaHumedad(void);
        float promediaPresion(void);
        float promediaLuz(void);
        float promediaAltitud(void);
        float promediaTemperaturaSuelo(void);
        float promediaHumedadSuelo(void);

        //////PARA DEPURACION  
        void setTempAire(float f) {tempAire=f;}
        void setHum(float f) {humedad=f;}
        void setLuz(float f) {luz=f;}
        void setPresion(float f) {presion=f;}
        void setAltitud(float f) {altitud=f;}
        void setTempSuelo(float f) {tempSuelo=f;}
        void setHumedadSuelo(float f) {humedadSuelo=f;}

    public:
        Sensores(uint8_t _dth_pin, uint8_t _one_wire_pin);
        boolean inicializa(boolean configFichero);
        void lee(void);

        /********************************* funciones get *********************************/
        float getTemperaturaAire(void);  //encapsula el acceso a la temperatura del aire
        float getHumedad(void);  //encapsula el acceso a la humedad
        float getLuz(void);  //encapsula el acceso a la luz
        float getPresion(void);  //encapsula el acceso a la presion
        float getAltitud(void);  //encapsula el acceso a la altitud
        float getTemperaturaSuelo(void);  //encapsula el acceso a la temperatura del suelo
        float getHumedadSuelo(void);  //encapsula el acceso a la humedad del suelo

        String getTemperaturaAireString(void);  //encapsula el acceso a la temperatura
        String getHumedadString(void);  //encapsula el acceso a la humedad
        String getLuzString(void);  //encapsula el acceso a la luz
        String getPresionString(void);  //encapsula el acceso a la presion
        String getAltitudString(void);  //encapsula el acceso a la altitud
        String getTemperaturaSueloString(void);  //encapsula el acceso a la temperatura
        String getHumedadSueloString(void);  //encapsula el acceso a la humedad dle suelo
        /********************************* Fin funciones get *********************************/

        String generaJsonEstado(void);        
};

extern Sensores sensores;
#endif