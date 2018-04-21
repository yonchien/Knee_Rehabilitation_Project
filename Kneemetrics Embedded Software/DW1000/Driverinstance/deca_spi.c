#include "deca_spi.h"
#include "spi.h"


int writetospi                          // returns 0 for success, or, -1 for error.
(
    uint16       headerLength,          // input parameter - number of bytes header being written
    const uint8 *headerBuffer,          // input parameter - pointer to buffer containing the 'headerLength' bytes of header to be written
    uint32       bodylength,            // input parameter - number of bytes data being written
    const uint8 *bodyBuffer             // input parameter - pointer to buffer containing the 'bodylength' bytes od data to be written
)
{
	SetDW1000ChipSelLow();
	SendMultiByteToDW1000(headerBuffer, headerLength);
	SendMultiByteToDW1000(bodyBuffer, bodylength);
	SetDW1000ChipSelHigh();
	return 0;
}


int readfromspi                         // returns offset where requested data begins in supplied buffer, or, -1 for error.
(
    uint16       headerLength,          // input parameter - number of bytes header to write
    const uint8 *headerBuffer,          // input parameter - pointer to buffer containing the 'headerLength' bytes of header to write
    uint32       readlength,            // input parameter - number of bytes data being read
    uint8       *readBuffer             // input parameter - pointer to buffer containing to return the data (NB: size required = headerLength + readlength)
)
{
	SetDW1000ChipSelLow();
	SendMultiByteToDW1000(headerBuffer, headerLength);
	ReadMultiByteFromDW1000(readBuffer, readlength);
	SetDW1000ChipSelHigh();
	return 0;
}



#if 0
/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */
#pragma GCC optimize ("O3")
int writetospi
(
    uint16       headerLength,
    const uint8 *headerBuffer,
    uint32       bodylength,
    const uint8 *bodyBuffer
)
{

	int i=0;

    decaIrqStatus_t  stat ;

    stat = decamutexon() ;

    SPIx_CS_GPIO->BSRRH = SPIx_CS;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

    	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

    	SPIx->DR ;
    }

    for(i=0; i<bodylength; i++)
    {
     	SPIx->DR = bodyBuffer[i];

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

		SPIx->DR ;
	}

    SPIx_CS_GPIO->BSRRL = SPIx_CS;

    decamutexoff(stat) ;

    return 0;
} // end writetospi()


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: readfromspi()
 *
 * Low level abstract function to read from the SPI
 * Takes two separate byte buffers for write header and read data
 * returns the offset into read buffer where first byte of read data may be found,
 * or returns -1 if there was an error
 */
#pragma GCC optimize ("O3")
int readfromspi
(
    uint16       headerLength,
    const uint8 *headerBuffer,
    uint32       readlength,
    uint8       *readBuffer
)
{

	int i=0;
	uint8 temp;

    decaIrqStatus_t  stat ;

    stat = decamutexon() ;

    /* Wait for SPIx Tx buffer empty */
    //while (port_SPIx_busy_sending());

    SPIx_CS_GPIO->BSRRH = SPIx_CS;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

     	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

     	temp = SPIx->DR ;
    }

    for(i=0; i<readlength; i++)
    {
    	SPIx->DR = 0;

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

	   	readBuffer[i] = SPIx->DR ;//port_SPIx_receive_data(); //this clears RXNE bit
    }

    SPIx_CS_GPIO->BSRRL = SPIx_CS;

    decamutexoff(stat) ;

    return 0;
} // end readfromspi()
#endif
