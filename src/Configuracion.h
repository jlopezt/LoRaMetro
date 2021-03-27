/*******************************************/
/*                                         */
/*  Include general para todo el proyecto  */
/*                                         */
/*******************************************/

/***************************** Defines *****************************/
#ifndef _CONFIGURACION_
#define _CONFIGURACION_

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

class Configuracion{
    private:
        //miembros        
        boolean valida;
        
        String nombre_dispositivo;
        String tipoSensorTemperatura;
        String tipoSensorHumedad;
        String tipoSensorPresion;
        String tipoSensorLuz;
        
        boolean debug=true;

        boolean parseaConfiguracion(String contenido);

    public:
        //metodos inicializacion
        Configuracion(void);
        boolean leeConfiguracion(boolean fichero);
        //metodos get
        String getNombreDispositivo(void);
        String getTipoSensorTemperatura(void);
        String getTipoSensorHumedad(void);
        String getTipoSensorPresion(void);
        String getTipoSensorLuz(void);
};

extern Configuracion configuracion;
#endif