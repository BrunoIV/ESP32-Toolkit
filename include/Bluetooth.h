#ifndef BLUETOOTH_H
#define BLUETOOTH_H

class Bluetooth {
public:
    static void spamBLE();
    static void enable();
    static void disable();
    static bool isRunning();

private:
    static bool run;
};

#endif
