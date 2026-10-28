	.section .rodata
	.balign 4
	.global rom_inicio
	.global rom_fin
rom_inicio:
	.incbin "build/juego.gba"
rom_fin:
	.section .note.GNU-stack,"",@progbits
