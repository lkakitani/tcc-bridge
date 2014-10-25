#include "pugixml.hpp"
#include "inotify-cxx.h"
#include <iostream>
#include <string>
#include <bitset>
#include <sstream>
#include <algorithm>
#include <time.h>
using namespace std;

string hexToBin(string hex) {
	string result = "";
	for (int i = 0; i < hex.length(); i++) {
		switch(hex[i])
		{
			case '0':
				result += "0000";
				break;
			case '1':
				result += "1000";
				break;
			case '2':
				result += "0100";
				break;
			case '3':
				result += "1100";
				break;
			case '4':
				result += "0010";
				break;
			case '5':
				result += "1010";
				break;
			case '6':
				result += "0110";
				break;
			case '7':
				result += "1110";
				break;
			case '8':
				result += "0001";
				break;
			case '9':
				result += "1001";
				break;
			case 'A':
				result += "0101";
				break;
			case 'B':
				result += "1101";
				break;
			case 'C':
				result += "0011";
				break;
			case 'D':
				result += "1011";
				break;
			case 'E':
				result += "0111";
				break;
			case 'F':
				result += "1111";
				break;
		}
	}
	return result;
}

string decToBin(int number) {
	string result = "";
	do {
		if ( (number & 1) == 0 )
			result += "0";
		else
			result += "1";

		number >>= 1;
	} while (number);

 // 	reverse(result.begin(), result.end());
	return result;
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

void setServletId(bitset<256> &payload, string servletId) {

	int id = stoi(servletId); // servletId as int
	string binaryString = decToBin(id); // servletId as binary

	string final = string(64 - binaryString.length(), '0').append(binaryString);
	cout << "servlet final: " << final << ", length: " << final.length() << endl;
	
	reverse(final.begin(), final.end());
	for (int i = 0; i < 64; i++) {
		if (final[i] == '0')
			payload[i+8] = 0;
		else
			payload[i+8] = 1;
	}
}

void setRecipient(bitset<256> &payload, string recipient) {

	int rec = stoi(recipient); // recipient as int
	string binaryString = decToBin(rec); // recipient as binary

	string final = string(8 - binaryString.length(), '0').append(binaryString);

	cout << "rec: " << rec << ", binaryString " << binaryString << endl;
	cout << "recipient final: " << final << ", length: " << final.length() << endl;

	reverse(final.begin(), final.end());
	for (int i = 0; i < 8; i++) {
		if (final[i] == '0')
			payload[i+72] = 0;
		else
			payload[i+72] = 1;
	}
}

void setUnixTime(bitset<256> &payload, string unixTime) {

	int time = stoi(unixTime); // unixTime as int
	string binaryString = decToBin(time); // unixTime as binary
	
	string final = string(32 - binaryString.length(), '0').append(binaryString);
	cout << "unix time final: " << final << ", length: " << final.length() << endl;

	reverse(final.begin(), final.end());
	for (int i = 0; i < 32; i++) {
		if (final[i] == '0')
			payload[i+80] = 0;
		else
			payload[i+80] = 1;
	}
}

void setColor(bitset<256> &payload, string hexColor) {	
	string final = hexToBin(hexColor);
	cout << "hex color final: " << final << ", length: " << final.length() << endl;

// 	reverse(final.begin(), final.end());
	for (int i = 0; i < 24; i++) {
		if (final[i] == '0')
			payload[i+112] = 0;
		else
			payload[i+112] = 1;
	}
}

int main() {

	pugi::xml_document doc;

	// Notify code
// 	Inotify notify;
// 	string watch_dir = "./";
// 	InotifyWatch watch(watch_dir, IN_ALL_EVENTS);
// 	notify.Add(watch);
// 	cout << "Watching directory " << watch_dir << endl << endl;

// 	for (;;) {
// 		notify.WaitForEvents();

// 		size_t count = notify.GetEventCount();
// 		while (count > 0) {
// 			InotifyEvent event;
// 			bool got_event = notify.GetEvent(&event);

// 			if (got_event) {
// 				string mask_str;
// 				event.DumpTypes(mask_str);

// 				string filename = event.GetName();

// 				cout << "[" << currentDateTime() << " watch " << watch_dir << "] ";
// 				cout << "event mask: \"" << mask_str << "\", ";
// 				cout << "filename: \"" << filename << "\"" << endl;
// 			}

// 			count--;
// 		}
// 	}

	bitset<256> payload; // 32 bytes

	//pugi::xml_parse_result result = doc.load_file("s_sensor.xml");
	pugi::xml_parse_result result = doc.load_file("s_function_changecolor.xml");
	//pugi::xml_parse_result result = doc.load_file("s_function_turnoff.xml");
	//pugi::xml_parse_result result = doc.load_file("s_program.xml");

	if (!result) return -1;

	// Get node called servletmsg
	pugi::xml_node msg = doc.child("servletmsg");

	// Get servletmsg attribute called sid
	//int sid = msg.attribute("sid").as_int();
	string sid = msg.attribute("sid").value();
	cout << "Load result: " << result.description() << "\nsid: " << sid << endl;	

	// Assign servlet id bits to payload
	setServletId(payload, sid);
	
	// Get servletmsg children: recipient and request_type
	string recipientTest = msg.child("recipient").child_value();
	string request_type = msg.child("request_type").child_value();

	// Assign recipient bits to payload
	setRecipient(payload, recipientTest);
	cout << "Recipient: " << recipientTest << ", request type: " << request_type << endl;

	if (request_type == "sensor") {
		// command: 0
		payload[0] = 0;

		// sensor: 000
		payload[1] = 0;
		payload[2] = 0;
		payload[3] = 0;

		cout << "sensor request" << endl;

	} else if (request_type == "function") {
		// command: 0
		payload[0] = 0;
		
		// function: 001
		payload[1] = 0;
		payload[2] = 0;
		payload[3] = 1;

		string functionType = msg.child("request").child("function").attribute("type").value();
		cout << "function: " << functionType << endl;
	
		string functionTime = msg.child("request").child("time").child_value();
		cout << "time: " << functionTime << endl;

		// Assign Unix Time bits to payload
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

			string hexColor = msg.child("request").child("function").child("color_hex").child_value();
			cout << "hexColor: " << hexColor << endl;

			// Assign color bits to payload
			setColor(payload, hexColor);
		}


	} else if (request_type == "program") {
		// program: 1
		payload[0] = 1;
		string program = msg.child("request").child("program").child_value();
		cout << "program: " << program << endl;

		string functionTime = msg.child("request").child("time").child_value();
		cout << "time: " << functionTime << endl;
	}

	cout << "Payload: " << payload << endl;

	cout << endl << "Payload breakdown:" << endl;
	cout << "Payload[0-7]: ";
	int i;
	for (i = 0; i < 8; i++) {
		cout << payload[i];
	}
	cout << "\nServlet ID: ";
	for (i = 8; i < 72; i++) {
		cout << payload[i];
	}
	cout << "\nRecipient: ";
	for (i = 72; i < 80; i++) {
		cout << payload[i];
	}
	cout << "\nUnix time: ";
	for (i = 80; i < 112; i++) {
		cout << payload[i];
	}
	cout << "\nColor (if function change_color):";
	cout << "\nRed: ";
	for (i = 112; i < 120; i++) {
		cout << payload[i];
	}
	cout << "\nGreen: ";
	for (i = 120; i < 128; i++) {
		cout << payload[i];
	}
	cout << "\nBlue: ";
	for (i = 128; i < 136; i++) {
		cout << payload[i];
	}
	cout << endl;


}
