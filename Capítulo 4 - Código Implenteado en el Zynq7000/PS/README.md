#  Código Implementado en el Zynq-7000 — Sistema de Procesamiento (PS)

Esta carpeta contiene el software embebido ejecutado en el **Processing System (PS)** del Zynq-7000, responsable de la gestión de datos radar, comunicación UART, y control general del sistema bajo PetaLinux.

| Carpeta     | Descripción                                                                 |
|------------------------|-----------------------------------------------------------------------------|
| `ipc/`                 | Biblioteca y funciones para comunicación entre procesos |
| `ipc_daemon/`           | "Demonio" principal que gestiona la sincronización y flujo de datos.         |
| `lectorUART/`           | Programa que lee los datos de brújula vía UART.              |
| `radar_control_app/`    | Aplicación principal que recibe los datos I/Q y gestiona el ciclo de procesado. |
| `scripts/`             | Scripts Bash para lanzar el sistema.                |
| `servertcp/`            | Servidor TCP que permite controlar el sistema.        |
