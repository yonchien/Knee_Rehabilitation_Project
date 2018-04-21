#ifndef _DECA_SPI_H_
#define _DECA_SPI_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "deca_types.h"
#include "deca_port.h"




int writetospi                          // returns 0 for success, or, -1 for error.
(
    uint16       headerLength,          // input parameter - number of bytes header being written
    const uint8 *headerBuffer,          // input parameter - pointer to buffer containing the 'headerLength' bytes of header to be written
    uint32       bodylength,            // input parameter - number of bytes data being written
    const uint8 *bodyBuffer             // input parameter - pointer to buffer containing the 'bodylength' bytes od data to be written
);


int readfromspi                         // returns offset where requested data begins in supplied buffer, or, -1 for error.
(
    uint16       headerLength,          // input parameter - number of bytes header to write
    const uint8 *headerBuffer,          // input parameter - pointer to buffer containing the 'headerLength' bytes of header to write
    uint32       readlength,            // input parameter - number of bytes data being read
    uint8       *readBuffer             // input parameter - pointer to buffer containing to return the data (NB: size required = headerLength + readlength)
);


#ifdef __cplusplus
}
#endif

#endif
