#ifndef TRAZA
#define TRAZA

#define MAX_CLIENTES_DEBUG 1
#define PUERTO_DEBUG       8000
#define MAX_NIVELDEBUG     256
#define BUFFER             16 

typedef enum {
  serie=0,
  ethernet,
  fichero
} medios_t;

#include <stdarg.h>
#include <WiFi.h>

class TrazaClass
  {
  private:
    WiFiServer _servidor;
    WiFiClient _cliente;
    uint8_t _nivelDebug;
    uint8_t _medio;

    void escribeTCP(String mensaje);
    
  public:
    TrazaClass(uint8_t nivel=MAX_NIVELDEBUG, medios_t medio=serie): _servidor(PUERTO_DEBUG, MAX_CLIENTES_DEBUG) {}

    void begin(uint8_t nivel, medios_t medio);
    
    void mensaje(char *str, ...);

    uint8_t getNivelDebug(void) {return _nivelDebug;}
    uint8_t setNivelDebug(uint8_t nivel) {_nivelDebug=nivel;}
    uint8_t getMedio(void) {return _medio;}
    uint8_t setMedio(medios_t medio) {_medio=medio;}
  };

extern TrazaClass Traza;
#endif
