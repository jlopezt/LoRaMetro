/**********************************************************************************/
/* ordenes.ino                                                                    */
/*                                                                                */
/* Comandos para el  control a traves del puerto serie                            */
/*                                                                                */
/**********************************************************************************/

/***************************** Defines *****************************/
#define LONG_COMANDO 40
#define LONG_PARAMETRO 30
#define LONG_ORDEN 22 //Comando (espacio) Parametros (fin de cadena)
#define MAX_COMANDOS   35
/***************************** Defines *****************************/

/***************************** Includes *****************************/
#include <Global.h>
#include <Ordenes.h>
#include <Ficheros.h>

#include <SPIFFS.h>
#include <Time.h>
/***************************** Includes *****************************/

typedef struct 
  {
  String comando;
  String descripcion;
  void (*p_func_comando) (int, char*, float)=NULL;
  }tipo_comando;
tipo_comando comandos[MAX_COMANDOS];

char ordenRecibida[LONG_ORDEN]="";
int lonOrden=0;
/*********************************************************************/
int HayOrdenes(int debug)
  {
  char inChar=0;
  
  while (Serial.available())
    {
    inChar=(char)Serial.read(); 
    switch (inChar)
      {
      case ';':
        //Recibido final de orden
        if (debug) Traza.mensaje("Orden recibida: %s\n",ordenRecibida);
        return(1);
        break;
      default:
        //Nuevo caracter recibido. Añado y sigo esperando
        ordenRecibida[lonOrden++]=inChar;
        ordenRecibida[lonOrden]=0;
        break;
      }
    }  
  return(0); //No ha llegado el final de orden
  }

int EjecutaOrdenes(int debug){
  String comando="";
  String parametros="";
  int iParametro=0;
  char sParametro[LONG_PARAMETRO]="";//LONG_PARAMETRO longitud maxmima del parametro
  float fParametro=0;
  int inicioParametro=0;

  if (debug) Traza.mensaje("Orden recibida: %s\n",ordenRecibida);
  
  for(int i=0;i<LONG_COMANDO;i++)
    {
    switch (ordenRecibida[i])
      {
      case ' ':
        //fin del comando, hay parametro
        inicioParametro=i+1;
        
        //Leo el parametro
        for (int j=0;j<LONG_ORDEN;j++)
          {  //Parsea la orden      
          if(ordenRecibida[j+inicioParametro]==0) 
            {
            strncpy(sParametro,ordenRecibida+inicioParametro,j+1);//copio el parametro como texto
            break;
            }
          else iParametro=(iParametro*10)+(int)ordenRecibida[j+inicioParametro]-48; //hay que convertir de ASCII a decimal
          }
        fParametro=String(sParametro).toFloat();
        
        i=LONG_COMANDO;
        break;
      case 0:
        //fin de la orden. No hay parametro
        i=LONG_COMANDO;
        break;
      default:
        comando+=ordenRecibida[i];
        break;
      }
    }

  //Limpia las variables que3 recogen la orden
  lonOrden=0;
  ordenRecibida[0]=0;

  if (debug) Traza.mensaje("comando: %s\niParametro: %i\nsParametro: %s\nfParametro: %f\n",comando.c_str(),iParametro,sParametro,fParametro);
    
/**************Nueva funcion ***************************/
  int8_t indice=0;
  for(indice=0;indice<MAX_COMANDOS;indice++)
    {
    if (debug) Traza.mensaje("Comando[%i]: {%s} - {%s}\n",indice,comando.c_str(),comandos[indice].comando.c_str());

    if (comandos[indice].comando==comando) 
      {
      //Ejecuta la funcion asociada
      comandos[indice].p_func_comando(iParametro, sParametro, fParametro);
      return(0);
      }    
    }

  //Si llega aqui es que no ha encontrado el comando
  Traza.mensaje("Comando no encontrado\n");
  return(-1);//Comando no encontrado  
/*******************************************************/
}

void limpiaOrden(void)
  {
  lonOrden=0;
  ordenRecibida[0]=0;
  }

/*********************************************************************/
/*  Funciones para los comandos                                      */
/*  void (*p_func_comando) (int, char*, float)                       */
/*********************************************************************/  
void func_comando_vacio(int iParametro, char* sParametro, float fParametro) //"vacio"
{}

void func_comando_help(int iParametro, char* sParametro, float fParametro) //"help"
  {
  Traza.mensaje("\n\nComandos:");  
  for(int8_t i=0;i<MAX_COMANDOS;i++) if (comandos[i].comando!=String("vacio")) Traza.mensaje("Comando %i: [%s]\n",i, comandos[i].comando.c_str());
  Traza.mensaje("\n------------------------------------------------------------------------------\n");
  }

void func_comando_restart(int iParametro, char* sParametro, float fParametro)//"restart")
  {
  ESP.restart();
  }
  
void func_comando_info(int iParametro, char* sParametro, float fParametro)//"info")
  {
  Traza.mensaje("\n-----------------info uptime-----------------\n");
  Traza.mensaje("Uptime: %lu segundos\n", (esp_timer_get_time()/(unsigned long)1000000)); //la funcion esp_timer_get_time() devuelve el contador de microsegundos desde el arranque. rota cada 292.000 años
  Traza.mensaje("-----------------------------------------------\n");  

  Traza.mensaje("-----------------Hardware info-----------------\n");
  Traza.mensaje("FreeHeap: %i\n",ESP.getFreeHeap());
  Traza.mensaje("ChipId: %i\n",ESP.getChipRevision());
  Traza.mensaje("SdkVersion: %s\n",ESP.getSdkVersion());
  Traza.mensaje("CpuFreqMHz: %i\n",ESP.getCpuFreqMHz());
  //gets the size of the flash as set by the compiler
  Traza.mensaje("FlashChipSize: %i\n",ESP.getFlashChipSize());
  Traza.mensaje("FlashChipSpeed: %i\n",ESP.getFlashChipSpeed());

  Traza.mensaje("-----------------------------------------------\n");
  }  

void func_comando_flist(int iParametro, char* sParametro, float fParametro)//"fexist")
  {
  Serial.printf("listado de ficheros:\n%s",listadoFicheros(String(sParametro)).c_str()); 
  }

void func_comando_fexist(int iParametro, char* sParametro, float fParametro)//"fexist")
  {
  if (strlen(sParametro)==0) Traza.mensaje("Es necesario indicar un nombre de fichero\n");
  else
    {
    if(SPIFFS.exists(sParametro)) Traza.mensaje("El fichero %s existe.\n",sParametro);
    else Traza.mensaje("NO existe el fichero %s.\n",sParametro);
    }
  }

void func_comando_fopen(int iParametro, char* sParametro, float fParametro)//"fopen")
  {
  if (strlen(sParametro)==0) Traza.mensaje("Es necesario indicar un nombre de fichero\n");
  else
    {
    File f = SPIFFS.open(sParametro, "r");
    if (f)
      { 
      Traza.mensaje("Fichero abierto\n");
      size_t tamano_fichero=f.size();
      Traza.mensaje("El fichero tiene un tamaño de %i bytes.\n",tamano_fichero);
      char buff[tamano_fichero+1];
      f.readBytes(buff,tamano_fichero);
      buff[tamano_fichero+1]=0;
      Traza.mensaje("El contenido del fichero es:\n******************************************\n%s\n******************************************\n",buff);
      f.close();
      }
    else Traza.mensaje("Error al abrir el fichero %s.\n", sParametro);
    } 
  } 

void func_comando_fremove(int iParametro, char* sParametro, float fParametro)//"fremove")
  {
  if (strlen(sParametro)==0) Traza.mensaje("Es necesario indicar un nombre de fichero\n");
  else
    { 
    if (SPIFFS.remove(sParametro)) Traza.mensaje("Fichero %s borrado\n",sParametro);
    else Traza.mensaje("Error al borrar el fichero%s\n",sParametro);
    } 
 }

void func_comando_format(int iParametro, char* sParametro, float fParametro)//"format")
  {     
  if (formatearFS()) Traza.mensaje("Sistema de ficheros formateado\n");
  else Traza.mensaje("Error al formatear el sistema de ficheros\n");
  } 

void func_comando_echo(int iParametro, char* sParametro, float fParametro)//"echo") 
  {
  Traza.mensaje("echo; %s\n",sParametro);
  }

void func_comando_debug(int iParametro, char* sParametro, float fParametro)//"debug")
  {
  ++debugGlobal=debugGlobal % 2;
  if (debugGlobal) Traza.mensaje("debugGlobal esta on\n");
  else Traza.mensaje("debugGlobal esta off\n");
  }

void inicializaOrden(void)
  { 
  int i =0;  

  limpiaOrden();
  
  comandos[i].comando="help";
  comandos[i].descripcion="Listado de comandos";
  comandos[i++].p_func_comando=func_comando_help;
  
  comandos[i].comando="restart";
  comandos[i].descripcion="Reinicia el modulo";
  comandos[i++].p_func_comando=func_comando_restart;
  
  comandos[i].comando="info";
  comandos[i].descripcion="Devuelve informacion del hardware";
  comandos[i++].p_func_comando=func_comando_info;
  
  comandos[i].comando="flist";
  comandos[i].descripcion="Lista los ficheros en el sistema de ficheros";
  comandos[i++].p_func_comando=func_comando_flist;

  comandos[i].comando="fexist";
  comandos[i].descripcion="Indica si existe un fichero en el sistema de ficheros";
  comandos[i++].p_func_comando=func_comando_fexist;

  comandos[i].comando="fopen";
  comandos[i].descripcion="Devuelve el contenido del fichero especificado";
  comandos[i++].p_func_comando=func_comando_fopen;
  
  comandos[i].comando="fremove";
  comandos[i].descripcion="Borra el fichero especificado";
  comandos[i++].p_func_comando=func_comando_fremove;
  
  comandos[i].comando="format";
  comandos[i].descripcion="Formatea el sistema de ficheros";
  comandos[i++].p_func_comando=func_comando_format;
  
  comandos[i].comando="echo";
  comandos[i].descripcion="Devuelve el eco del sistema";
  comandos[i++].p_func_comando=func_comando_echo;
   
  comandos[i].comando="debug";
  comandos[i].descripcion="Activa/desactiva el modo debug";
  comandos[i++].p_func_comando=func_comando_debug;

  //resto
  for(;i<MAX_COMANDOS;)
    {
    comandos[i].comando="vacio";
    comandos[i].descripcion="Comando vacio";
    comandos[i++].p_func_comando=func_comando_vacio;  
    }
    
  func_comando_help(0,NULL,0.0);
  }
