#include "ui.h"
#include "modules.h"
#include "rtos.h"

UI::UI(PinName sda, PinName scl, PinName rst, PinName b1, PinName b2, PinName b3):
dbuttons{b1,b2,b3}, reset(rst), menuActive(false), activeMenuItem(-1), timer(timerStarter, osTimerPeriodic, this)
{
    reset = 1;  
    lcd = new SB1602E(p9,p10);   
    
    dbuttons[0].attach_asserted(this, &UI::button1Push);
    dbuttons[1].attach_asserted(this, &UI::button2Push);
    dbuttons[2].attach_asserted(this, &UI::button3Push);
    
    dbuttons[0].setSampleFrequency();
    dbuttons[1].setSampleFrequency();
    dbuttons[2].setSampleFrequency();
        
    menuItems.push_back("Status");
    menuItems.push_back("History");
    menuItems.push_back("Battery");
    menuItems.push_back("Wireless");
    menuItems.push_back("Pressure Sensor");         
    
    timer.start(SCREEN_TIMEOUT);
}

void UI::timerStarter(void const* p)
{
    UI *instance = (UI*)p;
    instance->timerTask();
}

void UI::timerTask()
{
    screenOff();
    menuActive = false;
}

void UI::screenOn()
{
    lcd->contrast(0x35);
    timer.start(SCREEN_TIMEOUT);
}

void UI::screenOff()
{
    lcd->contrast(0);
    timer.stop();
}

void UI::showMenu()
{
    screenOn();
    writeText("Menu...");
    activeMenuItem = -1;
    menuActive = true;
    Thread::wait(3000); 
    menuUp();
}

UI::~UI()
{
    delete lcd;
}

void UI::writeText(string line1, string line2)
{    
    lcd->clear();
    lcd->puts(0,line1.c_str());
    if (line2 != "")
    {
        lcd->puts(1,line2.c_str());
    }
}

void UI::menuUp()
{    
    if ( (activeMenuItem + 1) < (int) menuItems.size())
    {
        activeMenuItem++;
        writeText(menuItems[activeMenuItem]);
    }
}

void UI::menuDown()
{    
    if ( (activeMenuItem - 1) > -1)
    {
        activeMenuItem--;
        writeText(menuItems[activeMenuItem]);
    }
}

void UI::button1Push()
{            
    timer.start(SCREEN_TIMEOUT);            
    if (!menuActive)
    {
        showMenu();
    }   
    else
    {
        menuDown();
    }      
}

void UI::button2Push()
{
    timer.start(SCREEN_TIMEOUT);   
    if (!menuActive)
    {
        showMenu();
    }  
    else
    {
        if (activeMenuItem == 0)
        {
            //Status
            menuActive = false;
            printStatus();
            showMenu();
        }
    } 
}

void UI::button3Push()
{
    timer.start(SCREEN_TIMEOUT);    
    if (!menuActive)
    {
        showMenu();
    }   
    else
    {
        menuUp();
    }
}

void UI::printStatus()
{
    int t = 5000;
    string str;
    
    if (!menuActive)
    {
        writeText("Device Status");
    }
    //wait(5);
    Thread::wait(t);
    
    str = modules::pressureSensor->getActive() ? "Active" : "Disabled";
    if (!menuActive)
    {
        writeText("Pressure Sensor", str);
    }
    Thread::wait(t);
    
    str = modules::pressureSensor->getLastReading();
    if (!menuActive)
    {
        writeText("Last Reading", str);
    }
    Thread::wait(t);
    
    str = util::getTimeStamp();
    if (!menuActive)
    {
        writeText("System Time", str);
    }   
    Thread::wait(t);
    
    str = Wireless::getConnectionModeString();  
    if (!menuActive)
    {  
        writeText("Wireless Mode", str);
    }
    Thread::wait(t);
    
    
}