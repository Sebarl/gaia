# Gaia
Dependencias:

Para poder compilar exitosamente se requieren los siguientes paquetes:

* Las dependencias de Fceux (segun el manual de instalación):
	
		-libsdl1.2 - es recomendable actualizar a la última versión disponible de SDL (1.2.15 al momento de escribir estas líneas).
		-scons - requerido para compilar FCEUX.
		-libgtk2.0 (opcional) - versión >= 2.24 recomendada
		-libgtk3.0 (opcional)
		-liblua5.1 (opcional)
		-minizip (opcional) - es posible utilizar la versión de minizip instalada en el sistema habilitando SYSTEM_MINIZIP en el archivo SConstruct
		-compilador c++ - puede usarse g++ de gcc o clang++ de llvm

* OpenMP

* En caso de querer utilizar las funcionalidades para offload al coprocesador Xeon Phi debe instalarse el compilador de Intel y las librerías correspondientes para cross-compilar a la arquitectura k1om

Instrucciones de compilación:

1. Compilar Fceux headless utilizando el script install.sh en la carpeta Linux/FceuxHeadlessSrc
2. Compilar el visor utilizando el script install.sh en la carpeta Linux/FceuxGUI
3. Compilar la etapa de inferencia utilizando el Makefile o el proyecto de Codeblocks en la carpeta Linux/Analyzer
4. Compilar la etapa de refinamiento utilizando el Makefile o el proyecto de Codeblocks en la carpeta ObjectInfererAE
5. Compilar la etapa de generación utilizando el Makefile o el proyecto de Codeblocks en la carpeta ANNTrainerAE

En las carpetas se encuentran presentes los binarios compilados bajo Fedora 23 para x86_64

