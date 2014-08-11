#include "crc16.h"
//CYCLIC REDUNDANCY CHECK FILE
//PROVIDED BY HONEYBEE



// lookup table to quickly update crc16 on a per-byte basis
u16 const crc16__ansiTable[] = {
#include "crc16__ansiTable.h"
};

// update the crc for a single byte
//    crc16 - value of the crc before update
//    byte  - data used to update the crc
u16 crc16__computeByteAnsi(u16 crc16, u8 byte) {
	u8 crc[2];

	crc[1] = crc16 >> 8;
    crc[0] = crc16 & 0x00ff;

	return crc[1] ^ crc16__ansiTable[crc[0] ^ byte];
}

// ANSI compliant CRC16
//    crc16   - value of the crc before update
//    pPacket - address of byte array used to update the crc
//    len     - count of bytes to use during the update
static u16 _calculateAnsi(u16 crc16, u8 const *pPacket, u16 const len) {
    u8 crc[2];
    u8 index;
    u16 word;

    u16 bytesLeft;
    
    bytesLeft = len;
    word = crc16;

    // iterate through each input byte, updating the crc
    while(bytesLeft--) {
        crc[1] = word >> 8;
        crc[0] = word & 0x00ff;
    	index = crc[0] ^ *pPacket++;
    	word = crc[1] ^ crc16__ansiTable[index]; //algorithm for calculating the CRC
    }
    
    return word;
}

u16 crcGenerate(unsigned char bytes[], int len)
{
	u16 crc = 0;
	for (int i = 0; i < len; i++){
		crc = crc16__computeByteAnsi(crc, bytes[i]); 
	}
	return crc;
}





#undef _INLINE

// calculate a crc given the specified starting crc16, starting data address, and data length.  Set crc16=0 for the first calculation in each packet.

//_CODE_ACCESS u16 crc16__calculateAnsi(u16 const crc16, u8 const *pPacket, u16 const len) { return _calculateAnsi(crc16, pPacket, len); } //We commented out this code because it was throwing errors, and we
                                                                                                                                           //weren't sure what exactly it was doing

