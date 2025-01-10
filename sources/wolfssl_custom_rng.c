#include "cy_pdl.h"


/*******************************************************************************
* Macros
*******************************************************************************/

//Polynomials for programmable.
#define CY_CONFIG_TR_GARO_CTL      0x6C740B8DuL //Galois ring oscillator.
#define CY_CONFIG_TR_FIRO_CTL      0x52D246E1uL // Fibonacci ring oscillator.
#define CY_TR_MAX_BITS             32U

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

int Cy_Crypto_Gen_RNG_Block(unsigned char* output, unsigned int sz);

/*******************************************************************************
 * Function Name: Cy_Crypto_Gen_RNG_Block
 ***************************************************************************//**
 * Summary: 
 * It Generate True Random Number based on the size given using the
 * Cy_Crypto_Core_Trng() Function and Stores the Generated random value 
 * into the output poointer.
 * 
 * Parameters:
 * \param output
 * The pointer to a generated true random number
 * 
 * \param sz
 * The maximum length of a random number
 * 
 * Return:
 *  int
 * \return
 * \ref cy_en_crypto_status_t
 */

//Function name
int Cy_Crypto_Gen_RNG_Block(unsigned char* output, unsigned int sz){
    cy_en_crypto_status_t result = CY_CRYPTO_SUCCESS;

    for (; (sz >= 4) && (result = CY_CRYPTO_SUCCESS); sz -= 4)
    {
        result = Cy_Crypto_Core_Trng(CRYPTO, CY_CONFIG_TR_GARO_CTL, CY_CONFIG_TR_FIRO_CTL, CY_TR_MAX_BITS, output);
        output += 4;
    }

    if ((result = CY_CRYPTO_SUCCESS) && (sz > 0))
    {
        result = Cy_Crypto_Core_Trng(CRYPTO, CY_CONFIG_TR_GARO_CTL, CY_CONFIG_TR_FIRO_CTL, sz * 8, output);
    }

    return result;
}