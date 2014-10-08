#include "pugixml.hpp"
#include <iostream>
#include <string>
#include <bitset>
#include <sstream>
#include <algorithm>

std::string decToBin(int number) {
	std::string result = "";
	do {
		if ( (number & 1) == 0 )
			result += "0";
		else
			result += "1";

		number >>= 1;
	} while (number);

 	reverse(result.begin(), result.end());
	return result;
}

void setServletId(std::bitset<256> payload, std::string servletId) {

	int id = std::stoi(servletId); // servletId as int
	std::string binaryString = decToBin(id); // servletId as binary

	std::string final = std::string(64 - binaryString.length(), '0').append(binaryString);
	std::cout << "servlet final: " << final << ", length: " << final.length() << std::endl;
}

void setRecipient(std::bitset<256> payload, std::string recipient) {

	int rec = std::stoi(recipient); // recipient as int
	std::string binaryString = decToBin(rec); // recipient as binary

	std::string final = std::string(8 - binaryString.length(), '0').append(binaryString);

	std::cout << "rec: " << rec << ", binaryString " << binaryString << std::endl;
	std::cout << "recipient final: " << final << ", length: " << final.length() << std::endl;
}

void setUnixTime(std::bitset<256> payload, std::string unixTime) {

	int time = std::stoi(unixTime); // unixTime as int
	std::string binaryString = decToBin(time); // unixTime as binary
	
	std::string final = std::string(32 - binaryString.length(), '0').append(binaryString);
	std::cout << "unix time final: " << final << ", length: " << final.length() << std::endl;
}

int main() {
	pugi::xml_document doc;
	std::bitset<256> payload; // 32 bytes

	// Parameters common to every message
	std::bitset<64> servletId; // 8 bytes
	std::bitset<8> recipient; // 1 byte
	std::bitset<32> unixTime; // 4 bytes

	//pugi::xml_parse_result result = doc.load_file("s_sensor.xml");
	pugi::xml_parse_result result = doc.load_file("s_function_changecolor.xml");
	//pugi::xml_parse_result result = doc.load_file("s_function_turnoff.xml");
	//pugi::xml_parse_result result = doc.load_file("s_program.xml");

	if (!result) return -1;

	// Get node called servletmsg
	pugi::xml_node msg = doc.child("servletmsg");

	// Get servletmsg attribute called sid
	//int sid = msg.attribute("sid").as_int();
	std::string sid = msg.attribute("sid").value();
	std::cout << "Load result: " << result.description() << "\nsid: " << sid << std::endl;	

	// Assign servlet id bits to payload
	setServletId(payload, sid);

	// Get servletmsg children: recipient and request_type
	std::string recipientTest = msg.child("recipient").child_value();
	std::string request_type = msg.child("request_type").child_value();

	// Assign recipient bits to payload
	setRecipient(payload, recipientTest);
	std::cout << "Recipient: " << recipientTest << ", request type: " << request_type << std::endl;

	if (request_type == "sensor") {
		// command: 0
		payload[0] = 0;

		// sensor: 000
		payload[1] = 0;
		payload[2] = 0;
		payload[3] = 0;

		std::cout << "sensor request" << std::endl;

	} else if (request_type == "function") {
		// command: 0
		payload[0] = 0;
		
		// function: 001
		payload[1] = 0;
		payload[2] = 0;
		payload[3] = 1;

		std::string functionType = msg.child("request").child("function").attribute("type").value();
		std::cout << "function: " << functionType << std::endl;
	
		std::string functionTime = msg.child("request").child("time").child_value();
		std::cout << "time: " << functionTime << std::endl;

		// Assign unix Time bits to payload
		setUnixTime(payload, functionTime);

		if (functionType == "turn_off") {
			// turn off: 0000
			payload[4] = 0;
			payload[5] = 0;
			payload[6] = 0;
			payload[7] = 0;

		} else if (functionType == "change_color") {
			// change color: 0001
			payload[4] = 0;
			payload[5] = 0;
			payload[6] = 0;
			payload[7] = 1;

			std::string hexColor = msg.child("request").child("function").child("color_hex").child_value();
			std::cout << "hexColor: " << hexColor << std::endl;
		}


	} else if (request_type == "program") {
		// program: 1
		payload[0] = 1;
		std::string program = msg.child("request").child("program").child_value();
		std::cout << "program: " << program << std::endl;

		std::string functionTime = msg.child("request").child("time").child_value();
		std::cout << "time: " << functionTime << std::endl;
	}

}
