#include "inotify-cxx.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


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

int main(void) {

	Inotify notify;
	string watch_dir = "/dev/shm";
	//InotifyWatch watch(watch_dir, IN_ALL_EVENTS);
	InotifyWatch watch(watch_dir, IN_CLOSE_WRITE);
	notify.Add(watch);
	cout << "Watching directory " << watch_dir << endl << endl;

	for (;;) {
		notify.WaitForEvents();

		size_t count = notify.GetEventCount();
		while (count > 0) {
			InotifyEvent event;
			bool got_event = notify.GetEvent(&event);

			if (got_event) {
				string mask_str;
				event.DumpTypes(mask_str);

				string filename = event.GetName();

				if (filename == "message") {
					cout << "[watch " << watch_dir << "] ";
					cout << "event mask: \"" << mask_str << "\", ";
					cout << "filename: \"" << filename << "\"" << endl;

					ofstream flagFile ("/dev/shm/flag.txt");
					if (flagFile.is_open()) {
						flagFile << "1";
					}
					flagFile.close();
				}

			}

			count--;
		}
	}


}
