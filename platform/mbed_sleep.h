
/** \addtogroup platform */
/** @{*/
/* mbed Microcontroller Library
 * Copyright (c) 2006-2017 ARM Limited
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
 */
#ifndef MBED_SLEEP_H
#define MBED_SLEEP_H

#include "sleep_api.h"
#include "mbed_toolchain.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Sleep manager API
 * The sleep manager provides API to automatically select sleep mode.
 *
 * There are two sleep modes:
 * - sleep
 * - deepsleep
 *
 * Use locking/unlocking deepsleep for drivers that depend on features that
 * are not allowed (=disabled) during the deepsleep. For instance, high frequency
 * clocks.
 *
 * Example:
 * @code
 *
 * void driver::handler()
 * {
 *     if (_sensor.get_event()) {
 *         // any event - we are finished, unlock the deepsleep
 *         sleep_manager_unlock_deep_sleep();
 *         _callback();
 *     }
 * }
 *
 * int driver::measure(event_t event, callback_t& callback)
 * {
 *      _callback = callback;
 *      sleep_manager_lock_deep_sleep();
 *      // start async transaction, we are waiting for an event
 *      return _sensor.start(event, callback);
 * }
 * @endcode
 */

/** Lock the deep sleep mode
 *
 * This locks the automatic deep mode selection. 
 * sleep_manager_sleep_auto() will ignore deepsleep mode if
 * this function is invoked at least once (the internal counter is non-zero)
 *
 * Use this locking mechanism for interrupt driven API that are
 * running in the background and deepsleep could affect their functionality
 * 
 * The lock is a counter, can be locked up to USHRT_MAX
 * This function is IRQ and thread safe
 */
void sleep_manager_lock_deep_sleep(void);

/** Unlock the deep sleep mode
 *
 * Use unlocking in pair with sleep_manager_lock_deep_sleep(). 
 * 
 * The lock is a counter, should be equally unlocked as locked
 * This function is IRQ and thread safe
 */
void sleep_manager_unlock_deep_sleep(void);

/** Get the status of deep sleep allowance for a target
 *
 * @return true if a target can go to deepsleep, false otherwise
 */
bool sleep_manager_can_deep_sleep(void);

/** Enter auto selected sleep mode. It chooses the sleep or deeepsleep modes based
 *  on the deepsleep locking counter
 *
 * This function is IRQ and thread safe
 *
 * @note
 * If MBED_DEBUG is defined, only hal_sleep is allowed. This ensures the debugger
 * to be active for debug modes.
 * 
 */
void sleep_manager_sleep_auto(void);

/** Send the microcontroller to sleep
 *
 * @note This function can be a noop if not implemented by the platform.
 * @note This function will be a noop in debug mode (debug build profile when MBED_DEBUG is defined).
 * @note This function will be a noop while uVisor is in use.
 *
 * The processor is setup ready for sleep, and sent to sleep using __WFI(). In this mode, the
 * system clock to the core is stopped until a reset or an interrupt occurs. This eliminates
 * dynamic power used by the processor, memory systems and buses. The processor, peripheral and
 * memory state are maintained, and the peripherals continue to work and can generate interrupts.
 *
 * The processor can be woken up by any internal peripheral interrupt or external pin interrupt.
 *
 * @note
 *  The mbed interface semihosting is disconnected as part of going to sleep, and can not be restored.
 * Flash re-programming and the USB serial port will remain active, but the mbed program will no longer be
 * able to access the LocalFileSystem
 */
__INLINE static void sleep(void)
{
#if !(defined(FEATURE_UVISOR) && defined(TARGET_UVISOR_SUPPORTED))
#if DEVICE_SLEEP
    sleep_manager_sleep_auto();
#endif /* DEVICE_SLEEP */
#endif /* !(defined(FEATURE_UVISOR) && defined(TARGET_UVISOR_SUPPORTED)) */
}

/** Send the microcontroller to deep sleep
 *
 * @note This function can be a noop if not implemented by the platform.
 * @note This function will be a noop in debug mode (debug build profile when MBED_DEBUG is defined)
 * @note This function will be a noop while uVisor is in use.
 *
 * This processor is setup ready for deep sleep, and sent to sleep. This mode
 * has the same sleep features as sleep plus it powers down peripherals and clocks. All state
 * is still maintained.
 *
 * The processor can only be woken up by an external interrupt on a pin or a watchdog timer.
 *
 * @note
 *  The mbed interface semihosting is disconnected as part of going to sleep, and can not be restored.
 * Flash re-programming and the USB serial port will remain active, but the mbed program will no longer be
 * able to access the LocalFileSystem
 */

MBED_DEPRECATED_SINCE("mbed-os-5.6", "One entry point for an application, use sleep()")
__INLINE static void deepsleep(void)
{
#if !(defined(FEATURE_UVISOR) && defined(TARGET_UVISOR_SUPPORTED))
#if DEVICE_SLEEP
    sleep_manager_sleep_auto();
#endif /* DEVICE_SLEEP */
#endif /* !(defined(FEATURE_UVISOR) && defined(TARGET_UVISOR_SUPPORTED)) */
}

#ifdef __cplusplus
}
#endif

#endif

/** @}*/