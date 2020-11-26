#define CS 10 

#include <SPI.h> // call library

int i;
byte recu[3]; 
int valeur;
float pression;

void setup()
{
 Serial.begin(9600); // initialization of serial communication
 SPI.begin(); // initialization of SPI port
 SPI.setDataMode(SPI_MODE0); // configuration of SPI communication in mode 0
 SPI.setClockDivider(SPI_CLOCK_DIV16); // configuration of clock at 1MHz
 pinMode(CS, OUTPUT);
}

void loop()
{
 digitalWrite(CS, LOW); // activation of CS line
 delayMicroseconds(20);
 for (i=0;i<2;i=i+1)
    {
    recu[i] = SPI.transfer(0); // Sending 2 data to retrieve the value of the two -byte conversion
    delayMicroseconds(20);
    }
 digitalWrite(CS, HIGH); // deactivation of CS line
 valeur = (recu[0] << 8|recu[1]);
 for (i=0;i<2;i=i+1) // Send in serial monitor
    {
    Serial.print("i");
    Serial.print(i);
    Serial.print("=");
    Serial.print(recu[i]);
    Serial.print('\t'); // tabulation
    }
 pression=(valeur/4096.0-0.08)/0.09; // The form given by the documentation
 Serial.print("Pression=");
 Serial.print(pression);
 Serial.println(" kPa");
 delay(1000);
}
