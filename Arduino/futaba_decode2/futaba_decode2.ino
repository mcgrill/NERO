#define channumber 1 //How many channels have your radio?
int value[channumber];

void setup()
{
  Serial.begin(57600); //Serial Begin
  pinMode(3, INPUT); //Pin 3 as input
}
void loop()
{
  while(pulseIn(3, LOW) < 5000){} //Wait for the beginning of the frame
  for(int x=0; x<=channumber-1; x++)//Loop to store all the channel position
  {
    value[x]=pulseIn(3, LOW);
  }
  for(int x=0; x<=channumber-1; x++)//Loop to print and clear all the channel readings
  {
    Serial.print(value[x]-13000); //Print the value
    Serial.print(" ");
    value[x]=0; //Clear the value afeter is printed
  }
  Serial.println(""); //Start a new line
}


