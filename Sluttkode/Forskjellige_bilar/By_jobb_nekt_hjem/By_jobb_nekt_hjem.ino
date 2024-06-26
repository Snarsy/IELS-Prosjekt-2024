#include <Zumo32U4.h> //Initialiserer zumo komponentene og laster inn Turnsensor som lar bilen rotere 90 grader
#include <IRremote.h>
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonC buttonC;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4IRPulses irSensors;
Zumo32U4IMU imu;
Zumo32U4Encoders encoder;

//For ir sender
#include "Zumo32u4IRsender.h"
#define DEVICE_ID 35
Zumo32U4IRsender ZumoIrSender(DEVICE_ID, RIGHT_IR);

//Ir Receive
#define PRINT_DEGUB_INFO 0
#define SUBCARIER_PERIOD 420 // The default frequency is 16000000 / (420 + 1) = 38.005 kHz
#define IR_BRIGHTNESS 50     // 0-400, 0 is off, 400 is max brightness
#define RIGHT_IR 0
#define LEFT_IR 1
const int IRPin = A4;
IRrecv IR(IRPin);

//Variabler for kjøremonsteret
int prevcase;
int driveOverNum = 0;
unsigned long prevmillis = 0;
int linelength = 400;
bool doDrive = 0;

int caseNum = 4;
int destination = 10;
int currentPosition = 3;
int clockWise = 0;
int rotationtime = 500;

//LadeStasjon
int chargePrevMillis;
int howMuchCharge = -1;  //kun for å teste uten ir signal
int chargecounter = 0;
bool haveturned = 1;

//Garasjen
int parkingAvailable = 0;//Har nummeret 10 fram til bilen får ledig plass.
int caseNumGarage = 0;
int currentPosGarage = 0;

//Bompenger
unsigned long BompreviousMillis = 0; // Store the last time the IR sensor was triggered
const long Bominterval = 10000; // 10 seconds interval
int tollgatepassings = 0;

//Nabolag
int housecounter = 5;
int housenumber = 2;
int housewait = 5000;

//Outsider
int outsidercounter = 0; // Setter man denne til 3 og caseNum til 5 så vil bilen være klar for å starte ved huset utenfor byen.
unsigned long outsidermillis;

// Batterinivå
unsigned long previousSpeedMillis, prevAveragesMillis = 0;
int speedDistance, totalDistance = 0;
int A = 1;
int16_t firstSpeed = 0;
int readTime = 100;
int16_t totalSpeed = 0;
int16_t negativeTotalSpeed = 0;
bool seventyMillis_start, stoppedTimer = false;
int holdTimerValue, secondsAboveSeventy, aboveSeventyCounter, maxSpeed, distanceAverage, averageSpeed60Sec = 0;
int akselerasjon;
int prevSpeed;
int ecodrive = 0;
unsigned long ecomillis;
int batterylevel;
#include "DriveLib.h"


void driveOverLine(){//Funksjon til for å kjøre over linje
    switch (driveOverNum){
        case 0:
            prevmillis = millis();
            driveOverNum = 1;
            break;
        case 1:
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>linelength){
                caseNum = prevcase;
                driveOverNum = 0;
                motors.setSpeeds(0,0);
            }
            break;
    }
}

void irDecodeGarasje(){ // Decoder ir signaler bilen får og setter verdier til hvor bilen skal kjøre i garasjen.
    if(buttonC.isPressed()){
        parkingAvailable = 1;
    }
    if (IR.decode())
    {
        if (IR.decodedIRData.decodedRawData == 135946 || IR.decodedIRData.decodedRawData == 271893) parkingAvailable = 1; // Ingen plass/Bensinbin
        if (IR.decodedIRData.decodedRawData == 510274632)   parkingAvailable = 2; // Første plass
        if (IR.decodedIRData.decodedRawData == 1277849113) parkingAvailable = 3; // Andre plass
        if (IR.decodedIRData.decodedRawData == 2163717077) parkingAvailable = 4; // Tredje plass
        if (IR.decodedIRData.decodedRawData == 2592268650)  parkingAvailable = 5; // Fjerde plass
        IR.resume();
    }
}

void irDecodeCharge(){
    if (IR.decode()){
        if (IR.decodedIRData.decodedRawData == 2290649224)     howMuchCharge = 1; //10
        if (IR.decodedIRData.decodedRawData == 1216907400)     howMuchCharge = 2; //20
        if (IR.decodedIRData.decodedRawData == 1149798536)     howMuchCharge = 3; //30
        if (IR.decodedIRData.decodedRawData == 1145604232)     howMuchCharge = 4; //40
        if (IR.decodedIRData.decodedRawData == 1145342088)     howMuchCharge = 5; //50   
        if (IR.decodedIRData.decodedRawData == 1145324680)     howMuchCharge = 6; //60
        if (IR.decodedIRData.decodedRawData == 1145324616)     howMuchCharge = 7; //70
        if (IR.decodedIRData.decodedRawData == 1145324612)     howMuchCharge = 8; //80
        IR.resume();
    }  
}


void garage(){// Funksjon for kjøringen i garasjen
    switch (caseNumGarage){
        case 0://Denne casen får zumo'n til å kjøre over linjen for så å rotere utifra hvilken retning den kommer fra(clockwise). Deretter kjører den frem, stopper og venter på ir signal.
            if(!haveturned){//Her må det være mindre enn samme verdi som 81.
                if(clockWise){turndeg(-90);};
                if(!clockWise){turndeg(90);};
                prevcase = caseNum;
                caseNum = 0;
                haveturned = !haveturned;
                break;
            }
            motors.setSpeeds(0,0);
            irDecodeGarasje();
            if(millis()-prevmillis>5000){
                ZumoIrSender.send(2);
                delay(50);
                IR.begin(IRPin, ENABLE_LED_FEEDBACK);
                prevmillis = millis();
            }
            if(parkingAvailable != 0){
                caseNumGarage = 1;
            }
            break;
        case 1:// Her er det en rekke tilfeller. Sjekker om bilen er ved linje hvor den vil kjøre over. Deretter kjører den inn eller over krysset.
            followLinemaxSpeed = 200;
            readSensors();
            if(aboveAll() && currentPosGarage<8){//Hvis den treffer et kryss og er på riktig plass i garasjen vil den snu 180 grader og bytte til case 2 hvor den står i ro.
                if(parkingAvailable == 1){// Hvis den ikke får parkere ved at det ikke er plass vil den kjøre over linjen og bytte til case 3.
                    caseNumGarage = 3;
                    prevcase = caseNum;
                    caseNum = 0;
                    haveturned = 0;
                    break;
                }
                prevmillis = millis();
                turndeg(180);
                caseNumGarage = 2;
                break;
            }
            if(aboveLeft()){//Denne skjer førrst ved at uansett hvilket kryss det er så vil zumo'n kjøre over ved hjelp av case 0 i switch(caseNum).
                if(parkingAvailable == 1 && currentPosGarage > 0 && currentPosGarage != 8){
                    caseNumGarage = 3;
                    prevcase = caseNum;
                    caseNum = 0;
                    haveturned = 0;
                    break;
                }
                if(currentPosGarage == (parkingAvailable+1)){
                    turndeg(180);
                    caseNumGarage = 2;
                    break;
                }
                currentPosGarage = currentPosGarage + 1;
                prevcase = caseNum;
                caseNum = 0;
                break;
            }
            if(currentPosGarage == parkingAvailable){//Hvis den er på riktig plass vil zumo'n rotere inn for så å kjøre videre med pid kjøring.
                turndeg(-90);
                currentPosGarage = currentPosGarage + 1;
                break;
            }
            if(currentPosGarage == 9){//Denne kommer i bruk når bilen kjører ut. Dette skjer etter case 2.
                turndeg(90);
                caseNum = 1;
                currentPosition = 9;
                haveturned = 0;
                destination = 4;
                housecounter = 0;
                housewait = 100000;
            }
            driveLinePID();
            break;
        case 2:
            if(millis()-prevmillis<30000 && currentPosGarage == (parkingAvailable+1)){
                motors.setSpeeds(0,0);
            }
            else{
                driveLinePID();
                if(aboveAll()){
                    prevcase = caseNum;
                    caseNum = 0;
                    currentPosGarage = currentPosGarage + 1;
                    break;
                }
                if(currentPosGarage == (parkingAvailable+2)){
                    caseNumGarage = 1;
                    turndeg(-90);
                }
            }
            break;
        case 3:
            if(!haveturned){
                caseNumGarage = 1;
                currentPosGarage = 8;
                turndeg(-90);
                haveturned = !haveturned;
            }
            break;
    }
}

void chargingStation(){// Funksjon for når bilen kjører inn til ladestasjonen
    if(buttonC.isPressed()){
        howMuchCharge = 8;
    }
    followLinemaxSpeed = 200;
    if(doDrive == 1){
        driveLinePID();
    }
    if(!haveturned){//Her må det være mindre enn samme verdi som 81.
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        doDrive = 1;
        haveturned = !haveturned;
    }
    if (aboveAll() && chargecounter == 0){
        motors.setSpeeds(0,0);
        doDrive = 0;
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        chargecounter = chargecounter + 1;
    }
    irDecodeCharge();
    display.gotoXY(3,3);
    if(howMuchCharge == 0){
        display.clear();
        display.print("Lading avbrutt");
    }
    if(howMuchCharge == 1){
        display.print("Lading: +10%");
    }
    if(howMuchCharge == 2){
        display.print("Lading: +20%");
    }
    if(howMuchCharge == 3){
        display.print("Lading: +30%");
    }
    if(howMuchCharge == 4){
        display.print("Lading: +40%");
    }
    if(howMuchCharge == 5){
        display.print("Lading: +50%");
    }
    if(howMuchCharge == 6){
        display.print("Lading: +60%");
    }
    if(howMuchCharge == 7){
        display.print("Lading: +70%");
    }
    if(howMuchCharge == 8){
        display.print("Lading: +80%");
        speedDistance = 0;
    }
    if(howMuchCharge!=-1){
        howMuchCharge = -1;
        chargecounter = chargecounter + 1;
        chargePrevMillis = millis();
    }
    if ((millis()-chargePrevMillis)>3000 && (millis()-chargePrevMillis)<3100){
        display.clear();
        if(clockWise) turndeg(-90);
        if(!clockWise) turndeg(90);
        prevcase = caseNum;
        caseNum = 0;
        doDrive = 1;
    }
   if(aboveAll() && chargecounter > 2){
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        caseNum = 1;
        destination = 4;
        currentPosition = 5;
        haveturned = 0;
   }
}


void neighbourhood(){//Funksjon for kjøringen i nabolaget
    followLinemaxSpeed = 200;
    if(!haveturned){
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        haveturned = !haveturned;
        doDrive = 1;
    }
    if(doDrive == 1){
        driveLinePID();
    }
    if(housecounter == housenumber || housecounter == (housenumber+5)){
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        housecounter = housecounter + 1;
    }
    if(aboveLeft() || aboveRight() && housecounter!=3){
        housecounter = housecounter + 1;
        prevcase = caseNum;
        caseNum = 0;
    }
    if(housecounter == (housenumber+2)){
        turndeg(180);
        housecounter = housecounter + 1;
        doDrive = 0;
        prevmillis = millis();
    }
    if((millis()-prevmillis)>housewait && housecounter == (housenumber+3)){
        doDrive = 1;
        housecounter = housecounter + 1;
    }
    if(housecounter == 9){
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        caseNum = 1;
        housecounter = 0;
        haveturned = 0;
        rotationtime = 700;
    }
}

void outsider(){
    if(outsidercounter == 0){
        if(clockWise) turndeg(-90);
        if(!clockWise) turndeg(90);
        outsidercounter = 1;
    }
    if(outsidercounter == 1){
        driveLinePID();
        if(aboveAll()){
            turndeg(170);
            outsidercounter = 3;
            outsidermillis = millis();
        }
    }
    if(outsidercounter == 2){
        outsidercounter = 0;
        caseNum = 1;
        currentPosition = 1;
        destination = 10;
        turndeg(90);
    }
    if(millis()-outsidermillis>5000 && outsidercounter == 3){
        driveLinePID();
        if(aboveAll()){
            outsidercounter = 2; // Sier at bilen skal til garasjen fra hjemme.
            prevcase = caseNum;
            caseNum = 0;
        }
    }
}

void tollGate(){ //Tar imot bompenger, denne må være bare om det er dieselbil
    unsigned long currentMillis = millis(); 
    
    if (IR.decode()){
        if (IR.decodedIRData.decodedRawData == 1217527807){
            if (BompreviousMillis == 0 || currentMillis - BompreviousMillis >= Bominterval) {
                BompreviousMillis = currentMillis;
                tollgatepassings += 1;
                //Spiller G4
                buzzer.playFrequency(392, 250, 5);
            }
        }
    }
    IR.resume();
}

void driving(){// Funksjon for kjøringen rundt i byen
followLinemaxSpeed = 200;
    switch (caseNum){
            case 0:                 // Driveoverline vil kjøre over en linje og returnere til det den gjorde før. Dette gjør at man kan kjøre over kryss og fortsette videre i koden. Husk prevcase = casenum før man setter casenum = 0.
                driveOverLine();
                break;
            case 1:                 // Generell kjørecase. Vil kjøre over linjene til den treffer ønsket posisjon
                driveLinePID();
                if(currentPosition == 11 && clockWise){
                    currentPosition = 0;
                }
                else if(currentPosition == -1 && !clockWise){
                    currentPosition = 10;
                }
                if(millis()%5==0){
                    display.clear();
                }
                if(aboveAll()){     // Treffer bilen et kryss vil den stoppe og kjøre over for så å oppdatere plasseringen.
                    prevcase = caseNum;
                    caseNum = 0;
                    if(clockWise == 1){
                        currentPosition = currentPosition + 1;
                    }
                    else{
                        currentPosition = currentPosition - 1;
                    }
                    break;
                }
                if(currentPosition == destination){//Hvis bilen er på riktig plass vil den rotere 90 grader og bytte til kjøremønsteret for destinasjonen.
                    prevmillis = millis();
                    if(destination == 10){//Garasje
                        caseNum = 2;
                    }
                    else if(destination == 6){
                        caseNum = 3;//Ladestasjon
                    }
                    else if(destination == 4){
                        caseNum = 4;//Nabolag
                    }
                    else if(destination == 2){
                        caseNum = 5;//Outsider
                    }
                }
                break;
            case 2:
                garage();
                break;
            case 3:
                chargingStation();
                break;
            case 4:
                neighbourhood();
                break;
            case 5:
                outsider();
                break;
        }
}    


void setup(){
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
    lineSensors.initFiveSensors();
    Serial.begin(115200);
    calibrateSensors();
    prevmillis, outsidermillis = 0;
    display.setLayout21x8();
}

void loop(){
    driving();
    tollGate();
    speedometer();
    advarsel();
    if(millis()%100==0 && ecodrive == 0){
        display.clear();
        display.print(batterylevel);
    }
}
