Comandos extra Fceux

Este fork posee los siguientes parametros extra:

Fceux full: -e | --ae-settings : especifica la ruta del archivo de configuracion de IA.so, por defecto: ae.txt
            -l | --frame-limit : numero maximo de frames a emular antes de cerrar el emulador, por defecto: -1
            -I | --enable-ia : habilitar o no la IA, (0 - off, 1 - on) por defecto: 1
	        -P | --play-dump : reproduce un dump realizado con Fceux Headless utilizando la opcion -d

Fceux headless: -e | --ae-settings : especifica la ruta del archivo de configuracion de IA.so, por defecto: ae.txt
                -l | --frame-limit : numero maximo de frames a emular antes de cerrar el emulador, por defecto: -1
		        -a | --analyze : especifica la ruta de archivo a un video .fm2 a analizar 
         		-d | --dump : especifica la ruta de archivo a un video .fm2 cuya ram dumpear

El formato del archivo de configuracion es bastante laxo, solamente requiere tener los parametros separados por un espacio o por un enter.

Nota: fceux guarda los ultimos parametros que le pasaste y los carga entre ejecucciones, por lo que una vez cambiados los parametros, estos actuan como los nuevos parametros por defecto.
