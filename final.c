 /*!
 \file final.c
 \brief Esse é o arquivo principal do sistema.
 \author Diego Conci Borsato e Alex de Souza
 \date 15/07/13
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

FILE *adc;
FILE *gpio;
FILE *html;

/*!
 \brief Essa função inicializa o pino utilizado como saida no GPIO.
 */

void inicializa(){

        //define PIN17 193
        gpio=fopen("/sys/class/gpio/export","w");
        fputs ("193",gpio);
        fclose (gpio);

        //seta a direção como saída
        gpio=fopen("/sys/class/gpio/gpio193/direction","w");
        fputs ("out",gpio);
        fclose (gpio);
}

/*!
 \brief Essa função é para incrementar um timer.
 \param int a, define quantas vezes deve ser aguardado o timer.
 */

void espera(int a){
	int i;
	for (i=0; i<a; i++){
		usleep(0);
	}
}

/*!
 \brief Essa função seta a saida em nivel alto ou baixo, dependendo po parametro.
 \param char *a,recebe o valor que deve ser escrito na saida, 0 para nivel baixo e 1 para nivel alto.
 */

void saida_lampada(char *a){

	gpio=fopen("/sys/class/gpio/gpio193/value","w");
	fputs (a,gpio);
	fclose (gpio);
}

/*!
 \brief Essa função realiza a leitura do adc da mini2440.
 \return Retorna o valor lida no ADC de 0 a 3300.
 */

int leitura_adc(){

	int cruzamento_zero;
	char valor[20];

	adc = fopen ("/sys/devices/platform/s3c24xx-adc/s3c-hwmon/in1_input", "r");
	fscanf (adc, "%s", valor);
	fclose (adc);
	cruzamento_zero = atoi(valor);
	return cruzamento_zero;
}

/*!
 \brief Essa função gera um pulso de saida em um determinada porta.
 \param int a, varia o momento do disparo de saida com relação a potêencia desejada.
 */

int disparo(int a){
	int cruzamento_zero, b, j, temp;


	temp = (1/120)*1000;

	if(a == 25){
		b = (temp*1)/4;
	}
	if(a == 50){
		b = temp/2;
	}	
	if(a == 75){
		b = (temp*3)/4;
	}
	if(a == 100){
		b = 0;	
	}
	//cruzamento_zero = leitura_adc();
	cruzamento_zero = 300;

	for(j=0; j<12000; j++){
		if(cruzamento_zero>200){
			espera(b);
			saida_lampada("1");
			espera(0);
			saida_lampada("0");
		}
	}
}

/*!
 \brief Essa função realiza a gravação de um arquivo html.
 \param int pot, representa a potencia que está sendo aplicada a saida.
 \param char *status, indica se a lampada está "LIGADA" ou "DESLIGADA".
 */

int carrega_html(int pot, char *status){

	char aux[6];

	html= fopen("/var/www/index.html","w");
	fputs("<html><head><title>Sistemas Embarcados</title><META HTTP-EQUIV='REFRESH' CONTENT='1'></head><body>\n",html);
	fputs("<table width=\"1020\" border=\"0\">\n",html);
	fputs("  <tr>\n",html);
	fputs("    <th width=\"788\" bgcolor=\"#FFFFFF\" scope=\"col\"><p align=\"center\"><u><strong>SISTEMAS EMBARCADOS</strong></u></p>\n",html);
	fputs("      <p align=\"center\"><strong>PROFESSOR RICARDO BALBINOT</strong></p>\n",html);
	fputs("      <p align=\"center\"><strong>TRABALHO FINAL</strong></p>\n",html);
	fputs("      <p align=\"center\"><strong>COMPONENTES: ALEX DE SOUZA E DIEGO CONCI BORSATO</strong></p>    </th>\n",html);
	fputs("  </tr>\n",html);
	fputs("</table>\n",html);
	fputs("<table width=\"1020\" height=\"483\" border=\"0\">\n",html);
	fputs("  <tr>\n",html);
	fputs("    <th width=\"1020\" height=\"477\" scope=\"col\"><table width=\"1020\" height=\"473\" border=\"0\">\n",html);
	fputs("      <tr>\n",html);
	fputs("        <th height=\"359\" align=\"center\" valign=\"middle\" scope=\"col\"><p>A LAMPADA ESTA \n",html);
	fputs(status, html);
	fputs("          </p>\n",html);
	fputs("          <p>Intensidade Luminosa:</p>\n",html);
	fputs("          <p>",html);
	sprintf (aux, "%d", pot);
	fputs(aux, html);
	fputs("%  DE  100% DE POTENCIA</p>\n",html);
	fputs("          <p>&nbsp;</p></th>\n",html);
	fputs("        </tr>\n",html);
	fputs("    </table></th>\n",html);
	fputs("  </tr>\n",html);
	fputs("</table>\n",html);
	fputs("</body>\n",html);
	fputs("</html>\n",html);
	fclose(html);
}

/*!
 \brief Função principal do sistema
 \param argc Indica número de argumentos repassados ao programa
 \param argv Ponteiro para o valor dos parametros repassados na forma de texto
 \return Código de falha ou sucesso
 */

int main (int argc, char **argv)
{
	int pot=0;
	char *status = "DESLIGADA";

	inicializa();

	while (1){
		saida_lampada("0");
		status = "DESLIGADA";
		pot = 0;
		printf("Entrou aqui");
		carrega_html(pot,status);
		espera(5000);
		status = "LIGADA";
		pot = 25;
		carrega_html(pot,status);
		disparo(25);
		pot = 50;
		carrega_html(pot,status);
		disparo(50);
		pot = 75;
		carrega_html(pot,status);
		disparo(75);
		pot = 100;
		carrega_html(pot,status);
		disparo(100);
	}
	return 1;
}

