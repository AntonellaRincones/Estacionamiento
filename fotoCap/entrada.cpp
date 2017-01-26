
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>

/*Librerias Opencv para manejo de imagenes y videostream*/
#include "opencv2\highgui\highgui.hpp"

#include "opencv2\imgcodecs\imgcodecs.hpp"

#include "opencv2\imgproc\imgproc.hpp"

/***********************************************/

/*librearias mysql para conexion y manejo de la BD*/
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
/***********************************************/

using namespace std;
using namespace cv;

#pragma comment(lib, "mysqlcppconn.lib")

//credenciales de la BD
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "";


/**Verifica si hay objetos circulares en la imagen
@param Mat img: la imagen capturada por la webcam
@return int: devuelve un entero 1 si hay circulos y un entero 0 si no los hay.

*/
int hough(Mat imgO,Mat img) {

	Mat gray;
	//img = imread("estacionamiento1.png", 1);
	try {

		cvtColor(imgO, gray, COLOR_RGB2GRAY);
		

		//GaussianBlur(img, gray, Size(9, 9), 2, 2);
		GaussianBlur(gray, gray, Size(9, 9), 2, 2);
		//medianBlur(img, gray, 5);
		vector<Vec3f> circles;
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, gray.rows / 4, 200, 100);

		/*for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(imgO, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center     
			circle(imgO, center, radius, Scalar(0, 0, 255), 3, 8, 0);// circle outline
			cout << "center : " << center << "\nradius : " << radius << endl;
		}*/

		// Show your results
		//namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
		//imshow("Hough Circle Transform Demo", imgO);

		if (circles.empty() == true) {
			return 0;
		}
		else {
			return 1;
		}
		
	}
	catch (cv::Exception e) {
		cout << "Could not get a database driver. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	
}

/**Obtener direccion al puesto
*/
void direccion(int id) {

	Mat dir1 = imread("direccion1.png", 1);
	Mat dir2 = imread("direccion2.png", 1);
	Mat dir3 = imread("direccion3.png", 1);
	Mat dir4 = imread("direccion4.png", 1);
	Mat dir5 = imread("direccion5.png", 1);
	Mat dir6 = imread("direccion6.png", 1);
	Mat dir7 = imread("direccion7.png", 1);
	Mat dir8 = imread("direccion8.png", 1);


	if (id == 1) {
		imshow("direccion", dir1);
		waitKey(0);	
	}

	if (id == 2) {
		imshow("direccion", dir2);
		waitKey(0);
	}
	if (id == 3) {
		imshow("direccion", dir3);
		waitKey(0);
	}

	if (id == 4) {
		imshow("direccion", dir4);
		waitKey(0);
	}
	if (id == 5) {
		imshow("direccion", dir5);
		waitKey(0);
	}

	if (id == 6) {
		imshow("direccion", dir6);
		waitKey(0);
	}
	if (id == 7) {
		imshow("direccion", dir7);
		waitKey(0);
	}

	if (id == 8) {
		imshow("direccion", dir8);
		waitKey(0);
	}

}


/** Verifica si hay algo azul en la imagen, Azul sera el color identificador
de cada puesto
@param Mat img: la imagen capturada por la webcam
@return Mat devuelve una matriz rrellena de 0 donde lo azul sera 1.
*/
Mat blue(Mat img) {

	Mat OutputImage;
	//cvtColor(img,OutputImage, COLOR_BGR2HSV);
	inRange(img, Scalar(100, 10, 10), Scalar(255, 100, 100), OutputImage); //convierte la imagen a binario solo si posee color azul (indicadores de puesto)
	//imshow("azul", OutputImage);
	//waitKey(0);
	
	return OutputImage;
}

/**
Metodo para comprar imagenes, se toma una muestra del puesto, en este caso
cada puesto tiene un numero, se busca ese numero dentro la foto capturada, si se encuentra
retorna 1, sino retorna 0
@param Int inicializar en 0,
@param void* 
@img Mat, imagen tomada por la camara
@templ Mat, numero que se buscara en la imagen tomada
@return int, retorna 1 si consigio el numero y 0, sino se consigio.
*/

int MatchingMethod(int, void*, Mat img, Mat templ)
{
	try {
		Mat img_display, result;
		img.copyTo(img_display);

		//Matriz de resultados
		int result_cols = img.cols - templ.cols + 1;
		int result_rows = img.rows - templ.rows + 1;

		result.create(result_rows, result_cols, CV_32FC1);

		matchTemplate(img, templ, result, CV_TM_CCOEFF_NORMED);
		//establece el % de coincidencia que se quiere, en este casi que sea casi perfecto: 90%
		threshold(result, result, 0.9, 1., CV_THRESH_TOZERO);
		//Localiza el Mejor match con minMaxLoc
		double minVal; double maxVal; double threshold = 0.9; Point minLoc; Point maxLoc;
		Point matchLoc;

		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
		matchLoc = maxLoc;
		//maxVal contiene el % de coicidencia que se obtuvo, si es igual o mayor al threshold es que se consigio un match muy similar al de la imagen template
		if (maxVal >= threshold) {
			//rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
			//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
			return 1;
		}
		//imshow("imagen", img_display);
		//imshow("resulr", result);
		return 0;
	}
	catch (cv::Exception e) {
		cout << "Error en MatchMatching" << e.what() << endl;
		system("pause");
		exit(1);
	}
}


/**
dada una imagen de entrada, tomada por la camara, y las acciones a realizar en la base de datos
en este caso llamar un procedimiento que marca un puesto como ocupado, y otra que marca un puesto como vacio,
para cada puesto se llamara a MatchingMethod para modificar la base de datos con que puestos estan vacios
y cuales no
@param Mat img: imagen de entrada tomada por la camara
@param *pstmt PreparedStatement: procedimiento a realizar en la base de datos, en este caso CALL ocuparPuesto()
@param *pstmt2 PreparedStatement: procedimiento a realizar en la base de datos, en este caso CALL desocuparPuesto()
*/

void identificaPuesto(Mat img, sql::PreparedStatement *pstmt, sql::PreparedStatement *pstmt2) {
		Mat gray, temp;
		int result;
		cvtColor(img, gray, COLOR_BGR2GRAY);
		temp = imread("1.jpg", 0);
		//cvtColor(temp1, temp1, COLOR_BGR2GRAY);
		//matchTemplate(gray, temp1, res, TM_CCORR_NORMED);
		//imshow("resultado", res);
		result = MatchingMethod(0, 0, gray, temp);		
		if (result == 1) {
			pstmt->setInt(1, 1);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}else {
			pstmt2->setInt(1, 1);
			pstmt2->execute();
		}
		temp = imread("2.jpg", 0);
		result = MatchingMethod(0, 0, gray, temp);
		if (result == 1) {
			pstmt->setInt(1, 2);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}
		else {
			pstmt2->setInt(1, 2);
			pstmt2->execute();
		}
		temp = imread("3.jpg", 0);
		result = MatchingMethod(0, 0, gray, temp);
		if (result == 1) {
			pstmt->setInt(1, 3);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}
		else {
			pstmt2->setInt(1, 3);
			pstmt2->execute();
		}temp = imread("4.jpg", 0);
		result = MatchingMethod(0, 0, gray, temp);
		if (result == 1) {
			pstmt->setInt(1, 4);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}
		else {
			pstmt2->setInt(1, 4);
			pstmt2->execute();
		}temp = imread("5.jpg", 0);
		result = MatchingMethod(0, 0, gray, temp);
		if (result == 1) {
			pstmt->setInt(1, 5);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}
		else {
			pstmt2->setInt(1, 5);
			pstmt2->execute();
		}temp = imread("6.jpg", 0);
		result = MatchingMethod(0, 0, gray, temp);
		if (result == 1) {
			pstmt->setInt(1, 6);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}
		else {
			pstmt2->setInt(1, 6);
			pstmt2->execute();
		}temp = imread("7.jpg", 0);
		
		result = MatchingMethod(0, 0, gray, temp);
		if (result == 1) {
			pstmt->setInt(1, 7);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}
		else {
			pstmt2->setInt(1, 7);
			pstmt2->execute();
		}

		temp = imread("8.jpg", 0);
		result = MatchingMethod(0, 0, gray, temp);
		if (result == 1) {
			pstmt->setInt(1, 8);//asignarle al procedure ocuparPuesto el parametro id
			pstmt->execute();
		}
		else {
			pstmt2->setInt(1, 8);
			pstmt2->execute();
		}
}


/**Para verificar si hay puesto o no en el estacionamiento
usa las funciones blue() y hough() para encontrar circulos azules en la foto capturada
y retorna 1 si los hay, es decir, si hay puesto, y 0, si no los hay.
@param Mat img: la imagen capturada por la webcam
@return int: devuelve un valor 0, no hay puesto, 1, si hay puesto
*/
int vacio(Mat img) {

	Mat gray;
	cvtColor(img, gray, COLOR_BGR2HSV);
	if ((hough(img,blue(gray))) == 1) 
	{
		return 1;
	}else
	{
		return 0;
	}
}

int main(void)
{ 
	Mat imagen;

	//imagen = imread("imagenlleno.jpg", 1);
	
	
	
	
	//imshow("Captured", imagen);
	//waitKey(0);
	sql::Driver     *driver; // Create a pointer to a MySQL driver object
	sql::Connection *dbConn; // Create a pointer to a database connection object
	sql::Statement  *stmt;   // Create a pointer to a Statement object to hold our SQL commands
	//sql::ResultSet  *res;    // Create a pointer to a ResultSet object to hold the results of any queries we run
	sql::PreparedStatement *pstmt, *pstmt2;

	
	VideoCapture cap(0);


	cap.set(CV_CAP_PROP_FRAME_WIDTH, 800);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 460);
	//cap >> imagen;
	//mientras la camara este funcional, tomar foto cada 10s
	int i = 1;

	if (cap.isOpened()) {
		while (1) {
			
			//cap.read(imagen);
			//cap >> imagen;
			//imshow("captura", imagen);
			//if (i == 1) { imagen = imread("imagenestado1.jpg", 1); }
			//if (i == 2) { imagen = imread("imagenestado2.jpg", 1); }
			//if (i == 3) { 
				//imagen = imread("imagenestado3.jpg", 1);
				//i = 1;
			//}
			imagen = imread("imagenestado1.jpg", 1);
			if (vacio(imagen) == 1) {
				try
				{
					driver = get_driver_instance();
				}
				catch (sql::SQLException e)
				{
					cout << "Could not get a database driver. Error message: " << e.what() << endl;
					system("pause");
					exit(1);
				}

				// Try to connect to the DBMS server
				try
				{
					dbConn = driver->connect(server, username, password);
				}
				catch (sql::SQLException e)
				{
					cout << "Could not connect to database. Error message: " << e.what() << endl;
					system("pause");
					exit(1);
				}

				stmt = dbConn->createStatement(); // Specify which connection our SQL statement should be executed on

												  // Try to query the database
				try
				{

					stmt->execute("USE basedesarrollo");//seleccionar la DB
													/*res = stmt->executeQuery("SELECT entrar('BBP13I')"); //realizar un query, guardar resultados en res
													while (res->next()) { //navegar por el puntero res
													id = res->getInt(1);//almacena el id obtenido en res
													}*/
				}
				catch (sql::SQLException e)
				{
					cout << "SQL error. Error message: " << e.what() << endl;
					system("pause");
					exit(1);
				}

				try
				{
					pstmt = dbConn->prepareStatement("CALL ocuparPuesto(?)"); //ocupar el primer puesto vacio
					pstmt2 = dbConn->prepareStatement("CALL desocuparPuesto(?)");
					identificaPuesto(imagen, pstmt, pstmt2);
					//pstmt->setInt(1, id);//asignarle al procedure ocuparPuesto el parametro id
					//pstmt->execute();
				}
				catch (sql::SQLException e)
				{
					cout << "SQL error. Error message: " << e.what() << endl;
					system("pause");
					exit(1);
				}

				// While there are still results (i.e. rows/records) in our result set...
				/*while (res->next())
				{
				// ...get each field we want and output it to the screen
				// Note: The first field/column in our result-set is field 1 (one) and -NOT- field 0 (zero)
				// Also, if we know the name of the field then we can also get it directly by name by using:
				// res->getString("TheNameOfTheField");
				cout << res->getString(1) << endl;
				}*/
			}
			//i++;
			waitKey(10000);
		}

	}

	if (vacio(imagen) == 1) {
		try
		{
			driver = get_driver_instance();
		}
		catch (sql::SQLException e)
		{
			cout << "Could not get a database driver. Error message: " << e.what() << endl;
			system("pause");
			exit(1);
		}

		// Try to connect to the DBMS server
		try
		{
			dbConn = driver->connect(server, username, password);
		}
		catch (sql::SQLException e)
		{
			cout << "Could not connect to database. Error message: " << e.what() << endl;
			system("pause");
			exit(1);
		}

		stmt = dbConn->createStatement(); // Specify which connection our SQL statement should be executed on

										  // Try to query the database
		try
		{

			stmt->execute("USE desarrollo");//seleccionar la DB
			/*res = stmt->executeQuery("SELECT entrar('BBP13I')"); //realizar un query, guardar resultados en res
			while (res->next()) { //navegar por el puntero res
				id = res->getInt(1);//almacena el id obtenido en res
			}*/
		}
		catch (sql::SQLException e)
		{
			cout << "SQL error. Error message: " << e.what() << endl;
			system("pause");
			exit(1);
		}

		try
		{
			pstmt = dbConn->prepareStatement("CALL ocuparPuesto(?)"); //ocupar el primer puesto vacio
			pstmt2 = dbConn->prepareStatement("CALL desocuparPuesto(?)");
			identificaPuesto(imagen,pstmt,pstmt2);
			//pstmt->setInt(1, id);//asignarle al procedure ocuparPuesto el parametro id
			//pstmt->execute();
		}
		catch (sql::SQLException e)
		{
			cout << "SQL error. Error message: " << e.what() << endl;
			system("pause");
			exit(1);
		}
		
		// While there are still results (i.e. rows/records) in our result set...
		/*while (res->next())
		{
			// ...get each field we want and output it to the screen
			// Note: The first field/column in our result-set is field 1 (one) and -NOT- field 0 (zero)
			// Also, if we know the name of the field then we can also get it directly by name by using:
			// res->getString("TheNameOfTheField");
			cout << res->getString(1) << endl;
		}*/
	}

	
	//direccion(id);

	return 0;

}