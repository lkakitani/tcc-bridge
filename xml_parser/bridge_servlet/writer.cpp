#include <iostream>
#include <fstream>
#include <string>

int main() {
	std::ofstream xmlFile;

	std::string bid = "b0003";
	std::string sid = "002";
	std::string recipient = "1";
	std::string request = "sensor"; // sensor | function | program
	std::string lightSensor = "1231";
	std::string presenceSensor = "1";

	xmlFile.open("bridge_response.xml");

	// XML Header
	xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
	
	xmlFile << "<bridgemsg bid=\"" << bid << "\">\n";
	xmlFile << "\t<sid>" << sid << "</sid>\n";
	xmlFile << "\t<recipient>" << recipient << "</recipient>\n";
	xmlFile << "\t<request_type>" << request << "</request_type>\n";

	xmlFile << "\t<response>\n";
	if (request == "sensor") {

		// light sensor
		xmlFile << "\t\t<sensor type=\"LUM\">\n";
		xmlFile << "\t\t\t<value>" << lightSensor << "</value>\n";
		xmlFile << "\t\t</sensor>\n";
		// presence sensor
		xmlFile << "\t\t<sensor type=\"PRES\">\n";
		xmlFile << "\t\t\t<value>" << presenceSensor << "</value>\n";
		xmlFile << "\t\t</sensor>\n";

	} else if (request == "function" || request == "program") {
		xmlFile << "\t\t<return>ok</return>\n";
	}

	xmlFile << "\t</response>\n";
	xmlFile << "</bridgemsg>";
	xmlFile.close();

	std::cout << "Write successful." << std::endl;
	return 0;
}
