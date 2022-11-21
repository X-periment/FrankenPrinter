; Code by Xperiment November 2022
; first 5 (and last 3) includes came from  https://www.sinclairzxworld.com/viewtopic.php?t=2186&start=40 via ByteForever
#include "zx81defs.asm"
#include "zx81rom.asm"
#include "charcodes.asm"
#include "zx81sys.asm"
#include "line1.asm"

    jp demoStart

#include "Lib_general.asm"


demoStart


    ld de, (D_FILE)
    inc de
    inc de

endLoop
    inc de
    ld bc, $fefe
    in a, (c)
    xor $ff
    ld (de), a

    ld de, (D_FILE)
    inc de

    ld h, 16
hloop

    ld l, 32
lineLoop
    ld bc, $ffef  ; $fefe  $ffef
    in a, (c)
    cp $ff
    jr nz, canPrint
    ld a, 20
canPrint
    ld (de), a

    inc de
    ld a, $01
    call delay2
    dec l

    jr nz, lineLoop

    inc de
    dec h
    jr nz, hloop

    jr demoStart




#include "from_brandonw.asm"

#include "line2.asm"
#include "screenBig1k.asm"      			; definition of the screen memory, in colapsed version for 1K
#include "endbasic.asm"
