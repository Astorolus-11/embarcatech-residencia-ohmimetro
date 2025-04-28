# Ohmímetro na BitDogLab com o RP2040


## 1. Propósito


O objetivo desse projeto é calcular o valor de resistência de resistores variados pertinentes a série E24 e exibir no display SSD1306 que está acoplado na placa BitDogLab.


## 2. Hardware


Para o desenvolvimento desse projeto foi utilizado a placa BitDogLab fornecida pela Embarcatech, com o Raspberry Pi Pico W com o microcontrolador RP2040.
Os componentes utilizados presentes na placa foram:


1. Botão : A.
2. Comuinicação serial I2C : SDA e SCL (GPIOs 14 e 15).
3. Display SSD1306 : Endereço 0x3C.





## 3. Funcionalidades


1. Botão A - Ativa o modo Bootsel.
2. Display - Exibe o valor real e comercial do resistor e suas respectivas cores. (Série E24/ tolerância de +/- 5%)



   
   
## 4. Pré-requisitos


1. Ter o [Pico SDK](https://github.com/raspberrypi/pico-sdk) instalado na máquina;
2. Ter o [ARM GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) instalado;
3. Ter o [Visual Studio Code](https://code.visualstudio.com/download) instalado;
4. Ter este repositório clonado na sua máquina;
5. Ter as seguintes extensões instaladas no seu VS Code para o correto funcionamento:
- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools);
- [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake);
- [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools);
- [Raspberry Pi Pico](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico);
6. Ter uma protoboard, 3 jumpers e resistores variados da série E24.
  

##  5. Como executar o projeto:


1. Clonar o repositório: https://github.com/Astorolus-11/embarcatech-residencia-ohmimetro
2. Abrir a pasta clonada através do import project:

   ![image](https://github.com/user-attachments/assets/9ea528e1-0253-4cf8-b6c6-8532be0fc1b4)
   

3. Para executar na placa clique em Run que está localizada no rodapé do vscode (A placa precisa já está conectada e com o BootSel ativado):

   ![image](https://github.com/user-attachments/assets/36b14dce-1309-4f0c-a7f3-3cd7edb2b336)


## 6. Vídeo de demonstração

https://www.dropbox.com/scl/fi/m9cvpdek0jns5eu76hf5k/Ohm-metro-BitDogLab-Feito-com-o-Clipchamp.mp4?rlkey=bnqqoy6xv0iqw1ck2431hzohn&st=7i0u6ceb&dl=0


   
  
  # Pronto! já está tudo pronto para testar o projeto!
