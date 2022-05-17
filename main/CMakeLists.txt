set(srcs
	"Main.c"
	"./UI/st7789.c"
	"./Lora/Lora.c"
	)

idf_component_register(SRCS ${srcs}
    INCLUDE_DIRS ".")
