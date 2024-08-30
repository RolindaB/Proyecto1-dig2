// Adafruit IO Publish & Subscribe Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include 

/************************ Example Starts Here *******************************/

// this int will hold the current count for our sketch
int count = 0;

// Track time of last published messages and limit feed->save events to once
// every IO_LOOP_DELAY milliseconds.
//
// Because this sketch is publishing AND subscribing, we can't use a long
// delay() function call in the main loop since that would prevent io.run()
// from being called often enough to receive all incoming messages.
//
// Instead, we can use the millis() function to get the current time in
// milliseconds and avoid publishing until IO_LOOP_DELAY milliseconds have
// passed.
#define IO_LOOP_DELAY 5000
unsigned long lastUpdate = 0;

// set up the 'counter' feed
AdafruitIO_Feed *counter0 = io.feed("LEDSA");
AdafruitIO_Feed *counter1 = io.feed("LEDSI");
AdafruitIO_Feed *counter2 = io.feed("MODO");
AdafruitIO_Feed *counter3 = io.feed("PORTON");
AdafruitIO_Feed *counter4 = io.feed("VENTILADOR");
AdafruitIO_Feed *counter5 = io.feed("ACTUALIZAR");
AdafruitIO_Feed *temperatura = io.feed("temperatura");


void setup() {

  // start the serial connection
  Serial.begin(9600);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  counter0->onMessage(handleMessage);
  counter1->onMessage(handleMessage);
  counter2->onMessage(handleMessage);
  counter3->onMessage(handleMessage);
  counter4->onMessage(handleMessage);
  counter5->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    
    delay(500);
  }

  // we are connected

  counter0->get();
  counter1->get();
  counter2->get();
  counter3->get();
  counter4->get();
  counter5->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // Leer datos de UART
  if (Serial.available()) {
        String datoTemperatura = Serial.readStringUntil('\n');  // Lee hasta el carácter de nueva línea

        //Serial.print("Dato recibido: ");
        Serial.println(datoTemperatura);  // Imprime la cadena completa

        if (datoTemperatura == "0") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(18);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "1") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(19);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "2") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(20);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "3") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(21);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "4") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(22);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "5") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(23);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "6") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(24);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "7") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(25);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "8") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(26);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "9") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(27);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "A") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(28);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "B") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(29);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "C") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(30);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "D") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(31);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "E") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(32);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "F") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(33);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "G") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(34);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "H") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(35);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "I") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(36);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "J") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(37);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "K") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(38);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "L") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(39);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "M") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(40);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "N") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(41);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "O") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(42);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "P") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(43);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "Q") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(44);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "R") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(45);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "S") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(46);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "T") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(47);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "U") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(48);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "V") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(49);  // Enviar el valor a Adafruit IO
        }else if (datoTemperatura == "W") {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(50);  // Enviar el valor a Adafruit IO
        }else {
            //Serial.println("Temperatura guardada en Adafruit IO...");
            temperatura->save(0);  // Enviar el valor a Adafruit IO
        }
    }
    delay(3000);  // Pausa para evitar el envío excesivo de datos

}

// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  //Serial.print("received <- ");
  Serial.println(data->value());

}
