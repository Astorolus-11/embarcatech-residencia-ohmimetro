//Bibliotecas:
#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include <math.h>
#include "ssd1306.h"
#include "font.h"
#include "pico/bootrom.h"
#include "hardware/clocks.h"
#include "string.h"

//Variáveis globais:
#define i2c_port i2c1 
const uint SDA = 14, SCL = 15, endereco = 0x3C;
const uint adc_omh = 28, botao_a = 5;
int r_conhecido = 10000;
float r_x = 0.0, resolucao = 4095, r_comercial;
ssd1306_t ssd;
float tensao;
//Armazena os valores numa string:
char str_y[7];
char str_comercial[7];
const float e24[]={
    10,11,12,13,15,16,18,20,22,24,27,30,33,36,39,43,47,51,56,62,68,75,82,91
};
char faixa_1[4],faixa_2[4],multiplicador[4],tolerancia[4];
const int e24_num = sizeof(e24) / sizeof(e24[0]); //24 elementos, se eu quiser trocar de série fica mais fácil de trocar.

//Protótipos das funções:
void gpio_irq_handler(uint gpio, uint32_t events);
void setup();
void i2c_setup();
float valor_comerc(float r_x);
void formatar_comerc(float valor, char *str_comercial);
void cores(char *faixa_1, char *faixa_2 , char *multiplicador, char * tolerancia);



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
        
        r_x = (r_conhecido*media) / (resolucao - media); //Valor real
        
        if(r_x>(1000*100)+1000){ //Máximo de 100k 
            r_x = 0.0f;
            
        }
         if(r_x<510){ //minimo de 510
            r_x=0.0f;
            
        }

        r_comercial = valor_comerc(r_x); //Valor comercial

        cores(faixa_1,faixa_2,multiplicador,tolerancia);

        formatar_comerc(r_comercial,str_comercial);

        

        sprintf(str_y, "%1.0f", r_x); //Transforma o resultado numa string
        
        
        //Estrutura do display: (Título, valor real e valor comercial)
        ssd1306_fill(&ssd,false);
        ssd1306_rect(&ssd,0,0,125,62,true,false);
        ssd1306_draw_string(&ssd,"Ohmimetro",27,2);
        ssd1306_hline(&ssd,1,124,10,true);
        ssd1306_draw_string(&ssd,"Real:",2,12);
        ssd1306_draw_string(&ssd,str_y,43,12);
        ssd1306_draw_string(&ssd,"Comerc:",2,22);
        ssd1306_draw_string(&ssd,str_comercial,58,22);
        
        //Cores do resistor:
        ssd1306_draw_string(&ssd,"Cores",42,32);
        if(r_comercial!=0){
        ssd1306_draw_string(&ssd,faixa_1,9,45);
        ssd1306_draw_string(&ssd,faixa_2,36,45);
        ssd1306_draw_string(&ssd,multiplicador,63,45);
        ssd1306_draw_string(&ssd,tolerancia,91,45);

        }
        else{
            ssd1306_rect(&ssd,45,9,100,15,false,true);
        }
        
        printf("%.0f\n",r_comercial);

        ssd1306_send_data(&ssd);
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

float valor_comerc(float r_x){
    float menor_dif = 1e9; // 1e9 = 1*10^9
    float mais_proximo = 0;
    if(r_x!=0){
        for(int i = 2; i<=5; i++){ // se i = 2, entao 10^2 = 100 e vai até 10^5 = 100k
            //calcula o fator, se é 100, 1000, 10000, 100000
            float fator = powf(10,i);
    
            for(int j = 0; j<e24_num;j++){ //Faz a busca do valor comercial
                
                float valor_candidato = e24[j] * fator; // multiplica cada valor da serie E24 pelo fator
                if(valor_candidato < 510.0f || valor_candidato > 100000.0f) continue; //Se não estiver nessa faixa, pula para próxima verificação
                
                float dif = fabsf(r_x - valor_candidato); //Retorna o valor absoluto da diferença entre o valor real e o valor candidato
    
                if(dif < menor_dif){
                    menor_dif = dif;
                    mais_proximo = valor_candidato; //Vai se aproximando cada vez mais do valor comercial
                }
            }
        }
    }
    else{
        mais_proximo=0;
    }
    
    return mais_proximo;

}

void formatar_comerc(float valor, char *str_comercial){
    if(valor>=1000.0f){
        sprintf(str_comercial,"%.1fk",valor/1000.0f); // Transformar 1000 em 1k

    }
    else{
        sprintf(str_comercial,"%.0f", valor);
    }
}

void cores(char *faixa_1, char *faixa_2 , char *multiplicador, char * tolerancia){
    int valor = (int)(r_comercial + 0.5f); // Arredonda o valor comercial
    int digito_1, digito_2, mult = 0;

    // Reduz até ficar com dois dígitos significativos:
    while (valor >= 100) {
        valor /= 10;
        mult++;
    }

    digito_1 = valor / 10;
    digito_2 = valor % 10;

    const char *cores[] = {
        "Prt", "Mrm", "Vrm", "Lrj", "Amr", "Vrd", "Azl", "Vio", "Cin", "Bra"
    };

    strcpy(faixa_1, cores[digito_1]);
    strcpy(faixa_2, cores[digito_2]);

    const char *multiplicadores[] = {
        "Prt", "Mrm", "Vrm", "Lrj", "Amr", "Vrd", "Azl"
    };

    if (mult >= 0 && mult <= 6) {
        strcpy(multiplicador, multiplicadores[mult]);
    } 

    strcpy(tolerancia, "Drd"); // 5% , Resistores da série E24
}


    


void gpio_irq_handler(uint gpio, uint32_t events){ 
    reset_usb_boot(0, 0);
}