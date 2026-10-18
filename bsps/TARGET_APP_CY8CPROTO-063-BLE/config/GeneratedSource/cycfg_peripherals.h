/*******************************************************************************
 * File Name: cycfg_peripherals.h
 *
 * Description:
 * Clock configuration
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.80.0
 * device-db 4.41.0.11271
 * mtb-pdl-cat1 3.24.0.47219
 *
 *******************************************************************************
 * Copyright 2026, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#if !defined(CYCFG_PERIPHERALS_H)
#define CYCFG_PERIPHERALS_H

#include "cycfg_notices.h"

#if defined (CY_USING_HAL)
#include "cyhal_hwmgr.h"
#endif /* defined (CY_USING_HAL) */

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define CYBSP_BLE_ENABLED 1U
#define CY_BLE_CORE_CORTEX_M4 4U
#define CY_BLE_CORE_CORTEX_M0P 0U
#define CY_BLE_CORE_DUAL 255U
#ifndef CY_BLE_CORE
    #define CY_BLE_CORE 4U
#endif
#define CY_BLE_IRQ bless_interrupt_IRQn

#if defined (CY_USING_HAL)
extern const cyhal_resource_inst_t CYBSP_BLE_obj;
#endif /* defined (CY_USING_HAL) */

void reserve_cycfg_peripherals(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_PERIPHERALS_H */
