In the Linux version of Atari emulator [Hatari](https://www.hatari-emu.org/index.html) you can specify a device file to be used for serial communication. On Windows this does not work, only a 'real' file can be used for serial input and output. The software changes presented in this repository remove this restriction and enable you to connect Hatari to a Windows COM port.

For accessing a COM port on Windows the fopen/fread/fwrite/fclose functions have been replaced with their equivalents from the Win32 API. All the info I used was retrieved from [About Communications Resources](https://learn.microsoft.com/en-us/windows/win32/devio/about-communications-resources). Functional limitation compared to the Linux version is that the same COM port is used for both input and output.

The changes have been tested with a serial device (a microcontroller) connected to the PC via an USB to serial converter. It also works when you use a terminal emulator such as PuTTY to communicate with Hatari with both programs running on the same PC. This relies on null modem emulator [*com0com*](https://com0com.sourceforge.net/) which creates pairs of connected COM ports. In the default installation of com0com these are COM9 and COM10. Connect Hatari to COM9 and PuTTY to COM10 and the two programs can communicate.

Stand-alone program *echo.c* demonstrates the use of the Win32 API functions for serial communication with a bit of error printing/handling. This was the foundation for the changes to Hatari.

#### Notes

Make sure option *Patch Timer-D* is disabled in the System options of Hatari. If not then Hatari's function to change the baud rate (which uses Timer D) will not work. For convenience the COM port is initialized at 9600 baud 8N1.

Only file rs232.c - taken from the Hatari version 2.6.1 repository - is modified, and only lines have been added (see 'diff' with rs232_original.c which is the unchanged version).

The modified code will only appear in a Windows build of Hatari (checked via macro WIN32) and only if macro ENABLECOMPORT has been defined in file rs232.c.

For now the two values which control whether to use a COM port (bool UseComPort) and which comport to use (#define COMPORT) are hard-coded in rs232.c. Ideally these would be entries in (the Windows version of) the Hatari configuration file. The comments in function RS232_Init() explain why I failed to get this working.

The name of the device file for a Windows COM port is specified as \\\\.\\COMx, where x is a number. Because in C a \\ indicates the start of an escape sequence the device file name looks like "\\\\\\\\.\\\\COMx".

Building Hatari was done using WSL and using the instructions found here: http://clarets.org/steve/projects/20201220\_building\_hatari\_for\_windows.html. These worked perfectly except that LibSDL has moved to GitHub.
