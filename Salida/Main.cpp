// Main.cpp

#include "Main.h"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <windows.h>

using namespace std;
using namespace cv;

#pragma comment(lib, "mysqlcppconn.lib")

//credenciales de la BD
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "";
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {


	sql::Driver     *driver; // Create a pointer to a MySQL driver object
	sql::Connection *dbConn; // Create a pointer to a database connection object
	sql::Statement  *stmt;   // Create a pointer to a Statement object to hold our SQL commands
							 
	sql::PreparedStatement *pstmt;

	VideoCapture cap(0);


	cap.set(CV_CAP_PROP_FRAME_WIDTH, 800);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 460);

    bool blnKNNTrainingSuccessful = loadKNNDataAndTrainKNN();           // attempt KNN training

    if (blnKNNTrainingSuccessful == false) {                            // if KNN training was not successful
                                                                        // show error message
        std::cout << std::endl << std::endl << "error: error: KNN traning was not successful" << std::endl << std::endl;
        return(0);                                                      // and exit program
    }

    cv::Mat imgOriginalScene;           // input image

    //imgOriginalScene = cv::imread("19.png");         // open image
	cout << "(Salida) Detector de placas inciado.." << endl;
	
	while (!GetAsyncKeyState(VK_ESCAPE)) {
		cout << "presion enter para capturar la placa" << endl;
		system("pause");
		if (GetAsyncKeyState(VK_RETURN)) {
			cap >> imgOriginalScene;
		}


		if (!imgOriginalScene.empty()) {    
			std::vector<PossiblePlate> vectorOfPossiblePlates = detectPlatesInScene(imgOriginalScene);          // detect plates

			vectorOfPossiblePlates = detectCharsInPlates(vectorOfPossiblePlates);                               // detect chars in plates// if unable to open image
			
			if (vectorOfPossiblePlates.empty()) {                                               // if no plates were found
				std::cout << std::endl << "No se detecto placa" << std::endl;
				cout << endl;
				// inform user no plates were found
				cout << endl;
				cout << endl;
			}
			else {                                                                            // else
																							  // if we get in here vector of possible plates has at leat one plate

																							  // sort the vector of possible plates in DESCENDING order (most number of chars to least number of chars)
				std::sort(vectorOfPossiblePlates.begin(), vectorOfPossiblePlates.end(), PossiblePlate::sortDescendingByNumberOfChars);

				// suppose the plate with the most recognized chars (the first plate in sorted by string length descending order) is the actual plate
				PossiblePlate licPlate = vectorOfPossiblePlates.front();


				if (licPlate.strChars.length() == 0) {                                                      // if no chars were found in the plate
					std::cout << std::endl << "No se detecto placa" << std::endl << std::endl;      // show message
					cout << endl;
					// inform user no plates were found
					cout << endl;
					cout << endl; // and exit program
				}
				else {
					//cout << "placa " << licPlate.strChars << endl;

					try
					{
						driver = get_driver_instance();
					}
					catch (sql::SQLException e)
					{
						cout << "No se pudo obtener driver de la BD. Mensaje de Error: " << e.what() << endl;
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
						cout << "No se pudo conectar con la BD. Mensaje de error: " << e.what() << endl;
						system("pause");
						exit(1);
					}

					stmt = dbConn->createStatement(); // Specify which connection our SQL statement should be executed on

													  // Try to query the database
					try
					{

						stmt->execute("USE basedesarrollo");//seleccionar la DB

					}
					catch (sql::SQLException e)
					{
						cout << "No se pudo acceder a la BD. Mensaje de Error: " << e.what() << endl;
						system("pause");
						exit(1);
					}
					cout << licPlate.strChars << endl;
					try
					{
						pstmt = dbConn->prepareStatement("CALL insertSalida(?)"); //ocupar el primer puesto vacio

						pstmt->setString(1, licPlate.strChars);
						
						pstmt->execute();
						cout << "placa capturada con exito" << endl;
						system("pause");
						//imgOriginalScene = NULL;
					}
					catch (sql::SQLException e)
					{
						cout << "No se pudo ejecutar procedimiento. Mensaje de error: " << e.what() << endl;
						cout << "El auto no esta registrado " << endl;
						system("pause");
						
					}
				}

			}
		}


	}
    return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawRedRectangleAroundPlate(cv::Mat &imgOriginalScene, PossiblePlate &licPlate) {
    cv::Point2f p2fRectPoints[4];

    licPlate.rrLocationOfPlateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect

    for (int i = 0; i < 4; i++) {                                       // draw 4 red lines
        cv::line(imgOriginalScene, p2fRectPoints[i], p2fRectPoints[(i + 1) % 4], SCALAR_RED, 2);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void writeLicensePlateCharsOnImage(cv::Mat &imgOriginalScene, PossiblePlate &licPlate) {
    cv::Point ptCenterOfTextArea;                   // this will be the center of the area the text will be written to
    cv::Point ptLowerLeftTextOrigin;                // this will be the bottom left of the area that the text will be written to

    int intFontFace = CV_FONT_HERSHEY_SIMPLEX;                              // choose a plain jane font
    double dblFontScale = (double)licPlate.imgPlate.rows / 30.0;            // base font scale on height of plate area
    int intFontThickness = (int)std::round(dblFontScale * 1.5);             // base font thickness on font scale
    int intBaseline = 0;
	
    cv::Size textSize = cv::getTextSize(licPlate.strChars, intFontFace, dblFontScale, intFontThickness, &intBaseline);      // call getTextSize

    ptCenterOfTextArea.x = (int)licPlate.rrLocationOfPlateInScene.center.x;         // the horizontal location of the text area is the same as the plate
	
    if (licPlate.rrLocationOfPlateInScene.center.y < (imgOriginalScene.rows * 0.75)) {      // if the license plate is in the upper 3/4 of the image
                                                                                            // write the chars in below the plate
        ptCenterOfTextArea.y = (int)std::round(licPlate.rrLocationOfPlateInScene.center.y) + (int)std::round((double)licPlate.imgPlate.rows * 1.6);
    } else {                                                                                // else if the license plate is in the lower 1/4 of the image
                                                                                            // write the chars in above the plate
        ptCenterOfTextArea.y = (int)std::round(licPlate.rrLocationOfPlateInScene.center.y) - (int)std::round((double)licPlate.imgPlate.rows * 1.6);
    }

    ptLowerLeftTextOrigin.x = (int)(ptCenterOfTextArea.x - (textSize.width / 2));           // calculate the lower left origin of the text area
    ptLowerLeftTextOrigin.y = (int)(ptCenterOfTextArea.y + (textSize.height / 2));          // based on the text area center, width, and height

             // write the text on the image
    cv::putText(imgOriginalScene, licPlate.strChars, ptLowerLeftTextOrigin, intFontFace, dblFontScale, SCALAR_YELLOW, intFontThickness);
}



