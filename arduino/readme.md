# Instruções para o Arduino Uno

A. Clonar o repositório tcc-bridge

    git clone https://github.com/lkakitani/tcc-bridge.git  

B. Instalar a biblioteca RF24

    mv tcc-bridge/arduino/RF24 ~/sketchbook/libraries/  

C. Rodar o programa pingpair_dyn.ino (o envio ou recebimento é decidido pelo pino 5; no momento presente, está hardcoded)

**Pinos:**

| PIN | NRF24L01 | Arduino GPIO  |
|:---:|:--------:|:-------------:|
|  1  |   GND    |    (GND)      |
|  2  |   VCC    |    (3.3V)     |
|  3  |   CE     |    (8)        |
|  4  |   CSN    |    (9)        |
|  5  |   SCK    |    (13)       |
|  6  |   MOSI   |    (11)       |
|  7  |   MISO   |    (12)       |
|  8  |   IRQ    |      -        |

