//TESTE !!! inicio

#include<boost/format.hpp>

#include "CoordTransform.hpp"
#include "CycloConfig.hpp"
#include "CycloTracker.hpp"
#include "Camera.hpp"

#define PI (4 * atan(tan(1.)))

unsigned long debug = 1;

static CycloConfig *config = nullptr;

std::string confCam("camera.conf");
Camera cam(confCam);

void initCamera(cv::Mat& f);

void initCamera(cv::Mat& f) {
	cam.SetFrameRows(f.rows);
	cam.SetFrameCols(f.cols);
	cv::Point2f A, B;
	A.x = 362;
	A.y = 86; //ponto de cima da trena  (foto em que estou na parede)
	B.x = 364;
	B.y = 225; //ponto de baixo da trena (foto em que estou na parede)
	float distTrenaCamera = 6.;
	float tamTrena = 1.;
	//nao e' necessario mudar as coordenadas de A e B pois o metodo abaixo usa a
	//distancia entre A e B.
	cam.SetFocalDistance(distTrenaCamera, tamTrena, A, B);
	std::cout << "focal distance " << cam.GetFocalDistance() << "px"
			<< std::endl;
	A.x = 389;
	A.y = 194;
	B.x = 388;
	B.y = 228;
	float coquinhoAteX = 1.43;
	//nao e' necessario mudar as coordenadas de A e B pois o metodo abaixo usa a
	//distancia entre A e B.
	cam.SetHeight(coquinhoAteX, A, B);
	std::cout << "cam height " << cam.GetHeight() << "m" << std::endl;
	A.x = 219;
	A.y = 275; //top cabo vassoura (top, left)
	B.x = 217;
	B.y = 313; //top cabo vassoura (bottom, left)
	float l = 2.32; //l = Rz1 - Rz2 = largura ciclofaixa;
	float tamCaboVassoura = 1.17;
	cam.SetTheta(l, A, B, tamCaboVassoura);
	std::cout << "theta " << cam.GetTheta() * 180. / PI << "°" << std::endl;
	A.y = 275;
	cam.SetRz(A, tamCaboVassoura);
	std::cout << "Rz " << cam.GetRz() << std::endl;
	float asd, zxc;
	cv::Point2f PP1;
	cv::Point2f PP2;
	PP1.x = 248;
	PP1.y = 55; //semaforo top
	PP2.x = 251;
	PP2.y = 90; //semafoto bottom
	asd = CoordTransform::GetRealRyFromImageiPoint(cam, PP1);
	zxc = CoordTransform::GetRealRyFromImageiPoint(cam, PP2);
	std::cout << "h1 = " << asd << " . h2 = " << zxc << ".... |h1-h2| = "
			<< fabs(asd - zxc) << std::endl;
	PP1.y = 275; //vassoura top
	PP2.y = 288; //vassoura bottom
	std::cout << "tam T: "
			<< (asd = CoordTransform::GetRealRyFromImageiPoint(cam, PP1))
			<< std::endl;
	std::cout << "tam B: "
			<< (zxc = CoordTransform::GetRealRyFromImageiPoint(cam, PP2))
			<< std::endl;
	std::cout << "vassoura tem: " << asd - zxc << std::endl;
	PP1.y = 134; //poste grande top
	PP2.y = 263; //poste grande bottom
	std::cout << "tam T: "
			<< (asd = CoordTransform::GetRealRyFromImageiPoint(cam, PP1))
			<< std::endl;
	std::cout << "tam B: "
			<< (zxc = CoordTransform::GetRealRyFromImageiPoint(cam, PP2))
			<< std::endl;
	std::cout << "poste grande tem: " << asd - zxc << std::endl;
	std::string fn(".camera.conf");
	cam.SaveConf(fn);
}

void tt(void);
void tt(void) {
	const char* a("camera.conf");
	const char* b("cameraa.conf");
	Camera cam(a);
	Camera cam2(b);
	cv::Point2f PP1, PP2;
	float asd, zxc;
	std::cout << "focal distance " << cam.GetFocalDistance() << "px"
			<< std::endl;
	std::cout << "cam height " << cam.GetHeight() << "m" << std::endl;
	std::cout << "theta " << cam.GetTheta() * 180. / PI << "°" << std::endl;
	std::cout << "Rz " << cam.GetRz() << std::endl;
	PP1.y = 134; //poste grande top
	PP2.y = 263; //poste grande bottom
	std::cout << "tam T: "
			<< (asd = CoordTransform::GetRealRyFromImageiPoint(cam, PP1))
			<< std::endl;
	std::cout << "tam B: "
			<< (zxc = CoordTransform::GetRealRyFromImageiPoint(cam, PP2))
			<< std::endl;
	std::cout << "poste grande tem: " << asd - zxc << std::endl;
}

int main(int argc, char **argv) {
	std::cout << "*****************************************************"
			<< std::endl;
	std::cout << "** Obtendo configurações                           **"
			<< std::endl;

	config = CycloConfig::get();

	std::cout << "** configurações carregadas                        **"
			<< std::endl;
	std::cout << "** Verificando Opções da Linha de comando          **"
			<< std::endl;

	struct option long_options[9] = 								//..
			{			                  								//..
			{ "reg_source", required_argument, nullptr, 's' }, 			//..
					{ "dev_source", required_argument, nullptr, 'D' }, 	//..
					{ "record", required_argument, nullptr, 'r' },     	//..
					{ "stream", required_argument, nullptr, 'S' },     	//..
					{ "address", required_argument, nullptr, 'a' },    	//..
					{ "help", no_argument, nullptr, 'h' }, 				//..
					{ nullptr, 0, nullptr, 0 } 							//..
			};

	config->parseCommandOptions(argc, argv, long_options);
	CycloTracker tracker(config);
	cv::VideoCapture* cap = tracker.openVideoCapture();

	std::cout << "** Obtendo o primeiro frame, para dimensionamento de janela"
			<< std::endl;
	std::cout << "** Criando Janela Principal." << std::endl;

	// Obtemos o primeiro Frame para calculos de tamanho da janela
	cv::Mat frame;
	*cap >> frame;

	cv::imshow(config->getAddress(), frame);

	cv::Size frame_size = frame.size();
	std::cout
			<< boost::format(
					"** Dimensionando janela, largura: %d, comprimento: %d")
					% frame_size.height % frame_size.width << std::endl;
	// seta o tamanho do frame nas configurações com base no primeiro frame obtido.
	// veja documentação do setFrameSize() para detalhes.
	config->setFullFrameSize(frame_size);

	std::cout
			<< boost::format(
					"** Contador a Esquerda: (%d,%d), a Direita (%d,%d)")
					% config->getCounterX(0) % config->getCounterY(0)
					% config->getCounterX(1) % config->getCounterY(1)
			<< std::endl;

	std::cout
			<< boost::format(
					"** Janela de Interesse: (%d,%d), largura: %d, altura: %d")
					% config->getInterestX(0) % config->getInterestY(0)
					% config->getInterestWidth() % config->getInterestHeight()
			<< std::endl;

	std::cout
			<< boost::format(
					"** Perspectiva atual: Ponto a esquerda: (%d,%d), a Direita (%d,%d)")
					% config->getX(0) % config->getY(0) % config->getX(1)
					% config->getY(1) << std::endl
			<< boost::format(
					"                      Esquerda inferior: (%d,%d), a direita (%d,%d)")
					% config->getX(2) % config->getY(2) % config->getX(3)
					% config->getY(3) << std::endl;
	std::cout
			<< boost::format(
					"** Janela de Corte: (%d,%d), comprimento: %d, altura: %d")
					% config->getCropX(0) % config->getCropY(0)
					% config->getCropWidth() % config->getCropHeight()
			<< std::endl;

	tracker.openVideoOutput();
	tracker.openVideoWriter();

	std::cout << "** Inciando loop de processamento das imagens" << std::endl;
	tt();
	initCamera (frame);
	tracker.setPosCallBack(PosCallBack);

	tracker.processFrames();

	delete config;

	exit(0);
	return 0;
}

void PosCallBack(	//.
		cv::Mat frame,				//.
		cv::Mat full,				//.
		cv::Mat fore				//.
		) {
	initCamera(frame);
}
void PreCallBack(	//.
		cv::Mat frame,				//.
		cv::Mat full,				//.
		cv::Mat fore				//.
		) {

}

//TESTE !!! fim
