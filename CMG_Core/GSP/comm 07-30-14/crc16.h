#ifndef _crc16_H
#define _crc16_H

#include "typedef.h"


typedef union _word16 {
    u16 word;
    u8  byte[2];
} t_word16;

u16 crcGenerate(unsigned char bytes[], int len);


// ANSI compliant CRC16
#ifdef _INLINE


 static inline u16 crc16__calculateAnsi(u8 const *const pPacket, u16 const len);
 static inline u16 crc16__calculateAnsi(u8 const *const pPacket, const u16 len) { return _calculateAnsi(pPacket, len); }
#else
 //extern _CODE_ACCESS u16  crc16__calculateAnsi(u16 crc16, u8 const *const pPacket, u16 const len); //Commented out b/c it was throwing errors and we weren't sure if it was a necessary component of the program
#endif
u16 crc16__computeByteAnsi(u16 crc16, u8 byte);
#endif

// To generate a CRC, use the following pseudocode
//    
//    u16 crc = 0;
//    for(int i=0; i<bytes.length; i++)
//      crc = crc16__computeByteAnsi(crc, bytes[i]);
//
// If the data to be protected consists of multiple
// buffers, use the folling pseudocode
//
//    int i;
//    u16 crc = 0;
//    for(i=0; i<headerBytes.length; i++)
//      crc = crc16__computeByteAnsi(crc, headerBytes[i]);
//    for(i=0; i<payloadBytes.length; i++)
//      crc = crc16__computeByteAnsi(crc, payloadBytes[i]);
//    for(i=0; i<footerBytes.length; i++)
//      crc = crc16__computeByteAnsi(crc, footerBytes[i]);
//
//  A more readable way to do the same appears in the
//  following example.
//
//    u16 crc = 0;
//    crc = crc16__calculateAnsi(crc, bytes, bytes.length);
//
//  To process multiple buffers in the more readable way:
//
//    u16 crc = 0;
//    crc = crc16__calculateAnsi(crc, headerBytes,  headerBytes.length);
//    crc = crc16__calculateAnsi(crc, payloadBytes, payloadBytes.length);
//    crc = crc16__calculateAnsi(crc, footerBytes,  footerBytes.length);
//
