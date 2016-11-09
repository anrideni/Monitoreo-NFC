// Codigo Datalogger con fecha y hora para sensores DHT11 y YL-69
/* Librerias NFC y SPI */
#include <SPI.h>                // Librerias NFC
#include "PN532_SPI.h"
#include "NfcAdapter.h"
#include "snep.h"
#include "NdefMessage.h"



/* Librerias Time, Sensores y SD */
#include <TimeLib.h>            // Libreria para el tiempo virtual
#include <DHT11.h>              // Libreria DHT11
#include <SD.h>                 // Libreria SD



/* Variables a usar */
File dataFile;                  // Archivo que creamos
DHT11 dht11(3);                 // Pin datos sensor



String dataString;              // Linea con datos a grabar
String UIDtag;
int err;                                  // Variable de error en la lectura
float temp, hum;                          // Variables del sensor 1
int humsuelo;                             // Variable convertida map porcentaje


    Sd2Card card;                 //Variables para la SD
    const int chipSelect = 4;

    uint8_t ndefBuf[128];         //variables para comunicación P2P
    String mensaje;


void setup(){
  PN532_SPI interface(SPI, 10);   // Se crea una interfaz serial para el chip PN532, en el pin 10
  NfcAdapter nfc = NfcAdapter(interface);
  
  Serial.begin(9600);                      // * Iniciamos comunicacion serial
  setTime(02,20,00,13,10,2016);            // * Configuramos la hora
  pinMode(chipSelect, OUTPUT);             // * Declaramos el pin ChipSelect como salida (modulo SD)
  
  pinMode(5, INPUT_PULLUP);                // * DHT11 GND
  pinMode(6, INPUT);                       // * Definimos como entrada el pin 8 del arduino para el switch
  pinMode(7, INPUT);                       // * Definimos como entrada el pin 8 del arduino para el switch
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  digitalWrite(10,LOW);                    // Desactivo SS del modulo NFC
  digitalWrite(4,HIGH);                    // Activo SS del modulo SD
  
  nfc.begin();
  
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println(F("Fallo al iniciar"));

    return;
    } else {
        Serial.println(F("Inicio CORRECTO"));
    }
 }



void loop(){

    time_t t = now();                         // Variable libreria tiempo
  
    int humsueloraw = analogRead(A0);         // Variable en raw del sensor 2 en la entrada A0
    humsuelo = map(humsueloraw,0,1020,100,0);
    //dht11.read(hum, temp);
    
    dataString=String(day(t)) + String("/") + String(month(t)) + String("/") + String(year(t)) + String(" ") + String(hour(t)) + String(":") + String(minute(t)) + String(", ") + String("temp") + String(" C, ") + String("hum") + String(" %") + String(", ") + String(humsuelo) + String(" %");    // Actualizamos la linea que guardaremos en el archivo de texto
    
    digitalWrite(10,LOW);                   // Desactivo SS del modulo NFC
    digitalWrite(4,HIGH);                   // Activo SS del modulo SD

    SD.begin(chipSelect);    
    File dataFile=SD.open("lectura.txt", FILE_WRITE);     // Creamos archivo de texto
    if(dataFile){
      dataFile.println(dataString);                       // Excribimos el contenido de "dataString" dentro del archivo de texto
      dataFile.close();                                   // Cerramos la escritura dentro del archivo
      Serial.println(dataString);                         // Mostramos en serial lo que se escribio
    }
    else{
      Serial.println(F("Escritura erronea... "));    // Texto informativo de error de escritura
      }
   delay(1000);
   digitalWrite(8, HIGH);
   digitalWrite(9, HIGH);  
     
  while(digitalRead(7)==LOW && digitalRead(6)==HIGH){
    NFCtags();
    }
  while(digitalRead(7)==HIGH && digitalRead(6)==LOW){
    NFCp2p();
  }
}
   

  
void NFCtags(){
  PN532_SPI interface(SPI, 10);   // Se crea una interfaz serial para el chip PN532, en el pin 10
  NfcAdapter nfc = NfcAdapter(interface);
  
  time_t t = now(); 
  nfc.begin();                                             // Inicia modulo NFC
  Serial.println(F("Acercar el Tag NFC a la antena"));     // Texto informativo en puerto serial

  if (nfc.tagPresent()){                              // Lectura por si hay un elemento NFC presente
    NfcTag tag = nfc.read();                          // Lee el Tag(nfc.read) y lo convierte en un objeto (NfcTag)
    //tag.print();                                    // Imprime el tipo de Tag NFC, UID (codifo unico de identificacion), y el mensaje grabado (si esta disponible)
    
    UIDtag = String(day(t)) + String("/") + String(month(t)) + String("/") + String(year(t)) + String(" ") + String(hour(t)) + String(":") + String(minute(t)) + String(" - ") + String(tag.getUidString());
    
    SD.begin(chipSelect);    
    File dataFile=SD.open("tag.txt", FILE_WRITE);     // Creamos archivo de texto
    if(dataFile){
      dataFile.println(UIDtag);                       // Excribimos el contenido de "dataString" dentro del archivo de texto
      dataFile.close();                                   // Cerramos la escritura dentro del archivo
      Serial.println(UIDtag);                         // Mostramos en serial lo que se escribio
    }
    else{
      Serial.println(F("Escritura erronea... "));     // Texto informativo de error de escritura
      }
    }
   delay(1000);
   digitalWrite(8, LOW);
   digitalWrite(9, HIGH);
}
 

void NFCp2p(){
  PN532_SPI pn532spi(SPI, 10);
  SNEP nfc(pn532spi);

  Serial.println("Acerque el dispositivo a la antena"); 
  
    mensaje=String(tag.txt);
    NdefMessage message = NdefMessage();
    message.addUriRecord(mensaje);
    int messageSize = message.getEncodedSize();
    if (messageSize > sizeof(ndefBuf)) {
        Serial.println("ndefBuf is too small");
        while (1) {
        }
    }

    message.encode(ndefBuf);
    if (0 >= nfc.write(ndefBuf, messageSize)) {
        Serial.println("Failed");
    } else {
        Serial.println("Mensaje enviado correctamente:");
        Serial.println(mensaje);

    }
    if(digitalRead(7)==LOW && digitalRead(6)==LOW){
      loop();
    }
    if(digitalRead(7)==
    delay(3000);    
}
