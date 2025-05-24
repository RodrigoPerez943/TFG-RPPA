#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <stdint.h>

#define UART_DEVICE "/dev/ttyPS1"
#define BAUD_RATE B4800
#define RX_BUFFER_SIZE 256

// Dirección base del GPIO en Zynq
#define GPIO_BASE_ADDR        0xE000A000
#define GPIO_DATA_2_OFFSET    0x48    // DATA_2
#define GPIO_DIRM_2_OFFSET    0x284   // DIRM_2 (Direction)
#define GPIO_OEN_2_OFFSET     0x304   // OEN_2  (Output Enable)

// Tamaño del mapeo de memoria
#define PAGE_SIZE 4096

// Máscara de 16 bits para usar únicamente pines [15..0]
#define MASK_16BITS 0x0000FFFF

// Leer y convertir el valor desde UART
int read_uart_value(int uart_fd, unsigned char *rx_buffer, size_t buf_size, int *index_ptr) {
    char ch;
    int ret = read(uart_fd, &ch, 1);
    if (ret > 0) {
        if (*index_ptr < buf_size - 1) {
            rx_buffer[(*index_ptr)++] = ch;
            if (*index_ptr >= 2 && 
                rx_buffer[*index_ptr - 2] == '\r' && 
                rx_buffer[*index_ptr - 1] == '\n') {

                rx_buffer[*index_ptr - 2] = '\0';
                char *endptr;
                float angulo = strtof((char *)rx_buffer, &endptr);
                if (endptr == (char *)rx_buffer) {
                    fprintf(stderr, "Error: Conversión de cadena a float fallida.\n");
                    return -2;
                }
                int angulo_int = (int)(angulo * 100);
                printf("Mensaje recibido: %s, Ángulo: %.2f, Valor entero: %d\n",
                       rx_buffer, angulo, angulo_int);

                *index_ptr = 0;
                memset(rx_buffer, 0, buf_size);
                return angulo_int;
            }
        } else {
            fprintf(stderr, "Advertencia: Buffer lleno, reiniciando y esperando valores válidos.\n");
            *index_ptr = 0;
            memset(rx_buffer, 0, buf_size);
        }
    } else if (ret < 0) {
        perror("Error al leer datos");
        return -2;
    }
    return -1;
}

// Configurar UART
int configure_uart(int uart_fd) {
    struct termios options;
    if (tcgetattr(uart_fd, &options) < 0) {
        perror("Error al obtener atributos UART");
        return -1;
    }

    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);
    options.c_cflag = BAUD_RATE | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_cflag &= ~CRTSCTS;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;

    tcflush(uart_fd, TCIFLUSH);
    if (tcsetattr(uart_fd, TCSANOW, &options) < 0) {
        perror("Error al configurar UART");
        return -1;
    }
    return 0;
}

int main(void) {
    int uart_fd;
    int index = 0;
    unsigned char rx_buffer[RX_BUFFER_SIZE];

    // Abrir UART
    uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Error al abrir UART");
        return 1;
    }

    if (configure_uart(uart_fd) < 0) {
        close(uart_fd);
        return 1;
    }

    printf("UART %s configurada. Escuchando...\n", UART_DEVICE);

    // Mapeo de memoria para GPIO
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Error al abrir /dev/mem");
        close(uart_fd);
        return 1;
    }

    void *gpio_base = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                           mem_fd, GPIO_BASE_ADDR);
    if (gpio_base == MAP_FAILED) {
        perror("Error en mmap");
        close(mem_fd);
        close(uart_fd);
        return 1;
    }

    // Punteros a los registros de bank2
    volatile uint32_t *gpio_data_2 = (volatile uint32_t *)((char *)gpio_base + GPIO_DATA_2_OFFSET);
    volatile uint32_t *gpio_dirm_2 = (volatile uint32_t *)((char *)gpio_base + GPIO_DIRM_2_OFFSET);
    volatile uint32_t *gpio_oen_2  = (volatile uint32_t *)((char *)gpio_base + GPIO_OEN_2_OFFSET);

    // 1) Configurar como salida los bits 0..15
    //    (read-modify-write para no pisar configuración de otros bits)
    {
        uint32_t reg;

        // DIRM_2
        reg = *gpio_dirm_2;
        reg |= MASK_16BITS; // activa dirección de salida en [15..0]
        *gpio_dirm_2 = reg;

        // OEN_2
        reg = *gpio_oen_2;
        reg |= MASK_16BITS; // habilita salida en [15..0]
        *gpio_oen_2 = reg;
    }

    // Bucle de lectura de UART y escritura a los bits [15..0] de DATA_2
    fd_set read_fds;
    struct timeval timeout;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(uart_fd, &read_fds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;  // 5 ms

        if (select(uart_fd + 1, &read_fds, NULL, NULL, &timeout) > 0) {
            int valor = read_uart_value(uart_fd, rx_buffer, RX_BUFFER_SIZE, &index);
            if (valor >= 0) {  // Mensaje completo recibido
                // Antes de escribir, leemos el valor actual y modificamos sólo [15..0].
                // Así no tocamos los bits [31..16] (en caso de que estén en uso).
                uint32_t reg = *gpio_data_2;
                reg &= ~MASK_16BITS;         // Pone a 0 bits [15..0]
                reg |= (valor & MASK_16BITS); // Inserta 'valor' en bits [15..0]
                *gpio_data_2 = reg;

                printf("Escribiendo en GPIO_DATA_2 (bits 0..15): 0x%04X\n", (valor & 0xFFFF));
            }
        }
    }

    munmap((void *)gpio_base, PAGE_SIZE);
    close(mem_fd);
    close(uart_fd);
    return 0;
}
