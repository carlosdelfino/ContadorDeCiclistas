Contador de Ciclistas
=====================

Projeto experimental de contagem de ciclistas utilizando webcam de baixo custo.
Adicionado a este Fork o uso de placa de prototipação NanoPI M3 com processador
Cortex-A53 dotado de unidade de ponto flutuante do tipo NEON.

Algumas alterações foram realizadas na compilação para que seja gerado um 
binário 100% para o Cortex-A53.

Requisitos
-----------

- GCC 4.9 (GNU-Linux-EABI)
- OpenCv 2.4
- V4l2Loopback
- WebCam (qualquer de R$ 20,00)

Como Usar
---------

- Compilar a aplicação digitando na linha de comando:
make

- Iniciar a aplicação utilizando a linha de comando abaixo:

`bin/CycloTracker usage:`

	--help   (-h): print this message. 
	--override (-O): override point picked parameters.
	--reg_source (-s) <file_name>: Specify regular file where data comes from.
	--dev_source (-D) <number>: Specify device number where data comes from.
	--record (-r) <file_name>: Record video to filename.
	--stream (-S) <device>: Streams video via device.
	--sensor (-t) <device>: Specify device file which is a sensor.
	--address (-a) <address>: address must be present and must be in in this format: street-number. --address faria_lima-1200.

Exemplo:

`bin/CycloTracker -D 0 --address Faria_Lima-2152 --record out.avi -S /dev/video1`

- Necessário selecionar (clicando na tela com o endereço fornecido):

1) O ponto em que o contador amarelo de ciclistas da direita para a esquerda deve se localizar

2) O ponto em que o contador amarelo de ciclistas da esquerda para a direita deve se localizar

3) O ponto superior esquerdo da área em perspectiva 

4) O ponto superior direito da área em perspectiva

5) O ponto inferior esquerdo da área em perspectiva

6) O ponto inferior direito da área em perspectiva

7) O ponto superior esquerdo da área de corte (vermelha)

8) O ponto inferior direito da área de corte (vermelha)

9) O ponto superior esquerdo da área de interesse (preta)

10) o ponto inferior direito da área de interesse (preta)

PGO Build
---------

`make pgo-firstpass`
(Use the software for a few hours like you're used to)
`make clean
make pgo-secondpass`
(The new binary is now optimised for that use)

Remove PGO files
----------------

`make distclean`

Analyse the generated PGO files
-------------------------------

Tools like gprof and gcov should help you :
- analyse the generated profiles and
- determine which parts of the code were used the most in your code.

You need to run the binary, like you're used to, on the build machine first.

Then :
`gprof bin/CycloTracker gmon.out > /tmp/performance_report.txt`
will generate a performance report in /tmp, which you can then open with a text editor.
The report will show how much CPU time was consumed by each invoked function.

`gcov CycloTracker.cpp`
will generate approximately one gcov file per source-file referenced in CycloTracker.cpp, including itself.
Each gcov file will be an annotated source-file which will provide you informations about which parts of
the code were executed, which are parts were not, and how many times the executed parts were executed.
These informations are cumulative. For example :
- If you have a function that is executed 10 times every time you run the binary.
- Running the same binary 5 times will show "50:function_name" in the gcov file.

