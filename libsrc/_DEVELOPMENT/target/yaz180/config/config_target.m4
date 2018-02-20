divert(-1)

###############################################################
# TARGET USER CONFIGURATION
#
# rebuild the library if changes are made
#

# Announce target

define(`__YAZ180', 1)

# Clock frequency in Hz

define(`__CPU_CLOCK', 18432000)

# I/O BASE ADDRESS OF INTERNAL PERIPHERALS

define(`__IO_BASE_ADDRESS', 0x00)

define(`__IO_BASE_ADDRESS', 0x`'eval(__IO_BASE_ADDRESS&0xc0,16))

# All I/O drivers are passed through system buffers
# This will help when user banking is used, as all I/O is via system RAM

define(`__COMMON_AREA_1_BASE', 0xf000)              # Base address of COMMON AREA 1
define(`__COMMON_AREA_1_PHASE_DATA_SIZE', 0x0600)   # Size allowance for COMMON AREA 1 PHASE DATA

define(`__COMMON_AREA_1_PHASE_DATA', 0x`'eval(__COMMON_AREA_1_BASE,16))    # Origin of COMMON AREA 1 Data
define(`__COMMON_AREA_1_PHASE_DRIVER', 0x`'eval(__COMMON_AREA_1_BASE+__COMMON_AREA_1_PHASE_DATA_SIZE,16))  # Origin of COMMON AREA 1 Drivers, after CA1 Data

# Z180 ASCI0 driver

define(`__ASCI0_RX_SIZE', 0x100)  # Size of the Rx Buffer
define(`__ASCI0_TX_SIZE', 0x080)  # Size of the Tx Buffer

# Z180 ASCI1 driver

define(`__ASCI1_RX_SIZE', 0x100)  # Size of the Rx Buffer
define(`__ASCI1_TX_SIZE', 0x080)  # Size of the Tx Buffer

# BREAK for Single Step Mode

define(`__IO_BREAK', 0x2000)      # Any value written $2000->$3FFF, halts CPU

# 82C55 PIO Definitions

define(`__IO_PIO_PORT_BASE', 0x4000)        # Base Address for 82C55

# PCA9665 I2C Definitions

define(`__IO_PCA9665_1_PORT_BASE', 0xA000)  # Base Address for PCA9665 1 I/O
define(`__IO_PCA9665_2_PORT_BASE', 0x8000)  # Base Address for PCA9665 2 I/O

# Am9511A-1 APU Definitions

define(`__APU_CMD_SIZE', 0x100)   # Size of the CMD Buffer, 256 CMDs
define(`__APU_DATA_SIZE', 0x100)  # Size of the DATA Buffer

define(`__IO_APU_PORT_BASE', 0xC000)        # Base Address for Am9511A

#
# END OF USER CONFIGURATION
###############################################################

divert(0)

dnl#
dnl# COMPILE TIME CONFIG EXPORT FOR ASSEMBLY LANGUAGE
dnl#

ifdef(`CFG_ASM_PUB',
`
PUBLIC `__YAZ180'

PUBLIC `__CPU_CLOCK'

PUBLIC `__IO_BASE_ADDRESS'

PUBLIC `__COMMON_AREA_1_BASE'
PUBLIC `__COMMON_AREA_1_PHASE_DATA'
PUBLIC `__COMMON_AREA_1_PHASE_DRIVER'

PUBLIC `__ASCI0_RX_SIZE'
PUBLIC `__ASCI0_TX_SIZE'

PUBLIC `__ASCI1_RX_SIZE'
PUBLIC `__ASCI1_TX_SIZE'

PUBLIC `__IO_BREAK'

PUBLIC `__IO_PIO_PORT_BASE'

PUBLIC `__IO_PCA9665_1_PORT_BASE'
PUBLIC `__IO_PCA9665_2_PORT_BASE'

PUBLIC `__APU_CMD_SIZE'
PUBLIC `__APU_DATA_SIZE'

PUBLIC `__IO_APU_PORT_BASE'
')

dnl#
dnl# LIBRARY BUILD TIME CONFIG FOR ASSEMBLY LANGUAGE
dnl#

ifdef(`CFG_ASM_DEF',
`
defc `__YAZ180' = __YAZ180

defc `__CPU_CLOCK' = __CPU_CLOCK

defc `__IO_BASE_ADDRESS' = __IO_BASE_ADDRESS

defc `__COMMON_AREA_1_BASE' = __COMMON_AREA_1_BASE
defc `__COMMON_AREA_1_PHASE_DATA' = __COMMON_AREA_1_PHASE_DATA
defc `__COMMON_AREA_1_PHASE_DRIVER' = __COMMON_AREA_1_PHASE_DRIVER

defc `__ASCI0_RX_SIZE' = __ASCI0_RX_SIZE
defc `__ASCI0_TX_SIZE' = __ASCI0_TX_SIZE

defc `__ASCI1_RX_SIZE' = __ASCI1_RX_SIZE
defc `__ASCI1_TX_SIZE' = __ASCI1_TX_SIZE

defc `__IO_BREAK' = __IO_BREAK

defc `__IO_PIO_PORT_BASE' = __IO_PIO_PORT_BASE

defc `__IO_PCA9665_1_PORT_BASE' = __IO_PCA9665_1_PORT_BASE
defc `__IO_PCA9665_2_PORT_BASE' = __IO_PCA9665_2_PORT_BASE

defc `__APU_CMD_SIZE' = __APU_CMD_SIZE
defc `__APU_DATA_SIZE' = __APU_DATA_SIZE

defc `__IO_APU_PORT_BASE' = __IO_APU_PORT_BASE
')

dnl#
dnl# COMPILE TIME CONFIG EXPORT FOR C
dnl#

ifdef(`CFG_C_DEF',
`
`#undef'  `__YAZ180'
`#define' `__YAZ180'  __YAZ180

`#define' `__CPU_CLOCK'  __CPU_CLOCK

`#define' `__IO_BASE_ADDRESS' __IO_BASE_ADDRESS

`#define' `__COMMON_AREA_1_BASE'  __COMMON_AREA_1_BASE
`#define' `__COMMON_AREA_1_PHASE_DATA'  __COMMON_AREA_1_PHASE_DATA
`#define' `__COMMON_AREA_1_PHASE_DRIVER'  __COMMON_AREA_1_PHASE_DRIVER

`#define' `__ASCI0_RX_SIZE'  __ASCI0_RX_SIZE
`#define' `__ASCI0_TX_SIZE'  __ASCI0_TX_SIZE

`#define' `__ASCI1_RX_SIZE'  __ASCI1_RX_SIZE
`#define' `__ASCI1_TX_SIZE'  __ASCI1_TX_SIZE

`#define' `__IO_BREAK'  __IO_BREAK

`#define' `__IO_PIO_PORT_BASE'  __IO_PIO_PORT_BASE

`#define' `__IO_PCA9665_1_PORT_BASE'  __IO_PCA9665_1_PORT_BASE
`#define' `__IO_PCA9665_2_PORT_BASE'  __IO_PCA9665_2_PORT_BASE

`#define' `__APU_CMD_SIZE'  __APU_CMD_SIZE
`#define' `__APU_DATA_SIZE'  __APU_DATA_SIZE

`#define' `__IO_APU_PORT_BASE'  __IO_APU_PORT_BASE
')
