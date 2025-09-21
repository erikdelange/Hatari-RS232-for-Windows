/* serial port echo task
 *
 * Connects to COMPORT and echos all incoming characters until receiving ctrl-z.
 *
 * Use with null-modem emulator com0com. Run this program and start a serial
 * PuTTY session connected to COM10.
 *
 * The program uses busy waiting. Microsoft advises use of the SetCommMask()
 * and WaitCommEvent() fucntions, which have been commented out in this example.
 *
 * 2025  K.W.E. de Lange
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define COMPORT "COM9"
#define HELLO "echo server\r\n"

char ComPortName[16];

int main(int argc, char **argv)
{
    char c;
    DCB dcb;
    HANDLE fh;
    COMSTAT Stat;
    DWORD Errors = 0;
    //    DWORD event = 0;

    strncpy(ComPortName, "\\\\.\\", sizeof(ComPortName) - 1);
    ComPortName[sizeof(ComPortName) - 1] = '\0';
    strncat(ComPortName, COMPORT, sizeof(ComPortName) - strlen(ComPortName) - 1);

    fh = CreateFile(ComPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (fh == INVALID_HANDLE_VALUE) {
        printf("CreateFile: error %lu\n", GetLastError());
        return -1;
    }

    SecureZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    if (GetCommState(fh, &dcb)) {
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        if (SetCommState(fh, &dcb)) {
            printf("SetCommState: %s port settings: baudrate=%lu, databits=%d, stopbits=%d, parity=%d\n", ComPortName, dcb.BaudRate,
                   dcb.ByteSize, dcb.StopBits, dcb.Parity);
        } else {
            printf("SetCommState: error %lu setting serial port parameters for %s\n", GetLastError(), ComPortName);
            return -1;
        }
    } else {
        printf("GetCommState: error %lu getting serial port parameters for %s\n", GetLastError(), ComPortName);
        return -1;
    }

    if (!WriteFile(fh, HELLO, sizeof(HELLO), NULL, NULL)) {
        printf("WriteFile: error %lu while writing '%s'\n", GetLastError(), HELLO);
        return -1;
    }

    //    SetCommMask(fh, EV_RXCHAR); /* only wait for incoming character */

    while (1) {
        //        WaitCommEvent(fh, &event, NULL); /* wait for incoming character(s) */

        ClearCommError(fh, &Errors, &Stat); /* get info on size of input queue */

        if (Stat.cbInQue > 1) /* only inform us if more than one byte waiting */
            printf("bytes waiting: %lu\n", Stat.cbInQue);

        while (Stat.cbInQue--) { /* as long as there are characters in the input queue */
            if (!ReadFile(fh, &c, 1, NULL, NULL)) {
                printf("ReadFile: error %lu\n", GetLastError());
                return -1;
            }

            printf("read %4d 0x%02x (%c)\n", c, c, isprint(c) ? c : '-');

            if (!WriteFile(fh, &c, 1, NULL, NULL)) {
                printf("WriteFile: error %lu while writing '%c'\n", GetLastError(), c);
                return -1;
            }

            if (c == 26) /* exit program on ctrl-z */
                return 0;
        }
    }

    CloseHandle(fh);

    return 0;
}
