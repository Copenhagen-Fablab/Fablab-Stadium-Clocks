/*  ####################################
    DisplayNumbers, v1.0
    ####################################
    by Emil Bjørn, bjoern.emil@gmail.com
    22 September, 2019
    ------------------------------------
    This sketch is made to easily display numbers on a 4 digit stadium clock. 
    Each clock consists of two modules, which in turn are controlled by three HEF4094B bit shift registers.
    The output voltage is amplified from the output of the bit shift registers to 12V in subsequent four darlington arrays.

    The code has the primary function shiftSignNumber(int), which correctly displays any number that fits on the display.

    To use more than one stadium sign in series, change the variable 'signLen' to match the total digits of the combined sign
    ####################################*/

//Pin connected to latch pin (STR) of HEF4094B
const int latchPin = 8;
//Pin connected to clock pin (CP) of HEF4094B
const int clockPin = 12;
//Pin connected to Data pin (D) of HEF4094B
const int dataPin = 11;
//Pin connected to Output Enable (OE) of HEF4094B
const int outEnable = 9;

// Number of digits in sign - change this variable if chaining multiple displays together
int signLen = 4;

void setup()
{
    //set pins to output because they are addressed in the main loop
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(outEnable, OUTPUT);
    digitalWrite(outEnable, HIGH);
    Serial.begin(9600);
}

void loop()
{
// Code to determine which number to display goes here. Use shiftSignNumber(x) to display number x
    for (int i = 0; i < pow(10, signLen); i++)
    {
        shiftSignNumber(i);
        delay(1000);
    }
}

// Function to shift out two digits from an integer between 00 and 99
void shiftTwoDigits(int num)
{
    //  Arrays holding the correct bytes responsible for rendering digits 0-9
    const int digitTable1[10][3] = {{B01100111, B00000111, B00000000},  // ?0
                                    {B00000110, B00000010, B00000000},  // ?1
                                    {B10111001, B00000110, B00000000},  // ?2
                                    {B10010111, B00000110, B00000000},  // ?3
                                    {B10010110, B00000011, B00000000},  // ?4
                                    {B10011011, B00000101, B00000000},  // ?5
                                    {B01111011, B00000101, B00000000},  // ?6
                                    {B00000110, B00000110, B00000000},  // ?7
                                    {B01110111, B00000111, B00000000},  // ?8
                                    {B10010111, B00000111, B00000000}}; // ?9

    const int digitTable2[10][3] = {{B00000000, B01111000, B01101001},  // 0?
                                    {B00000000, B01100000, B00000001},  // 1?
                                    {B00000000, B10101000, B01100110},  // 2?
                                    {B00000000, B01101000, B01000111},  // 3?
                                    {B00000000, B01110000, B00000111},  // 4?
                                    {B00000000, B10011000, B01000111},  // 5?
                                    {B00000000, B10011000, B01101011},  // 6?
                                    {B00000000, B01101000, B00000001},  // 7?
                                    {B00000000, B01111000, B01101011},  // 8?
                                    {B00000000, B01111000, B01000111}}; // 9?

    //Extract digits from 'num' input
    int digit[] = {num % 10, (num / 10) % 10};

    // Generating the 3 bytes for the registers
    int registerVal[3];
    for (int i = 0; i < 3; i++)
    {
        registerVal[i] = digitTable1[digit[0]][i] | digitTable2[digit[1]][i];
    }

    // shiftOut bytes
    for (int i = 0; i < 3; i++)
    {
        shiftOut(dataPin, clockPin, MSBFIRST, registerVal[2 - i]);
    }
}

// shiftSignNumber takes in any integer number and display the last 'signLen' digits on the display
void shiftSignNumber(int num)
{
    int signModules = signLen / 2;
    digitalWrite(latchPin, LOW);
    for (int i = signModules; i > 0; i--)
    {
        int shiftPayload = (int)(num / pow(100, i - 1)) % 100;
        shiftTwoDigits(shiftPayload);
    }
    digitalWrite(latchPin, HIGH);
}