#ifndef SERIALIB_H
#define SERIALIB_H
 
 #include <sys/time.h>                                   // Used for TimeOut operations
 
 // Include for windows
 #if defined (_WIN32) || defined( _WIN64)
 #include <windows.h>                                // Accessing to the serial port under Windows
 #endif
 
 // Include for Linux
 #ifdef __linux__
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/shm.h>
 #include <termios.h>
 #include <string.h>
 #include <iostream>
 #include <fcntl.h>                                  // File control definitions
 #include <unistd.h>

 #endif
 
 class serialib
 {
 public:
     serialib    ();                                                 // Constructor
     ~serialib   ();                                                 // Destructor
 
     // ::: Configuration and initialization :::
 
     char    Open        (const char *Device,const unsigned int Bauds);      // Open a device
     void    Close();                                                        // Close the device
 
     // ::: Read/Write operation on characters :::
 
     char    WriteChar   (char);                                             // Write a char
     char    ReadChar    (char *pByte,const unsigned int TimeOut_ms=NULL);         // Read a char (with timeout)
 
     // ::: read/Write operation on strings :::
 
     char    WriteString (const void *String,long unsigned int NbBytes);                               // Write a string
     int     ReadString  (char  *String,char FinalChar,unsigned int MaxNbBytes,const unsigned int TimeOut_ms=NULL); // Read a string (with timeout)
 
     // ::: Read/Write operation on bytes :::
 
     char    Write       (const void *Buffer, const unsigned int NbBytes); // Write an array of bytes
     int     Read        (void *Buffer,unsigned int MaxNbBytes,const unsigned int TimeOut_ms=NULL);
 
 
 private:
     int     ReadStringNoTimeOut  (char *String,char FinalChar,unsigned int MaxNbBytes);             // Read a string (no timeout)
 
 
 #if defined (_WIN32) || defined( _WIN64)
     HANDLE          hSerial;
     COMMTIMEOUTS    timeouts;
 #endif
 #ifdef __linux__
     int             fd;
 #endif
};
 
 // Class TimeOut
 class TimeOut
 {
 public:
     TimeOut();                                                      // Constructor
     void                InitTimer();                                // Init the timer
     unsigned long int   ElapsedTime_ms();                           // Return the elapsed time since initialization
 private:
     struct timeval      PreviousTime;
 };
 
 #endif // SERIALIB_H
