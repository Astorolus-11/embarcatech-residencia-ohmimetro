//Bibliotecas:
#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "pico/bootrom.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

//Variáveis globais:
#define i2c_port i2c1 
const uint SDA = 14, SCL = 15, endereco = 0x3C;
const uint adc_omh = 28, botao_a = 5, pin_matriz = 7;
int r_conhecido = 10000;
float r_x = 0.0, resolucao = 4095;
ssd1306_t ssd;
float tensao;
//Armazena os valores numa string:
char str_x[5];
char str_y[5];

//Protótipos das funções:
void gpio_irq_handler(uint gpio, uint32_t events);
void setup();
void i2c_setup();


int main()
{
    stdio_init_all();
    setup();
    i2c_setup();


    //Interrupção para o modo bootsel:
    gpio_set_irq_enabled_with_callback(botao_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
   
    ssd1306_fill(&ssd, false); //Limpa o display
    ssd1306_send_data(&ssd);

    //Inicializando o ADC:
    adc_init();
    adc_gpio_init(adc_omh);

    while (true) {
        
        adc_select_input(2); //Pino 28 canal 2

        float soma = 0.0f; // 0.0f para ser explicito para o compilador, se utilizar 0.0 sera atribuido o double e depois convertido para float.
        float media;

        for(int i=0; i<500; i++){ //Esse loop soma todos os valores obtidos e calcula a média
            soma+= adc_read();
            sleep_ms(1);
        }
        media = soma / 500.0f;
        //Formula para calcular o valor de r_x (resistor desconhecido):
        r_x = (r_conhecido*media) / (resolucao - media);

        sprintf(str_x, "%1.0f", media); //Converte o valor inteiro e armazena numa string
        sprintf(str_y, "%1.0f", r_x);
        
        //Estrutura do display:
        ssd1306_rect(&ssd,0,0,124,62,true,false);
        ssd1306_send_data(&ssd);
        ssd1306_draw_string(&ssd,"Ohmimetro",27,2);
        ssd1306_hline(&ssd,1,124,10,true);
        ssd1306_draw_string(&ssd,"Real:",2,12);
        ssd1306_draw_string(&ssd,str_y,43,12);
        ssd1306_draw_string(&ssd,"Ohms",77,12);
        ssd1306_draw_string(&ssd,"Comerc:",2,22);
        //DEFINIR O CALCULO PARA O COMERCIAL
    }
}
//Campo das funções:
void setup(){
    gpio_init(botao_a);
    gpio_set_dir(botao_a,GPIO_IN);
    gpio_pull_up(botao_a);
}
void i2c_setup(){
    //Configura a comunicação I²C
    i2c_init(i2c_port, 400*1000); //Inicializa o I²C com 400KHz  
    gpio_set_function(SCL,GPIO_FUNC_I2C); //Habilita a função para i²c
    gpio_set_function(SDA,GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);

    //Configura o display:
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, i2c_port);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);





}
void gpio_irq_handler(uint gpio, uint32_t events){ 
    reset_usb_boot(0, 0);
}