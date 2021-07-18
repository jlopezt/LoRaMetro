/***************************** Defines *****************************/

/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Configuracion.h>
#include <Ficheros.h>
/***************************** Includes *****************************/

/***************************** Variables *****************************/
struct configuracion_s{
  char nombre_dispositivo[16];
  char tipoSensorTemperaturaAire[LONG_TIPO_SENSOR_TEMPERATURA];
  char tipoSensorHumedad[LONG_TIPO_SENSOR_HUMEDAD];
  char tipoSensorPresion[LONG_TIPO_SENSOR_PRESION];
  char tipoSensorLuz[LONG_TIPO_SENSOR_LUZ];
  char tipoSensorAltitud[LONG_TIPO_SENSOR_ALTITUD];
  char tipoSensorTemperaturaSuelo[LONG_TIPO_SENSOR_TEMPERATURA];
  char tipoSensorHumedadSuelo[LONG_TIPO_SENSOR_HUMEDADSUELO];

  uint32_t seqnoUp;
  uint32_t seqnoDn;
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

  tipoSensorTemperaturaAire=String(config.tipoSensorTemperaturaAire);
  tipoSensorHumedad=String(config.tipoSensorHumedad);
  tipoSensorPresion=String(config.tipoSensorPresion);
  tipoSensorLuz=String(config.tipoSensorLuz);
  tipoSensorAltitud=String(config.tipoSensorAltitud);
  tipoSensorTemperaturaSuelo=String(config.tipoSensorTemperaturaSuelo);
  tipoSensorHumedadSuelo=String(config.tipoSensorHumedadSuelo);

  //seqnoUp=config.seqnoUp;
  //seqnoDn=config.seqnoDn;

  Serial.printf("tipoSensorTemperaturaAire: %s\ntipoSensorHumedad: %s\ntipoSensorPresion: %s\ntipoSensorLuz: %s\ntipoSensorAltitud: %s\ntipoSensorTemperaturaSuelo: %s\ntipoSensorHumedadSuelo: %s\n",tipoSensorTemperaturaAire.c_str(),tipoSensorHumedad.c_str(),tipoSensorPresion.c_str(),tipoSensorLuz.c_str(),tipoSensorAltitud.c_str(),tipoSensorTemperaturaSuelo.c_str(),tipoSensorHumedadSuelo.c_str());
  Serial.printf("seqnoUp: %i\nseqnoDn: %i\n",config.seqnoUp,config.seqnoDn);
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

    if (json.containsKey("tipoSensorTemperaturaAire")){
      cad=json.get<String>("tipoSensorTemperaturaAire");
      //Serial.printf("Temperatura: %s\n",cad.c_str());
      strncpy(config.tipoSensorTemperaturaAire,cad.c_str(),LONG_TIPO_SENSOR_TEMPERATURA-1);
    }
    else config.tipoSensorTemperaturaAire[0]=0;

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

    if (json.containsKey("tipoSensorAltitud")){
      cad=json.get<String>("tipoSensorAltitud");
      //Serial.printf("Altitud: %s\n",cad.c_str());
      strncpy(config.tipoSensorAltitud,cad.c_str(),LONG_TIPO_SENSOR_ALTITUD-1);
    }
    else config.tipoSensorAltitud[0]=0;

    if (json.containsKey("tipoSensorTemperaturaSuelo")){
      cad=json.get<String>("tipoSensorTemperaturaSuelo");
      //Serial.printf("Temperatura: %s\n",cad.c_str());
      strncpy(config.tipoSensorTemperaturaSuelo,cad.c_str(),LONG_TIPO_SENSOR_TEMPERATURA-1);
    }
    else config.tipoSensorTemperaturaAire[0]=0;

    if (json.containsKey("tipoSensorHumedadSuelo")){
      cad=json.get<String>("tipoSensorHumedadSuelo");
      //Serial.printf("HumedadSuelo: %s\n",cad.c_str());
      strncpy(config.tipoSensorHumedadSuelo,cad.c_str(),LONG_TIPO_SENSOR_HUMEDADSUELO-1);
    }
    else config.tipoSensorHumedadSuelo[0]=0;

    Serial.printf("tipo sensor temperatura aire: %s\ntipo sensor humedad: %s\ntipo sensor luz: %s\ntipo sensor presion: %s\ntipo sensor altitud: %s\ntipo sensor temperatura suelo: %s\ntipo sensor humedad del suelo: %s\n", config.tipoSensorTemperaturaAire, config.tipoSensorHumedad, config.tipoSensorLuz, config.tipoSensorPresion, config.tipoSensorAltitud, config.tipoSensorTemperaturaSuelo, config.tipoSensorHumedadSuelo);
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

String Configuracion::getTipoSensorTemperaturaAire(void){
  if(valida) return String(tipoSensorTemperaturaAire);
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

String Configuracion::getTipoSensorAltitud(void){
  if(valida) return String(tipoSensorAltitud);
  return String("Invalida");
}

String Configuracion::getTipoSensorTemperaturaSuelo(void){
  if(valida) return String(tipoSensorTemperaturaSuelo);
  return String("Invalida");
}

String Configuracion::getTipoSensorHumedadSuelo(void){
  if(valida) return String(tipoSensorHumedadSuelo);
  return String("Invalida");
}

uint32_t Configuracion::getSeqnoUp(void){
  if(valida) return config.seqnoUp;
  return 0;
}

uint32_t Configuracion::getSeqnoDn(void){
  if(valida) return config.seqnoDn;
  return 0;
}
/*************************FUNCIONES GET************************************/
/*************************FUNCIONES SET************************************/
void Configuracion::setSeqnoUp(uint32_t valor){config.seqnoUp=valor;}
void Configuracion::setSeqnoDn(uint32_t valor){config.seqnoDn=valor;}
/*************************FUNCIONES SET************************************/        