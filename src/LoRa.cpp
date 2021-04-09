/*
https://www.thethingsnetwork.org/forum/t/overview-of-lorawan-libraries-howto/24692
https://github.com/Lora-net/LoRaMac-node
*/
#include <LoRa.h>
#include <Configuracion.h>

EmisorLora Lora;

// Pin mapping for the SparkX ESP32 LoRa 1-CH Gateway
const lmic_pinmap lmic_pins = {
 .nss = 18,
 .rxtx = LMIC_UNUSED_PIN,
 .rst = 14,
 .dio = {26, 35, 33},
};

void recepcionMensajes(void *pUserData, u1_t port, const u1_t *pMessage, size_t nMessage);

EmisorLora::EmisorLora(void){};

//Init LoRa
void EmisorLora::inicializa(void){  
  //Inicializo en flag de envio a falseo
  setOnAir(false);

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
  LMIC.seqnoUp=configuracion.getSeqnoUp();
  LMIC.seqnoDn=configuracion.getSeqnoDn();

  // EU channel setup
  // Set up the channel used by the Things Network and compatible with our gateway.
  // Setting up channels should happen after LMIC_setSession, as that configures the minimal channel set.
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //////LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_DECI);      // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band  


  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Disabe ADR mode
  //LMIC_setAdrMode(0);
  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7, 14);

  // Let LMIC offset for +/- 10% clock error
  LMIC_setClockError (MAX_CLOCK_ERROR * 10/100);  

  //Configro el callback para la recepcion de mensajes
  LMIC_registerRxMessageCb (recepcionMensajes, NULL);
}

void EmisorLora::loop(void){os_runloop_once();}

uint16_t EmisorLora::setMensaje(String m){
  mensaje=m;
  return mensaje.length();
}

//Envia los datos pasado
lmic_tx_error_t EmisorLora::envia(String msg) {
  setMensaje(msg);
  return envia();
}
// Envia los datos cargados
lmic_tx_error_t EmisorLora::envia(void) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
        return OP_TXRXPEND;
    } else {
        // Limpio
        //////LMIC_clrTxData();
        // Prepare upstream data transmission at the next possible time.
        //Serial.println("Antes de enviar...");
        Lora.setOnAir(true);
        return  LMIC_setTxData2(1, (xref2u1_t)mensaje.c_str(), mensaje.length(), 0);
    }
}
 
/************************** callbacks **********************************/
//Necesario declararlas
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

// State machine event handler
void onEvent (ev_t ev) {
  //Serial.printf("%i: ", os_getTime());
  Serial.printf("%lld: ", esp_timer_get_time());  

  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.printf("EV_SCAN_TIMEOUT\n");
      break;
    case EV_BEACON_FOUND:
      Serial.printf("EV_BEACON_FOUND\n");
      break;
    case EV_BEACON_MISSED:
      Serial.printf("EV_BEACON_MISSED\n");
      break;
    case EV_BEACON_TRACKED:
      Serial.printf("EV_BEACON_TRACKED\n");
      break;
    case EV_JOINING:
      Serial.printf("EV_JOINING\n");
      break;
    case EV_JOINED:
      Serial.printf("EV_JOINED\n");
      break;
    case EV_RFU1:
      Serial.printf("EV_RFU1\n");
      break;
    case EV_JOIN_FAILED:
      Serial.printf("EV_JOIN_FAILED\n");
      break;
    case EV_REJOIN_FAILED:
      Serial.printf("EV_REJOIN_FAILED\n");
      break;
    case EV_TXCOMPLETE:
      digitalWrite(LED_BUILTIN, LOW); // Turn off LED after send is complete
      
      configuracion.setSeqnoUp(LMIC.seqnoUp);
      configuracion.setSeqnoDn(LMIC.seqnoDn);

      Serial.printf("EV_TXCOMPLETE (includes waiting for RX windows)\n");

      //if (LMIC.txrxFlags & TXRX_ACK) Serial.printf("Received ack\n");
      /*
      if (LMIC.txrxFlags & TXRX_ACK) Serial.printf("confirmed UP frame was acked (mutually exclusive with TXRX_NACK)\n");
      if (LMIC.txrxFlags & TXRX_NACK) Serial.printf("confirmed UP frame was not acked (mutually exclusive with TXRX_ACK)\n");
      if (LMIC.txrxFlags & TXRX_PORT) Serial.printf("a port byte is contained in the received frame at offset LMIC.dataBeg – 1.\n");
      if (LMIC.txrxFlags & TXRX_NOPORT) Serial.printf("no port byte is available.\n");
      if (LMIC.txrxFlags & TXRX_DNW1) Serial.printf("received in first DOWN slot (mutually exclusive with TXRX_DNW2)\n");
      if (LMIC.txrxFlags & TXRX_DNW2) Serial.printf("received in second DOWN slot (mutually exclusive with TXRX_DNW1)\n");
      if (LMIC.txrxFlags & TXRX_PING) Serial.printf("received in a scheduled RX slot\n");
      if (LMIC.txrxFlags & TXRX_LENERR) Serial.printf("the transmitted message was abandoned because it was longer than the established data rate.\n");
      
      Serial.printf("Frame: [%s]\n", LMIC.frame);
      Serial.printf("Data len: %i\n", LMIC.dataLen);
      Serial.printf("Data beg: %i\n", LMIC.dataBeg);
      Serial.printf("txrxFlags: %i\n", LMIC.txrxFlags); 

      Serial.printf("txCnt:    %i\n", LMIC.txCnt);
      Serial.printf("SeqnoUp:    %i\n", LMIC.seqnoUp);
      Serial.printf("SeqnoDn:    %i\n", LMIC.seqnoDn);

      Serial.printf("PendTxData: [%s]\n", LMIC.pendTxData);
      Serial.printf("PendTxLen: %i\n", LMIC.pendTxLen);
      Serial.printf("PendTxPort: %i\n", LMIC.pendTxPort);
      Serial.printf("PendTxConf: %i\n", LMIC.pendTxConf);

      if (LMIC.dataLen) {
        Serial.printf("*******************recepcion****************************");
        Serial.printf("Received %i bytes of payload\n",LMIC.dataLen);
        Serial.printf("Mensaje: [");
        for(uint8_t i=0;i<LMIC.dataLen;i++)Serial.printf("0x%u ",LMIC.frame[i]);
        Serial.printf("]\n");
        Serial.printf("********************************************************");        
      }
      */

      Lora.setOnAir(false); //Pongo el flag de envio a cierto, espero el fin de la transmision

#ifdef SEND_BY_TIMER
      // Schedule the next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
#endif
      break;
    case EV_LOST_TSYNC:
      Serial.printf("EV_LOST_TSYNC\n");
      break;
    case EV_RESET:
      Serial.printf("EV_RESET\n");
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.printf("EV_RXCOMPLETE\n");
      break;
    case EV_LINK_DEAD:
      Serial.printf("EV_LINK_DEAD\n");
      break;
    case EV_LINK_ALIVE:
      Serial.printf("EV_LINK_ALIVE\n");
      break;
    case EV_TXSTART:
      Lora.setOnAir(true);
      digitalWrite(LED_BUILTIN, HIGH); // Turn on LED after send is complete
      Serial.printf("EV_TXSTART\n");    
      break;
    case EV_TXCANCELED:
      Serial.printf("EV_TXCANCELED\n");
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      Serial.printf("EV_RXSTART\n");
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.printf("EV_JOIN_TXCOMPLETE: no JoinAccept\n");
      break;
    default:
      Serial.printf("Unknown event: \n");
      Serial.println((unsigned) ev);
      break;
  }
}

void recepcionMensajes(void *pUserData, u1_t port, const u1_t *pMessage, size_t nMessage){
  Serial.println("--------------------------------------------");
  Serial.printf("port: %i\n",port);
  Serial.printf("Codificado\nlongitud: %i | mensaje: ",nMessage);
  for(uint8_t i=0;i<nMessage;i++) Serial.printf("0x%u ",pMessage[i]);
  Serial.printf("\n\n");
  Serial.println("--------------------------------------------");
}
/********************************************************************************************************/

