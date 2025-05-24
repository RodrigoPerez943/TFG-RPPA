#  Programa de Procesamiento de Datos

Esta carpeta contiene el código de MATLAB para el procesamiento completo de las señales I/Q generadas por el radar ROSAR FM-CW, incluyendo decodificación, compresión en rango y azimut, y detección de blancos. También se incluyen datos de ejemplo de experimentos reales y un archivo de ruido medio para su corrección.


## Contenido de Archivos

| Archivo                         | Descripción                                                                 |
|---------------------------------|-----------------------------------------------------------------------------|
| `RADAR_ROSAR_V3_Vectorizada.mlx` | Script principal en MATLAB Live Script. Procesa las señales I/Q, brújula y sincronismo. |
| `mean_noise.mat`               | Archivo con el perfil promedio de ruido del sistema para su corrección.     |
| `1Blanco.txt`                  | Datos de experimento con un solo blanco cercano.                            |
| `1BlancoLejos.txt`             | Datos de experimento con un solo blanco alejado.                            |
| `2Blancos.txt`                 | Datos de experimento con dos blancos.                                       |
| `3BlancosB.txt`                | Datos de experimento con tres blancos.