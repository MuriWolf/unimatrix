/*
	UNImariX 0.2 - Kernel mínimo com "protodriver" de vídeo
	
  INTEGRANTES:
  Nome                                      RA
  Murillo Pinheiro de Oliveira              1999341
*/

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

// valores para cores do texto e fundo
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* 
	parâmetros:
		fg (foreground): cor para o texto
		bg (background): cor para o fundo
	retorno:
		inteiro de 1 byte sem sinal contendo os códigos para cor do texto e do fundo
*/
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

/* 
	parâmetros:
		uc: caractere a ser impresso
		color: inteiro de 1 byte sem sinal contendo o código da cor do texto e do fundo
		       (criado com a função vga_entry_color)
	retorno:
		inteiro de 1 byte sem sinal contendo os códigos para cor do texto e do fundo
*/

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

/*
	parâmetro:
		str: ponteiro para uma string
	retorno:
		inteiro com o tamanho da string
*/
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}


#define VGA_WIDTH   80			// constante com a quantidade de colunas em uma linha
#define VGA_HEIGHT  25			// constante com a quantidade de linhas na tela
#define VGA_MEMORY  0xB8000 	// constante com o endereço da memória do buffer de video

size_t terminal_row;			// armazena a "linha de impressão atual"
size_t terminal_column;			// armazena a "coluna de impressão atual"
uint8_t terminal_color;			// armazena a "atual" 

// ponteiro para a posição da memória do buffer de vídeo
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_newline();

// "inicializa" o terminal (basicamente, limpa a tela)
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

/* define a cor do texto/fundo padrão ("atual")
 	
 	parâmetro:
 		color: inteiro de 1 byte sem sinal contendo o código da cor do texto e do fundo
		       (criado com a função vga_entry_color)
*/
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

// imprime um caractere (c) na coordenada de tela passada (x e y)
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

// imprime um caractere (c) na linha e coluna atual 
// variáveis globais terminal_row e terminal_column, respectivamente)
void terminal_putchar(char c) 
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}


// imprime a string apontada pelo parâmetro "data" na linha e coluna atual 
// (variáveis globais terminal_row e terminal_column, respectivamente).
// é necessário passar o tamanho da string no parâmetro size
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++) {
    if ((int) data[i] == 10) { // Código ASCII para '\n'
      terminal_newline();
    } else {
		  terminal_putchar(data[i]);
    }
  }
}

// imprime a string apontada pelo parâmetro "data" na linha e coluna atual 
// (variáveis globais terminal_row e terminal_column, respectivamente).
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

/* ===============================================================
 * COMEÇO FUNÇÕES DO TRABALHO 
 * ===============================================================
 */

// Pula uma linha e vai para a primeira coluna
void terminal_newline() {
  terminal_row++;
  terminal_column = 0;
}

// Imprime string nas coordenadas e cores informadas 
void terminal_writestring_location(size_t x, size_t y, const char* data, uint8_t color) {
  if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
    return;
  }

	for (size_t i = 0; i < strlen(data); i++) {
    terminal_putentryat(data[i], color, x+i, y);
  }
}

// Imprime string centralizada no eixo X. Y e cores variam
void terminal_writestring_centered(size_t y, const char* text, uint8_t color) {
  int total_spacing = VGA_WIDTH - strlen(text);
  int start_spacing = total_spacing / 2;

  terminal_writestring_location(start_spacing, y, text, color);
}

// Imprime uma borda na tela usando o caractere passado no parâmetro character
void terminal_write_border(char character) {
  for (size_t x = 0; x < VGA_WIDTH; x++) {
    terminal_putentryat(character, terminal_color, x, 0);
    terminal_putentryat(character, terminal_color, x, VGA_HEIGHT-1);
  }

  for (size_t y = 1; y < VGA_HEIGHT-1; y++) {
    terminal_putentryat(character, terminal_color, 0, y);
    terminal_putentryat(character, terminal_color, VGA_WIDTH-1, y);
  }
    
}

// Imprime um retângulo preenchido pelo caractere character, com a largura especificada em width, a
// altura em height e com seu canto superior esquerdo posicionado nas coordenadas x e y passadas.
void terminal_write_rectangle(size_t x, size_t y, size_t width, size_t height, char character, uint8_t color) {
  if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
    return;
  }
    
  for (size_t w = 0; w < width; w++) {
    size_t rectangle_x_coord = w+x;
    if (rectangle_x_coord >= VGA_WIDTH) {
      return;
    }

    for (size_t h = 0; h < height; h++) {
      size_t rectangle_y_coord = h+y;
      if (rectangle_y_coord >= VGA_HEIGHT) {
        break;
      }

      terminal_putentryat(character, color, rectangle_x_coord, rectangle_y_coord);
    }
  }
}

// Imprime um tringulo retângulo (pode não ficar tão parecido devido a fonte do texto)
void terminal_write_right_triangle(size_t x, size_t y, size_t height, char character, uint8_t color) {
  if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
    return;
  }
    
  for (size_t h = 0; h < height; h++) {
    size_t rectangle_y_coord = h+y;
    if (rectangle_y_coord >= VGA_HEIGHT) {
      break;
    }

    for (size_t w = 0; w < h+1; w++) {
      size_t rectangle_x_coord = w+x;
      if (rectangle_x_coord >= VGA_WIDTH) {
        return;
      }

      terminal_putentryat(character, color, rectangle_x_coord, rectangle_y_coord);
    }
  }
}

/* ===============================================================
 * FIM FUNÇÕES DO TRABALHO 
 * ===============================================================
 */

void main() {
    
    //char* memoria_de_video = (char *)0xB8000;
    //*memoria_de_video = 'X';
    
   terminal_initialize();
   // mudando a cor
   terminal_setcolor(VGA_COLOR_RED);
   terminal_writestring("\n <color test>");
   terminal_setcolor(VGA_COLOR_LIGHT_GREY);

   // imprime um X no centro (mais ou menos...) da tela (x: 40 e y: 12)
   terminal_putentryat('X', vga_entry_color(VGA_COLOR_RED, VGA_COLOR_LIGHT_GREY), 40, 12);
  
  // 1. String com \n 
   terminal_writestring("\n <Line 2>\n <below line 2>");

  // 2. Print string em coordenadas especificas
  terminal_writestring_location(VGA_WIDTH-14, VGA_HEIGHT-3, "No cantinho", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));

  // 3. Print string no centro do eixo X
  terminal_writestring_centered(VGA_HEIGHT-5, "Na meiota", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_GREEN));

  // 4. Desenha borda
  terminal_write_border('$');

  // 5. Desenha quadrado
  terminal_write_rectangle(6, 8, 16, 4, '@', vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_RED));

  // 6. Desenha tringulo retângulo
  terminal_write_right_triangle(45, 10, 10, '*', vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLUE));
}
