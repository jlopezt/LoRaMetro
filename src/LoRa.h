#define BAND    868E6  //you can set band here directly,e.g. 433E6,868E6,915E6

#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// LoRaWAN NwkSKey, network session key
static const PROGMEM u1_t NWKSKEY[16]= { 0x6D, 0x0A, 0x35, 0x75, 0xE2, 0xEF, 0x8F, 0x3D, 0xF9, 0xF1, 0x1E, 0x6E, 0x6D, 0x42, 0x28, 0x6E };
// LoRaWAN AppSKey, application session key
static const u1_t PROGMEM APPSKEY[16]= { 0xEB, 0x35, 0x7B, 0x33, 0xC9, 0x07, 0xA1, 0x29, 0x57, 0x35, 0x2C, 0x92, 0x73, 0x3E, 0x00, 0x6D };
// LoRaWAN end-device address (DevAddr)
static const u4_t DEVADDR = 637613599; //= 0x2601361F ; // <-- Change this address for every node!

class EmisorLora{
    private:
    //claves ABP
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    String mensaje;  
    
    public:
    uint16_t setMensaje(String m);
    EmisorLora(void);
    void inicializa(void);
    lmic_tx_error_t envia(void);//osjob_t* j);
    lmic_tx_error_t envia(String msg);
    void finaliza(void);
    void reset(void);
};

extern EmisorLora Lora;