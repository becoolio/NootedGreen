/*
 * SSDT-dGPU-Off.dsl
 *
 * Disables the NVIDIA dGPU (PEGP) at \_SB.PC00.PEG1 during _INI.
 * This prevents Lilu's processSwitchOff() from polling the service plane
 * for 60+ seconds waiting for the device to appear, which delays the
 * _cs_validate_page hook and causes DYLD patches to miss CoreDisplay.
 *
 * ACPI path confirmed from Lilu debug dump:
 *   PC00 (PCI root) -> PEG1 (bridge 0x8086, class 0x60400) -> PEGP (NVIDIA 0x10DE)
 */
DefinitionBlock ("", "SSDT", 2, "NGreen", "dGPUoff", 0x00001000)
{
    External (\_SB.PC00.PEG1, DeviceObj)
    External (\_SB.PC00.PEG1.PEGP, DeviceObj)
    External (\_SB.PC00.PEG1.PEGP._OFF, MethodObj)
    External (\_SB.PC00.PEG1.PEGP._PS3, MethodObj)
    External (\_SB.PC00.PEG1.PEGP._DSM, MethodObj)

    /* Disable via _STA override — report device as not present */
    Scope (\_SB.PC00.PEG1.PEGP)
    {
        Method (_STA, 0, NotSerialized)
        {
            Return (Zero)
        }
    }

    /* Also call _OFF/_PS3 early during system init if available */
    Scope (\_SB)
    {
        Device (DGOF)
        {
            Name (_HID, "DGOF0001")

            Method (_STA, 0, NotSerialized)
            {
                Return (0x0F)
            }

            Method (_INI, 0, NotSerialized)
            {
                If (CondRefOf (\_SB.PC00.PEG1.PEGP._OFF))
                {
                    \_SB.PC00.PEG1.PEGP._OFF ()
                }
                ElseIf (CondRefOf (\_SB.PC00.PEG1.PEGP._PS3))
                {
                    \_SB.PC00.PEG1.PEGP._PS3 ()
                }
            }
        }
    }
}
