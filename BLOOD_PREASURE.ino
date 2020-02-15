#include <SoftwareSerial.h>
#define tempPin   A0 
#define DEBUG true 
#define IP "184.106.153.149"// thingspeak.com ip

SoftwareSerial esp(2,3);
//SoftwareSerial sms(7, 6);

String Api_key = "GET /update?key=xxxxxxxxxx"; //change it with your api key like "GET /update?key=Your Api Key" 

String textMessage="";
const int number_of_spaces = 13;
const int number_of_values = 11;

int error;
int val;
float temp; 
int sys_;
int dia_;
int pulse_rate_;


void setup()
{
 
  Serial.begin(38400);
  esp.begin(115200);
  
  pinMode(tempPin,INPUT);
  
  send_command("AT+RST\r\n", 2000, DEBUG); //reset module
  send_command("AT+CWMODE=1\r\n", 1000, DEBUG); //set station mode
  send_command("AT+CWJAP=\"USSID\",\"PASSWORD\"\r\n", 2000, DEBUG);   //connect wifi network
  while(!esp.find("OK")) {} //wait for connection
  
  
}
void loop()
{
  val = analogRead(tempPin);
  float mv = ( val/1024.0)*5000;
  float cel = mv/10;
  temp = cel;


  //----------------------------------------------------------------------
  if (Serial.available() > 0) {

    //save the incoming data into textMessage string
    textMessage = Serial.readString();
   
    //check if the text message contains #
    if (textMessage.indexOf("00") >= 0) {
      //++++++++++++++++++++++++++++++++++++++++++
     
      int start_index = textMessage.indexOf(':');
       start_index = textMessage.indexOf(':', start_index + 1);
       start_index = textMessage.indexOf('\r', start_index + 1);
       start_index = textMessage.indexOf('\r', start_index + 1);
      
      int space_index[number_of_spaces];
      String received_value[number_of_values];
      space_index[0] = start_index;

      for(int f = 0; f < number_of_spaces; f++){
        space_index[f+1] = textMessage.indexOf(' ',space_index[f]+1);
      }

      for(int f = 0; f < number_of_values ; f++){
        received_value[f] = textMessage.substring(space_index[f]+1, space_index[f+1]);
        
      }

      //++++++++++++++++++++++++++++++++++++++++++++++
    

      char received_sys[4];
      char received_dia[4];
      char received_pulse_rate[4];
      
      received_value[0].toCharArray(received_sys,4);
      received_value[1].toCharArray(received_dia,4);
      received_value[3].toCharArray(received_pulse_rate,4);
      
      unsigned long sys = strtoul(received_sys, NULL, 16);
      unsigned long dia = strtoul(received_dia, NULL, 16);
      unsigned long pulse_rate = strtoul(received_pulse_rate, NULL, 16);

      sys_ = sys;
      dia_ = dia;
      pulse_rate_ = pulse_rate;
      
  
      
      start: //label 
        error=0;
        updatedata();
    }
  }



  //---------------------------------------------------------------------------
  
 
 


}

void updatedata(){
  String command = "AT+CIPSTART=\"TCP\",\"";
  command += IP;
  command += "\",80";
  //Serial.println(command);
  esp.println(command);
  delay(2000);
  if(esp.find("Error")){
    return;
  }
  command = Api_key ;
  command += "&field1=";   
  command += temp;
  command += "&field2=";   
  command += sys_;
  command += "&field3=";   
  command += dia_;
  command += "&field4=";   
  command += pulse_rate_;
  command += "\r\n";
 // Serial.print("AT+CIPSEND=");
  esp.print("AT+CIPSEND=");
 // Serial.println(command.length());
  esp.println(command.length());
  if(esp.find(">")){
    //Serial.print(command);
    esp.print(command);
  }
  else{
    
  // Serial.println("AT+CIPCLOSE");
   esp.println("AT+CIPCLOSE");
    //Resend...
    error=1;
  }
  }

String send_command(String command, const int timeout, boolean debug)
{
  String response = "";
  esp.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp.available())
    {
      char c = esp.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
