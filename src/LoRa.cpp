/*
https://www.thethingsnetwork.org/forum/t/overview-of-lorawan-libraries-howto/24692
https://github.com/Lora-net/LoRaMac-node
*/
#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
//#include <SPI.h>

#define BAND    868E6  //you can set band here directly,e.g. 433E6,868E6,915E6

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
//static const PROGMEM u1_t NWKSKEY[16] = { PASTE_NWKSKEY_KEY_HERE };
static const PROGMEM u1_t NWKSKEY[16] = { 0x6D, 0x0A, 0x35, 0x75, 0xE2, 0xEF, 0x8F, 0x3D, 0xF9, 0xF1, 0x1E, 0x6E, 0x6D, 0x42, 0x28, 0x6E };

// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
//static const u1_t PROGMEM APPSKEY[16] = { PASTE_APPSKEY_KEY_HERE };
static const u1_t PROGMEM APPSKEY[16] = { 0xEB, 0x35, 0x7B, 0x33, 0xC9, 0x07, 0xA1, 0x29, 0x57, 0x35, 0x2C, 0x92, 0x73, 0x3E, 0x00, 0x6D };

// LoRaWAN end-device address (DevAddr)
//static const u4_t DEVADDR = 0xPASTE_DEV_ADDR_HERE;
//static const u4_t DEVADDR = 0x2601361F ; // <-- Change this address for every node!
static const u4_t DEVADDR = 637613599; // <-- Change this address for every node!

static osjob_t sendjob;

// Pin mapping for the SparkX ESP32 LoRa 1-CH Gateway
const lmic_pinmap lmic_pins = {
 .nss = 18,
 .rxtx = LMIC_UNUSED_PIN,
 .rst = 14,
 .dio = {26, 35, 33},
};

void do_send(osjob_t* j, uint8_t* data);

//Init LoRa
void initLora(void){  
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);

  // EU channel setup
  // Set up the channel used by the Things Network and compatible with our gateway.
  // Setting up channels should happen after LMIC_setSession, as that configures the minimal channel set.
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7, 14);

  // Start job -- Transmit a message on begin
  //do_send(&sendjob,"inicio");
}

// State machine event handler
void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      digitalWrite(LED_BUILTIN, LOW); // Turn off LED after send is complete
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
#ifdef SEND_BY_TIMER
      // Schedule the next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
#endif
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;
    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned) ev);
        break;
  }
}

// Transmit data from mydata
void do_send(osjob_t* j, uint8_t* data) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // Turn on LED while sending
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, data, sizeof(data) - 1, 0);
    Serial.println(F("Packet queued"));
  }
}

