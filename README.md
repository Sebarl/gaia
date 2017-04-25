Dependencias:

Para poder compilar exitosamente se requieren los siguientes paquetes:

*Las dependencias de Fceux (segun el manual de instalacion):
	-libsdl1.2 - It is strongly recommended that you upgrade to the latest
		version of sdl (1.2.15 at the time of writing).
	-scons - Required to build fceux.
	-libgtk2.0 (optional) - version >= 2.24 recommended
	-libgtk3.0 (optional) - this compiles as of fceux 2.2.0
	-liblua5.1 (optional)
	-minizip (optional) - you may chose to use the version of minizip on your system by enabling SYSTEM_MINIZIP in the SConstruct
	-c++ compiler -- you may use g++ from gcc or clang++ from llvm.

*OpenMP

*En caso de querer utilizar las funcionalidades para offload al coprocesador Xeon Phi debe instalarse el compilador de Intel y las librerias correspondientes para cross-compile a la arquitectura k1om

Instrucciones de compilacion:

1. Compilar Fceux headless utilizando el script install.sh en la carpeta Linux/FceuxHeadlessSrc
2. Compilar el visor utilizando el script install.sh en la carpeta Linux/FceuxGUI
3. Compilar la etapa de inferencia utilizando el Makefile o el proyecto de Codeblocks en la carpeta Linux/Analyzer
4. Compilar la etapa de refinamiento utilizando el Makefile o el proyecto de Codeblocks en la carpeta ObjectInfererAE
5. Compilar la etapa de generacion utilizando el Makefile o el proyecto de Codeblocks en la carpeta ANNTrainerAE

En las carpetas se encuentran presentes los binarios compilados bajo Fedora 23 para x86_64

