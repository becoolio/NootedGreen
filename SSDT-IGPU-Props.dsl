/*
 * SSDT-IGPU-Props.dsl
 *
 * Injects AAPL,ig-platform-id and related properties into GFX0 (IGPU)
 * before Lilu device scan, preventing "frame without connectors" headless fallback.
 *
 * ACPI path from Lilu debug:
 *   \_SB.PC00.GFX0 (Intel iGPU, class 0x30000)
 */

DefinitionBlock ("", "SSDT", 2, "NGreen", "IGPUPRP", 0x00001000)
{
    External (\_SB.PC00.GFX0, DeviceObj)

    Scope (\_SB.PC00.GFX0)
    {
        /* _DSM returns actual property keys queried by macOS/Lilu. */
        Method (_DSM, 4, NotSerialized)
        {
            If (!Arg2)
            {
                Return (Buffer () { 0x03 })
            }

            Return (Package ()
            {
                "AAPL,ig-platform-id", Buffer () { 0x00, 0x00, 0x49, 0x9A },
                "device-id",           Buffer () { 0x49, 0x9A, 0x00, 0x00 },
                "built-in",            Buffer () { 0x00 },
                "AAPL,slot-name",      "built-in",
                "hda-gfx",             "onboard-1"
            })
        }
    }
}
