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
#include <Adafruit_Sensor.h>
#include <BH1750.h>
/***************************** Includes *****************************/

//Defines de pines de los captadores
static const uint8_t ONE_WIRE_BUS = 17; //Pin donde esta el DS18B20
static const uint8_t DHTPIN = 22; //Pin de datos del DTH22
static const uint8_t LDR_PIN = 13; //Pin de la LDR para medir luz

class Sensores{
    private:
        //Sensores
        DHT dht;
        OneWire oneWire;
        DallasTemperature DS18B20;       
        ClosedCube_HDC1080 hdc1080;
        Adafruit_BME280 bme280; // I2C
        BH1750 bh1750; //I2C direccion por defecto 0x23

        //Configuracion
        String tipoSensorTemperatura;
        String tipoSensorHumedad;
        String tipoSensorLuz;
        String tipoSensorPresion;
        //String tipoSensorAltitud; //No es necesario, porque si hay es porque hay presion

        //Valores
        float tempC=-100; //se declara global 
        float humedad=-1;
        float luz=-1;
        float presion=-1;
        float altitud=-1;

        boolean parseaConfiguracion(String contenido);

        /********************************* funciones de lectura *********************************/
        void leeTemperaturaDS18B20(void);
        void leeTemperaturaDHT22(void);
        void leeTemperaturaHDC1080(void);
        void leeTemperaturaBME280(void);
        void leeHumedadDHT22(void);
        void leeHumedadHDC1080(void);
        void leeHumedadBME280(void);
        void leeLuzGL5539(void);
        void leeLuzBH1750(void);
        void leePresionBME280(void);
        /********************************* Fin funciones de lectura *********************************/
        
        float promediaTemperatura(void);
        float promediaHumedad(void);
        float promediaPresion(void);
        float promediaLuz(void);

        //////PARA DEPURACION  
        void setTemp(float f) {tempC=f;}
        void setHum(float f) {humedad=f;}
        void setLuz(float f) {luz=f;}
        void setPresion(float f) {presion=f;}
        void setAltitud(float f) {altitud=f;}  

    public:
        Sensores(uint8_t _dth_pin, uint8_t _one_wire_pin);
        boolean inicializa(boolean configFichero);
        void lee(void);

        /********************************* funciones get *********************************/
        float getTemperatura(void);  //encapsula el acceso a la temperatura
        float getHumedad(void);  //encapsula el acceso a la humedad
        float getLuz(void);  //encapsula el acceso a la luz
        float getPresion(void);  //encapsula el acceso a la presion
        float getAltitud(void);  //encapsula el acceso a la altitud
        String getTemperaturaString(void);  //encapsula el acceso a la temperatura
        String getHumedadString(void);  //encapsula el acceso a la humedad
        String getLuzString(void);  //encapsula el acceso a la luz
        String getPresionString(void);  //encapsula el acceso a la presion
        String getAltitudString(void);  //encapsula el acceso a la altitud
        /********************************* Fin funciones get *********************************/

        String generaJsonEstado(void);        
};

extern Sensores sensores;
#endif