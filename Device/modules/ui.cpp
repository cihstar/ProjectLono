#include "ui.h"
#include "modules.h"

UI::UI(PinName sda, PinName scl, PinName rst, PinName b1, PinName b2, PinName b3):
lcd(sda,scl), buttons{b1,b2,b3}, reset(rst)
{
    reset = 1;       
    sendCommand(0x38,0,0); //init lcd. 8-bit I2C, 2 line display, normal font, normal instructions
    setEntryMode(true); //increment to left after each write
}

UI::~UI(){}

bool UI::sendCommand(char byte, bool rs, bool rw)
{
    char addr;
    char control;
    char send[2];
    
    if (rw)
    {
        addr = 0x7D;
    }
    else
    {
        addr = 0x7C;
    }
    
    if (rs)
    {
        control = 0x80;
    }
    else
    {
        control = 0xC0;   
    }
    
    send[0] = control;
    send[1] = byte;
        
    waitOnBusy();
    
    return lcd.write(addr, send, 2);
}

void UI::waitOnBusy()
{
    char addr = 0x7D;
    char control = 0x80;
    
    char r;
    
    do
    {
        lcd.write(addr, &control, 1);
        lcd.read(addr, &r, 1);
    }
     while( (r & 0x80) == 0);
}

void UI::clearDisplay()
{
    if (!sendCommand(0x01, 0, 0))
    {
        util::printError("Clear LCD Display Failed");
    }
}

void UI::returnHome()
{
    if (!sendCommand(0x02, 0, 0))
    {
        util::printError("LCD Return Home Failed");
    }      
}

void UI::displayOn()
{
    //display on, cursor on, cursor pos on = 0x0F
    //display on, cursor on, cursor pos off = 0x0E
    //display on, cursor off, cursor pos on = 0x0E
    //display on, cursor on, cursor pos off = 0x0D
    //display on, cursor off, cursor pos off = 0x0C
    if (!sendCommand(0x0F, 0, 0))
    {
        util::printError("LCD Display on Failed");
    }
}

void UI::displayOff()
{
    if (!sendCommand(0x08, 0, 0))
    {
        util::printError("LCD Display off Failed");
    }   
}

void UI::setEntryMode(bool p)
{
    //or off completely = 0x04
    char cmd;
    if (p)
    {
        //cursor moves to right after write
        cmd = 0x06;
    }
    else
    {
        //cursor moves to left after write
        cmd = 0x05;
    }
    if (!sendCommand(cmd, 0, 0))
    {
        util::printError("LCD Display off Failed");
    }  
}

void UI::writeText(string text)
{
    clearDisplay();
    returnHome();
    sendCommand(0x80,0,0); //send DDRAM address to 0.
    
    int max;
    if (text.length() > 80)
    {
        max = 80;
    }
    else
    {
        max = text.length();
    }
    
    for (int i = 0; i<max; i++)
    {
        sendCommand((char)text[i],1,0);
    }
}