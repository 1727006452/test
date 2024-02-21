#include "fmsh_ps_parameters.h"
#include "fmsh_nfcps_lib.h"

extern FNfcPs_Config_T FNfcPs_ConfigTable[];
/*****************************************************************************
* This function looks up the device configuration based on the unique device ID.
* The table s_ConfigTable contains the configuration info for each device
* in the system.
*
* @param	
*       - DeviceId contains the ID of the device for which the
*		device configuration pointer is to be returned.
*
* @return
*		- A pointer to the configuration found.
*		- NULL if the specified device ID was not found.
*
* @note		None.
*
******************************************************************************/
FNfcPs_Config_T* FNfcPs_LookupConfig(u16 deviceId)
{
    int index;
    FNfcPs_Config_T* cfgPtr = NULL;
    
    for (index = 0; index < FPS_NFC_NUM_INSTANCES; index++) {
        if (FNfcPs_ConfigTable[index].deviceId == deviceId) {
            cfgPtr = &FNfcPs_ConfigTable[index];
            break;
        }
    }
    return cfgPtr;
}