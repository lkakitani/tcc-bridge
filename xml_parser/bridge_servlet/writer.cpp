#include <iostream>
#include <fstream>
#include <string>

int main() {
	std::ofstream xmlFile;

	std::string bid = "b0003";
	std::string sid = "002";
	std::string recipient = "1";

	xmlFile.open("bridge_response.xml");

	// XML Header
	xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
	
	xmlFile << "<bridgemsg bid=\"" << bid << "\">\n";
	xmlFile << "\t<sid>" << sid << "</sid>\n";
	xmlFile << "\t<recipient>" << recipient << "</recipient>\n";
	xmlFile << "\t<response>\n";

	xmlFile << "\t</response>\n";
	xmlFile << "</bridgemsg>";
	xmlFile.close();
	return 0;
}
