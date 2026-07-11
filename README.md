# Bluetooth Controlled Rover using AIROC BLE on PSoC 6

## Overview

This project extends an existing PSoC 6 rover by replacing UART-based motor control with Bluetooth Low Energy (BLE) communication. The objective was to control the rover wirelessly using the AIROC Bluetooth stack while maintaining the existing motor driver implementation.

The project was considerably more involved than simply adding Bluetooth functionality. Multiple initialization, build, linker, and architecture issues had to be resolved before a stable BLE connection could be established.

---

# Initial Rover

The rover originally consisted of:

* PSoC 6 (CY8CPROTO-063-BLE)
* DRV8874 motor driver
* Four motor control GPIO outputs
* UART console for testing

Motor commands were entered through the serial terminal:

```
w
a
s
d
b
```

These commands directly called the motor driver functions.

Although functional, the rover required a wired USB connection to receive commands.

---

# Goal

Replace UART command input with Bluetooth Low Energy while preserving the existing motor control code.

Desired control flow:

```
Phone
      ↓
Bluetooth LE
      ↓
PSoC 6
      ↓
Motor Control
```

---

# First Attempt

The original implementation attempted to add the AIROC Bluetooth stack directly into the existing rover project.

The following initialization sequence was implemented:

* Platform configuration
* IPC transport
* Bluetooth stack initialization
* Management callback
* GATT callback

Although

```
wiced_bt_stack_init()
```

returned success, the expected

```
BTM_ENABLED_EVT
```

never occurred.

Because this event never arrived:

* advertising never started
* GATT database was never registered
* phone could never discover the rover

Large amounts of debugging time were spent verifying:

* IPC transport
* CM0+/CM4 interaction
* platform configuration
* Bluetooth settings
* callback registration
* generated configuration files

The Bluetooth subsystem appeared partially initialized but never became operational.

---

# Change of Approach

Instead of continuing to debug the original project indefinitely, the decision was made to begin with an official Infineon Bluetooth example.

The "Bluetooth LE Hello Sensor" application was selected because it already demonstrated:

* Bluetooth stack startup
* advertising
* GATT database
* characteristic writes
* notifications

This isolated Bluetooth from the rest of the rover software.

---

# Verifying Bluetooth

The Hello Sensor application successfully produced:

* Bluetooth stack initialization
* BTM_ENABLED_EVT
* advertising
* successful phone connection
* GATT registration

This confirmed that:

* hardware was functional
* board configuration was correct
* Bluetooth stack was operating correctly

The issue therefore originated from the previous application architecture rather than the hardware.

---

# Understanding the GATT Database

Before modifying the example, time was spent understanding:

* Services
* Characteristics
* Handles
* UUIDs
* CCCD
* Notifications
* Write requests

The AIROC mobile application was used to manually:

* connect
* enable notifications
* write characteristic values

This established a complete understanding of how Bluetooth commands reached the firmware.

---

# Integrating Rover Control

Rather than redesigning the motor driver, the existing rover implementation was reused.

A new module was created:

```
motor_prim.c
motor_prim.h
```

This prevented duplicate symbol conflicts with earlier versions while preserving all original motor functions.

The Bluetooth write callback was modified to translate incoming bytes into motor commands.

Example mapping:

```
'w' → Forward
'a' → Left
's' → Reverse
'd' → Right
'b' → Stop
```

This allowed Bluetooth writes to invoke the same motor functions previously called by UART.

---

# Build and Integration Issues

Several software engineering problems were encountered during integration.

These included:

* duplicate symbol definitions
* conditional compilation errors
* missing bool definitions
* header dependency problems
* linker failures
* FreeRTOS include ordering
* project organization issues
* generated Bluetooth configuration dependencies

Each issue required restructuring source files until the Bluetooth example and rover code compiled together successfully.

---

# Final Result

The completed system performs the following sequence:

```
Phone

      ↓

Bluetooth LE Connection

      ↓

AIROC Stack

      ↓

GATT Write Callback

      ↓

Command Decoder

      ↓

Motor Driver

      ↓

Rover Motion
```

The rover now responds to Bluetooth commands instead of UART commands while preserving the original motor driver implementation.

---

# Lessons Learned

This project demonstrated that successful embedded Bluetooth development requires considerably more than simply enabling a communication library.

Important topics learned included:

* Bluetooth stack initialization
* BLE architecture
* GATT database design
* characteristic read/write operations
* notification mechanisms
* callback-driven firmware
* generated middleware configuration
* FreeRTOS integration
* project organization
* linker and build system debugging

Perhaps the most significant lesson was recognizing when to stop debugging an increasingly complex implementation and instead validate functionality using a known-good reference application before integrating custom software.

This approach ultimately resulted in a functioning Bluetooth-controlled rover while providing a much deeper understanding of the AIROC BLE software stack than would have been obtained by simply following a tutorial.
