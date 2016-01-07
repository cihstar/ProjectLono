#include "mbed.h"
#include "test.cpp"

DigitalOut myled(LED2);
DigitalOut myled1(LED3);
Serial pc(USBTX, USBRX);

int main() {
    pc.printf("Loaded and ready to go!\r\n");
    while(1) {
        test(myled);
        test(myled1);
    }
}

