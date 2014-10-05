#include "pugixml.hpp"
#include <iostream>
#include <string>

int main() {
	pugi::xml_document doc;

	//pugi::xml_parse_result result = doc.load_file("s_sensor.xml");
	pugi::xml_parse_result result = doc.load_file("s_function.xml");
	//pugi::xml_parse_result result = doc.load_file("s_program.xml");
    
	if (!result) return -1;

	// Get node called servletmsg
	pugi::xml_node msg = doc.child("servletmsg");

	// Get servletmsg attribute called sid
	//int sid = msg.attribute("sid").as_int();
	std::string sid = msg.attribute("sid").value();

	// Get servletmsg children: recipient and request_type
	std::string recipient = msg.child("recipient").child_value();
	std::string request_type = msg.child("request_type").child_value();
	
	std::cout << "Recipient: " << recipient << ", request type: " << request_type << std::endl;

	if (request_type == "sensor") {
		
		std::string sensor = msg.child("request").child("sensor_request").child_value(); // ALL | LUM | PRES
		std::cout << "sensor request: " << sensor << std::endl;

	} else if (request_type == "function") {
		
		std::string functionType = msg.child("request").child("function").attribute("type").value();
		std::cout << "function: " << functionType << std::endl;
	
		if (functionType == "turn_off") {
			std::string functionTime = msg.child("request").child("function").child("time").child_value();
			std::cout << "time: " << functionTime << std::endl;
		} else if (functionType == "change_color") {
			std::string hexColor = msg.child("request").child("function").child("color_hex").child_value();
			std::string functionTime = msg.child("request").child("function").child("time").child_value();
			
			std::cout << "hexColor: " << hexColor << ", time: " << functionTime << std::endl;
		}


	} else if (request_type == "program") {
		
		std::string program = msg.child("request").child("program").child_value();
		std::cout << "program: " << program << std::endl;

	}

	std::cout << "Load result: " << result.description() << ", sid: " << sid << std::endl;	
}
