/**
  ******************************************************************************
  *
  * COPYRIGHT(c) 2024 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/*Include main.h to get the MPU definitions from the used MCU (e.g. stm32L4xx_hal_cortex.h) */
#include "main.h"

void MPU_AddNullTrap(uint8_t mpu_region_size)
{ /* AN4838 defines tables in a code snippet but not adapted for max mem layout valid for Cortex-M4 also omitting SRAM2 on L4.
     For some specific STM32 areas could be reduced.              */
#if (__ARM_ARCH == 7) && (__MPU_PRESENT == 1) && (__CORTEX_M == 4)
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* Disable MPU */
    HAL_MPU_Disable();

    /* Every region we are setting we also want to enable: */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;

    /* Configure no access, no code execute permissions producing memory fault on access. */
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

    /* Use no access permissions for address 0 - (normally remapped to flash) to detect NULL pointer dereference.
       PM0214: "When the MPU is enabled, accesses to the System Control Space and vector table are
                always permitted."  => independent of VTOR
                Thus we can block the complete area when actually running from fixed flash address.
    */
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x00000000;
    MPU_InitStruct.Size = mpu_region_size; /* Protecting On STM32L4 after 128MB the Flash starts (RM0351 2.2.2)*/
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable MPU, keeping the default MPU mapping active by using MPU_PRIVILEGED_DEFAULT / PRIVDEFENA */
    /* If not using PRIVDEFENA then also regions for every used memory area need to be defined. Typical
       Memory region attributes in PM0214 4.2.4 */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
#endif
}
