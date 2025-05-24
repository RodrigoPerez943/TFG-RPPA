#  Código Control Gimbal

Esta carpeta contiene los archivos modificados del control del gimbal embarcado en el dron **DJI Matrice 300 RTK**, utilizando el **DJI Payload SDK (PSDK)**.


| Archivo                        | Descripción                                                                 |
|-------------------------------|-----------------------------------------------------------------------------|
| `main.c`                      | Programa principal que inicializa todos los sistemas. |
| `dji_sdk_config.h`            | Configuración de los módulos habilitados en el SDK.               |
| `dji_sdk_app_info.h`          | Información de autenticación y licencia del SDK DJI.                       |
| `hal_uart.h`                  | Configuración de las interfaces usadas en la comunicación UART entre el dron y el sistema embarcado. |
| `test_payload_xport.c/h`      | Funciones para controlar el gimbal mediante comandos de rotación. |
| `test_widget_interaction.c`   | Interfaz personalizada para botones/widgets en el mando del dron.         |
| `widget_config.json`          | Configuración de los botones interactivos visibles en el mando del dron. |