The Linux version of Atari emulator [Hatari](https://www.hatari-emu.org/index.html) supports serial communication via a device file. On Windows this does not work, only a 'real' file can be used for serial input and output. The software changes presented in this repository remove this restriction and enable connecting Hatari to a Windows COM port.

To access a COM port on Windows the Win32 API must be used. The fopen/fread/fwrite/fclose functions will not work, and have been replaced with their equivalents from the Win32 API. Info on serial communication via the Win32 API was retrieved from [About Communications Resources](https://learn.microsoft.com/en-us/windows/win32/devio/about-communications-resources). Functional limitation of the solution presented compared to the Linux version is that the same COM port is used for both input and output.

The changes have been tested with a serial device (a microcontroller) connected to the PC via an USB to serial converter. It also works when using a terminal emulator such as PuTTY to communicate with Hatari with both programs running on the same PC. This relies on null modem emulator [*com0com*](https://com0com.sourceforge.net/) which creates pairs of connected COM ports. In the default installation of com0com these are COM9 and COM10. Connect Hatari to COM9 and PuTTY to COM10 and the two programs can communicate.

Stand-alone program *echo.c* demonstrates the use of the Win32 API functions for serial communication with a bit of error printing/handling. This was the foundation for the changes to Hatari.

#### Notes

Only file *rs232.c* - taken from the Hatari version 2.6.1 repository - is modified, and only lines have been added (do 'diff' with *rs232_original.c* which is the unchanged version).

The modified code will only be present in a Windows build of Hatari (checked via macro WIN32) and only if macro ENABLECOMPORT has been defined in file *rs232.c*.

For now the variable which controls whether to use a COM port (bool UseComPort) and which comport to use (#define COMPORT) are hard-coded in *rs232.c*. Ideally these would be entries in (the Windows version of) the Hatari configuration file. The comments in function RS232_Init() explain the problems in getting this to work.

The name of the device file for a Windows COM port is specified as \\\\.\\COMx, where x is a number. Because in C a \\ indicates the start of an escape sequence the device file name looks like "\\\\\\\\.\\\\COMx". See function RS232_Init() how this is done.

You have to build your own version of Hatari. I used WSL and the instructions found here: http://clarets.org/steve/projects/20201220_building_hatari_for_windows.html. These worked perfectly except that LibSDL has moved to GitHub. And of course don't forget to replace file *rs232.c* with the one from this repository.

When running Hatari make sure option *Patch Timer-D* is disabled in the System options. If not then Hatari's function to change the baud rate (which uses Timer D) will not work. The COM port is initialized at 9600 baud 8N1.
