/***************************** Defines *****************************/

/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Configuracion.h>
#include <Ficheros.h>
/***************************** Includes *****************************/

/***************************** Variables *****************************/
struct configuracion_s{
  char nombre_dispositivo[16];  
  char tipoSensorTemperatura[LONG_TIPO_SENSOR_TEMPERATURA];
  char tipoSensorHumedad[LONG_TIPO_SENSOR_HUMEDAD];
  char tipoSensorPresion[LONG_TIPO_SENSOR_PRESION];
  char tipoSensorLuz[LONG_TIPO_SENSOR_LUZ];
};
RTC_DATA_ATTR struct configuracion_s config;

Configuracion configuracion;
/***************************** Variables *****************************/

/************************************************/
/* Recupera los datos de configuracion          */
/* del archivo global                           */
/************************************************/
Configuracion::Configuracion(void){
    valida=false;
    debug=true;
    }

boolean Configuracion::leeConfiguracion(boolean leerDeFichero){
  String cad="";
  if (leerDeFichero){//Leo la configuracion del fichero a la memoria persistente
    if (debug) Traza.mensaje("Recupero configuracion de archivo...\n");
    
    //Inicializo el sistema de ficheros
    inicializaFicheros(debug);

    if(!leeFichero(GLOBAL_CONFIG_FILE, cad)) {Serial.printf("Error al leer el fichero\n");return false;}
    if(!parseaConfiguracion(cad)) {Serial.printf("Error al parsear configuracion\n");return false;}
  }
  
  //recupero de memoria persistente a los miembros de la clase
  if (debug) Traza.mensaje("Recupero configuracion de la memoria persisitente...\n");
  nombre_dispositivo=String(config.nombre_dispositivo);
  tipoSensorTemperatura=String(config.tipoSensorTemperatura);
  tipoSensorHumedad=String(config.tipoSensorHumedad);
  tipoSensorPresion=String(config.tipoSensorPresion);
  tipoSensorLuz=String(config.tipoSensorLuz);
  valida=true;

  return valida;
}

/******************************************************/
/* Parsea el json leido del fichero de configuracion  */
/*  auto date = obj.get<char*>("date");
    auto high = obj.get<int>("high");
    auto low = obj.get<int>("low");
    auto text = obj.get<char*>("text");
 ******************************************************/
boolean Configuracion::parseaConfiguracion(String contenido)
  {  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(contenido.c_str());
  
  if (json.success()) 
    {
    Traza.mensaje("parsed json\n");
//******************************Configuracion general********************************
    if (json.containsKey("nombre_dispositivo")) strcpy(config.nombre_dispositivo,(const char *)json["nombre_dispositivo"]);
    if(nombre_dispositivo==NULL) strcpy(config.nombre_dispositivo,NOMBRE_FAMILIA);    
    
    Traza.mensaje("Configuracion leida:\nNombre dispositivo: %s\n",config.nombre_dispositivo);
//************************************************************************************************

//******************************Configuracion de sensores********************************
    String cad="";

    if (json.containsKey("tipoSensorTemperatura")){
      cad=json.get<String>("tipoSensorTemperatura");
      //Serial.printf("Temperatura: %s\n",cad.c_str());
      strncpy(config.tipoSensorTemperatura,cad.c_str(),LONG_TIPO_SENSOR_TEMPERATURA-1);
    }
    else config.tipoSensorTemperatura[0]=0;

    if (json.containsKey("tipoSensorHumedad")){
      cad=json.get<String>("tipoSensorHumedad");
      //Serial.printf("Humedad: %s\n",cad.c_str());
      strncpy(config.tipoSensorHumedad,cad.c_str(),LONG_TIPO_SENSOR_HUMEDAD-1);
    }
    else config.tipoSensorHumedad[0]=0;

    if (json.containsKey("tipoSensorPresion")){
      cad=json.get<String>("tipoSensorPresion");
      //Serial.printf("Presion: %s\n",cad.c_str());
      strncpy(config.tipoSensorPresion,cad.c_str(),LONG_TIPO_SENSOR_PRESION-1);
    }
    else config.tipoSensorPresion[0]=0;

    if (json.containsKey("tipoSensorLuz")){
      cad=json.get<String>("tipoSensorLuz");
      //Serial.printf("Luz: %s\n",cad.c_str());
      strncpy(config.tipoSensorLuz,cad.c_str(),LONG_TIPO_SENSOR_LUZ-1);
    }
    else config.tipoSensorLuz[0]=0;

    Serial.printf("Configuracion leida:\ntipo sensor temperatura: %s\ntipo sensor humedad: %s\ntipo sensor luz: %s\ntipo sensor presion: %s\n", config.tipoSensorTemperatura, config.tipoSensorHumedad, config.tipoSensorLuz, config.tipoSensorPresion);
//************************************************************************************************
    return true;
    }
  Serial.printf("Error al parsear el json\n");  
  return false;  
  }

/*************************FUNCIONES GET************************************/
String Configuracion::getNombreDispositivo(void){
    if(valida) return String(nombre_dispositivo);
    return String("Invalida");
}

String Configuracion::getTipoSensorTemperatura(void){
    if(valida) return String(tipoSensorTemperatura);
    return String("Invalida");
}

String Configuracion::getTipoSensorHumedad(void){
    if(valida) return String(tipoSensorHumedad);
    return String("Invalida");
}

String Configuracion::getTipoSensorPresion(void){
    if(valida) return String(tipoSensorPresion);
    return String("Invalida");
}

String Configuracion::getTipoSensorLuz(void){
    if(valida) return String(tipoSensorLuz);
    return String("Invalida");
}
/*************************FUNCIONES GET************************************/