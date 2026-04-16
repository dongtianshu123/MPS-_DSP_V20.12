$ErrorActionPreference = "Stop"

$workspace = Split-Path -Parent $PSScriptRoot
Set-Location $workspace

$toolRoot = "C:\Program Files (x86)\Microchip\mplabc30\v3.25\bin"
$cc = Join-Path $toolRoot "pic30-gcc.exe"
$hex = Join-Path $toolRoot "pic30-bin2hex.exe"

if (-not (Test-Path $cc)) {
    throw "pic30-gcc.exe not found: $cc"
}

if (-not (Test-Path $hex)) {
    throw "pic30-bin2hex.exe not found: $hex"
}

$commonArgs = @(
    "-mcpu=30F5011",
    "-I.\h",
    "-D__DEBUG",
    "-D__MPLAB_DEBUGGER_PK3=1",
    "-g",
    "-Wall"
)

$compileUnits = @(
    @{ Source = "src\ADInterrupt.c"; Object = "ADInterrupt.o" },
    @{ Source = "src\Current.c"; Object = "Current.o" },
    @{ Source = "src\Delays.c"; Object = "Delays.o" },
    @{ Source = "src\init5011.c"; Object = "init5011.o" },
    @{ Source = "src\IO.c"; Object = "IO.o" },
    @{ Source = "src\MainCont.c"; Object = "MainCont.o" },
    @{ Source = "src\PhyVol.c"; Object = "PhyVol.o" },
    @{ Source = "src\Protect1.c"; Object = "Protect1.o" },
    @{ Source = "src\RelayFlow.c"; Object = "RelayFlow.o" },
    @{ Source = "src\T1_CTRL.c"; Object = "T1_CTRL.o" },
    @{ Source = "src\Uart1ModBus.c"; Object = "Uart1ModBus.o" },
    @{ Source = "src\Uart2.c"; Object = "Uart2.o" }
)

foreach ($unit in $compileUnits) {
    & $cc @commonArgs -x c -c $unit.Source -o $unit.Object
    if ($LASTEXITCODE -ne 0) {
        throw "Compile failed: $($unit.Source)"
    }
}

& $cc -mcpu=30F5011 -c -I.\h -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 "src\DataEEPROM.s" -o "DataEEPROM.o" "-Wa,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g"
if ($LASTEXITCODE -ne 0) {
    throw "Assemble failed: src\DataEEPROM.s"
}

$objects = @(
    "ADInterrupt.o",
    "Current.o",
    "DataEEPROM.o",
    "Delays.o",
    "init5011.o",
    "IO.o",
    "MainCont.o",
    "PhyVol.o",
    "Protect1.o",
    "RelayFlow.o",
    "T1_CTRL.o",
    "Uart1ModBus.o",
    "Uart2.o"
)

$linkerFlags = '-Wl,-L"C:\Program Files (x86)\Microchip\mplabc30\v3.25\lib",-Tp30F5011.gld,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=__ICD2RAM=1,-Map="mps_dsp_V19.79.map",--report-mem'
& $cc -mcpu=30F5011 @objects -o "mps_dsp_V19.79.cof" $linkerFlags
if ($LASTEXITCODE -ne 0) {
    throw "Link failed"
}

& $hex "mps_dsp_V19.79.cof"
if ($LASTEXITCODE -ne 0) {
    throw "HEX generation failed"
}

Write-Host "Build completed: mps_dsp_V19.79.cof / mps_dsp_V19.79.hex"

