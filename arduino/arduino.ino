#include <SPI.h> //Libreria para manejo del puerto serie de la PC 
#include <Ethernet.h> //Libreria para usar funciones de redes en arduino

byte mac[]={0xDE,0xAD,0xBE,0xEF,0xFE,0xED}; //MAC del Arduino en la red
IPAddress ip(192,168,1,121); //Dirección IP para el Arduino Ethernet
EthernetServer servidor(80); //Puerto del servidor web Arduino.

String json;        //Variable para enviar datos en formato JSON
float temperatura;  //Varianble que almacena la temperatura del Sensor LM 35
int tempPin = 0;    //Variable para una de las entradas de arduino recibir la temperatura
int brilloLed = 0;  // variable para guardar el valor con el que se iluminara el led
int valorPotenciometro = 0;  // variable para guardar el valor leido del sensor
int PIN_VAR=6;      //Pin 6 del arduino se asocia a la luz variable
int PIN_LED1=7;     //Pin 7 del arduino se asocia a la luz sala
int PIN_LED2=8;     //Pin 8 del arduino se asocia a la luz cocina
int PIN_LED3=9;     //Pin 9 del arduino se asocia a la luz dormitorio
String readString=String(30);  //Variable para recibir datos de provientes de las aplicaciones web y mobil
String state=String(3);        //Variable para mostrar el estado de la Luz 1
String estadoAlarma=String(12); //Varibale para mostrar el estado de la alarma (activado/desactivado)
String sensorAlarma=String(12); //Variable para mostrar el estado del sensor (activado/desactivado)

int ledPin = 3; // declaramos el led en el pin 12 del arduino
int sensorPin=2; // declaramos el sensor PIR en el pin 11
int val = 0;     //variable para asignar la lectura del sensor 
int alarma=4;    //variable para saber si la alarma esta mostrando algo

void setup(){
  Ethernet.begin(mac, ip); //Inicializamos con las direcciones asignadas
  servidor.begin();        
  Serial.begin(9600);       //Puerto serial 
  pinMode(PIN_LED1,OUTPUT); //PIN para el led1 lo inicializamos como salida
  digitalWrite(PIN_LED1,LOW); //PIN para el led1 lo inicializamos como apagado
  state="OFF";
  pinMode(PIN_LED2,OUTPUT);
  digitalWrite(PIN_LED2,LOW);
  pinMode(PIN_LED3,OUTPUT);
  digitalWrite(PIN_LED3,LOW);
  pinMode(PIN_VAR,OUTPUT);
  digitalWrite(PIN_VAR,LOW);

  pinMode(ledPin, OUTPUT);     //El pin 12 del arduino lo asignamos como salida alarma
  pinMode(sensorPin, INPUT);  //Pin para recibir el estado del sensor
  pinMode(alarma, OUTPUT);    //pin para hacer sonar el boozer

  estadoAlarma="Desactivado"; //Por defecto la alarma y el sensor estan desactivados
  sensorAlarma="Desactivado";
}

void loop(){

  temperatura = analogRead(tempPin); //Leemos la temperatura del LM35 esto ingresa en voltajes
  temperatura = (4.0 * temperatura * 100.0 ) / 1024.0; //calculamos el valor de la temperatura en °C

  val = digitalRead(sensorPin);   //Lee el valor de la variable del sensor de presencia
  if(val==HIGH && estadoAlarma=="Activado"){ //Si esta activado el sensor y esta activa el aviso de alarma
    digitalWrite(alarma, HIGH); //sonamos el boozer
    digitalWrite(ledPin, HIGH); //prendemos la luz de alarma
    sensorAlarma="Activado";  //mostramos el aviso de sensor activado
  }
  if(val==LOW){  //Si la alarma no detecta presencia
    digitalWrite(alarma, LOW);  //apagamos el boozer
    digitalWrite(ledPin, LOW);  //apagamos el led
    sensorAlarma="Desactivado"; //mostramos el aviso de sensor desactivado
  }
  
  EthernetClient cliente=servidor.available(); //Inicializadmos red
  
  if(cliente){ //si existe conexión a red
    boolean lineaenblanco=true; 
    while(cliente.connected()){
      if(cliente.available()){ //red habilitada
        
        char c=cliente.read();
        if(readString.length()<30){ //Leemos petición caracter a caracter
          readString.concat(c); //Almacenar los caracteres en la variable readString
        }
        if(c=='\n' && lineaenblanco){ //Si la petición ha finalizado

          int ALAR = readString.indexOf("ALAR=");   //Leemos los datos enviados
          if(readString.substring(ALAR,ALAR+6)=="ALAR=F"){ //Si es apagar alarma
            estadoAlarma="Desactivado";
          }else if (readString.substring(ALAR,ALAR+6)=="ALAR=T"){ //se es prender alarma
            estadoAlarma="Activado";
          }
          
          int LED1 = readString.indexOf("LED1="); //Leemos los datos enviados
          if(readString.substring(LED1,LED1+6)=="LED1=F"){ //si es apagar luz
            digitalWrite(PIN_LED1,LOW);
            state="OFF";
          }else if (readString.substring(LED1,LED1+6)=="LED1=T"){ //si es prender luz
            digitalWrite(PIN_LED1,HIGH);
            state="ON";
          }

          int LED2 = readString.indexOf("LED2=");
          if(readString.substring(LED2,LED2+6)=="LED2=F"){
            digitalWrite(PIN_LED2,LOW);
          }else if (readString.substring(LED2,LED2+6)=="LED2=T"){
            digitalWrite(PIN_LED2,HIGH);
          }

          int LED3 = readString.indexOf("LED3=");
          if(readString.substring(LED3,LED3+6)=="LED3=F"){
            digitalWrite(PIN_LED3,LOW);
          }else if (readString.substring(LED3,LED3+6)=="LED3=T"){
            digitalWrite(PIN_LED3,HIGH);
          }

          int VAR1 = readString.indexOf("VAR1=");
          if(readString.substring(VAR1,VAR1+4)=="VAR1"){
            valorPotenciometro=readString.substring(VAR1+5,VAR1+6).toInt();
            Serial.print(valorPotenciometro);
            brilloLed = map(valorPotenciometro,0,16,0,255); // control de luz variable
            analogWrite(PIN_VAR, brilloLed);
          }
          
          //Juntamos variables y texto para enviarlos en formato JSON
          json = "{\"";
          json += "temperatura\": \"" + (String)temperatura + "\", ";
          json += "\"uptime\": \"" + (String)millis() + "\", ";
          json += "\"estadoAlarma\": \"" + estadoAlarma + "\", ";
          json += "\"sensorAlarma\": \"" + sensorAlarma + "\", ";
          json += "\"valSensor\": \"" + (String)val + "\" ";
          json += "}\n";
          
          cliente.println("HTTP/1.1 200 OK"); // enviamos cabeceras
          cliente.println("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
          cliente.println("Content-Type: text/javascript");
          cliente.println("Access-Control-Allow-Origin: *");
          cliente.println();
          cliente.println(json); //imprimimos datos
          cliente.stop();//Cierro conexión con el cliente
          readString="";
        }
      }
    }
  }
}
