# Modbus Remote Angle Change Notes

Commit: `7448c90`

## Summary

This change separates Modbus register upload from angle control logic.

- `mode=0/1/2/3`: `SoftStart()` keeps control of `StartParams.Data/OutData`
- `mode=4`: DSP executes the angle value received from the upper computer through Modbus

## Files Changed

- `src/MainCont.c`
- `src/T1_CTRL.c`
- `src/Uart1ModBus.c`

## Main Fixes

- Added `Apply_Modbus_RemoteAngle()` and called it after `Modbus_Slave_App()`
- Removed the unconditional overwrite of `StartParams.Data/OutData` from `Update_Modbus_Registers()`
- Fixed Modbus reset command path so it uses the actual fault reset flow
- Added frame length validation for function `0x10`
- Changed Modbus frame-end timeout from fixed `10ms` to a baud-rate-based calculation

## Build Status

- Source files compiled successfully
- Project link completed successfully and generated `mps_dsp_V19.79.cof`

## Important Note

This program has **not** been debugged on real hardware.
This change is only compile-verified and has **not** been field-tested.
