#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <time.h>
#include <string>
#include <vector>
#include <algorithm> 
#include <iterator>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#define MAXTIMEDIGITS       5
#define MAXHOURS			23
#define MAXMINUTES			59

enum menu { add = 1, erase, show_all, show_in_range, fuel_consumption, save, load, clear, quit};

using namespace std;

/*Program for keeping track of your driving.
Key features of program are:
	- Add driving records
	- Delete driving records by hand or deleting entire list at once
	- Inspect entire log records or from user given distance limitations
	- Keep track of your average fuel consumption
	- Save and load your log on disc
*/

//Class for storing and containing logs of driving data
class Diary {
public:
	Diary();
	void start_trip();	//collect data of starting driving
	void end_trip();	//collect data of ending driving
	void printer();	//print trip data to console
	float get_distance();
	float get_refill();
	friend ostream & operator << (ostream &out, const Diary & data);	//overloaded operator for writing state to drive
	friend istream & operator >> (istream &in, Diary & data);			//overloaded operator for reading state from drive

private:
	string location_start;
	int hours_start;
	int minutes_start;
	int odometer_start;
	string location_end;
	int hours_end;
	int minutes_end;
	int odometer_end;
	float refill;
};

//Functions for verifying valid user input, implementations can be found below main.
void read_int(string str, int &num);
void read_float(string str, float &num);
void read_time(string str, int &hours, int &minutes);

int main()
{
	vector<Diary> vect; //vector that contains all driving logs given by user or loaded from drive
	Diary diary;	//temporary Diary container for gathering data to be pushed to vector
	int index, to_erase, lower_distance_limit, upper_distance_limit, listing;
	float refill_all, distance_all;
	bool loop = true;	//for staying within menu until command for terminating is given

	cout << "Welcome to driving diary." << endl;
	do {
		cout << "Input number of action you'd like to institute." << endl;
		cout << "1) Add new trip" << endl;
		cout << "2) Delete trip" << endl;
		cout << "3) Show all trips" << endl;
		cout << "4) Show trips with given lower and upper travel distance" << endl;
		cout << "5) Show your average fuel consumption (calculated by your odometer readings and fuel refills)" << endl;
		cout << "6) Save data to file" << endl;
		cout << "7) Load data from file" << endl;
		cout << "8) Clear all trips" << endl;
		cout << "9) Quit" << endl;
		read_int("", index);
		cout << endl;

		/*Switch statement for choosing main menu actions indicated in before.
		Commands between 1 and 9 are valid and other inputs give message of invalid input.*/
		switch (index) {
		case add:
			//Create new trip and push it to vector
			diary.start_trip();
			cout << "Getting to location" << endl;
			system("pause");
			diary.end_trip();
			vect.push_back(diary);
			cout << "Trip added" << endl;
			break;
		case erase:
			//delete trip of choosing from vector
			do {
				cout << "Which element you want to remove? (between 1 and " << vect.size() << ")" << endl;
				cin >> to_erase;
				if (to_erase <= 1 || to_erase >= vect.size()) {
					cout << "Trip with given index does'nt exist." << endl;
				}
			} while (to_erase <= 1 || to_erase >= vect.size());
			
			vect.erase(vect.begin() + to_erase - 1);
			cout << "Trip number " << to_erase <<" deleted." << endl;
			break;
		case show_all:
			//Show all trips stored in vector
			listing = 1;
			cout << "Showing all trips:" << endl << endl;
			for (auto it = vect.begin(); it != vect.end(); ++it) {
				cout << "Trip number " << listing << ":" << endl;
				(*it).printer();
				cout << endl;
				listing++;
			}
			break;
		case show_in_range:
			/*Show trips between user given distance limits.
			Note that trip indexing numbers match the original index  to help going through base of multiple trips*/
			listing = 1;

			read_int("Give lower limit of distance.\n", lower_distance_limit);

			read_int("Give upper limit of distance.\n", upper_distance_limit);
			cout << endl << "Showing all trips with distance between " << lower_distance_limit << " and " << upper_distance_limit << " kilometers: " << endl << endl;
			for (auto it = vect.begin(); it != vect.end(); ++it) {
				if ((*it).get_distance() > lower_distance_limit && (*it).get_distance() < upper_distance_limit) {
					cout << "Trip number " << listing << ":" << endl;
					(*it).printer();
					cout << endl;
				}
				listing++;
			}
			break;
		case fuel_consumption:
			//Show on terminal overall driven distance, overall refill ammount and average fuel consumption per 100 kilometers.
			refill_all = 0;
			distance_all = 0;
			for (auto it = vect.begin(); it != vect.end(); ++it) {
				refill_all += (*it).get_refill();
				distance_all += (*it).get_distance();
			}
			cout << "You have driven " << distance_all << " kilometers and refilled " << refill_all << " litters of gasoline" << endl;
			cout << "Your average fuel consumption is " << (refill_all / distance_all) * 100 << " litters per 100 kilometers." << endl;
			break;
		case save:
			/*Save contents of current vector to drive on file drivingDiary.txt.
			Only one savestate is allowed so making new save will erase existing file*/
		{
			ofstream out("drivingDiary.txt");
			for (auto it = vect.begin(); it != vect.end(); ++it) {
				out << *it;
			}
			out.close();
		}
		cout << "Save completed" << endl;
		break;
		case load:
			//Read data from drivingDiary.txt and store it to temporary Diary object to be pushed to vector
			vect.erase(vect.begin(), vect.end()); // empty vector before loading data
			{
				ifstream in("drivingDiary.txt");
				while (!in.eof()) {
					Diary temp;
					in >> temp;
					vect.push_back(temp);
				}
				in.close();
				vect.pop_back();
			}
			cout << "Load completed." << endl;
			break;
		case clear:
			//Empty the vector from all current data
			vect.erase(vect.begin(), vect.end());
			cout << "System wiped clean." << endl;
			break;
		case quit:
			//End running the program
			loop = false;
			break;
		default:
			//Message for invalid input
			cout << "Invalid input." << endl;
		}
		cout << endl;
	} while (loop == true);
	cout << "Terminated." << endl;

	system("pause");

	return 0;
}

Diary::Diary() {
	location_start="";
	hours_start=00;
	minutes_start=00;
	odometer_start=0;
	location_end="";
	hours_end=00;
	minutes_end=00;
	odometer_end=1;
	refill=0;
}

//Member function to gather data from starting the trip
void Diary::start_trip() {
	cout << "Starting location:" << endl;
	cin >> location_start;

	read_int("Odometer value on departure\n", odometer_start);

	//loop for chekcking that user input for hours and minutes are within range
	do {
		read_time("Time of departure?\n", hours_start, minutes_start);
		if (hours_start > MAXHOURS || minutes_start > MAXMINUTES) {
			cout << "Invalid time." << endl;
		}
	} while (hours_start > MAXHOURS || minutes_start > MAXMINUTES);
}
//Member function to gather data from ending the trip
void Diary::end_trip() {
	cout << "Arrival location: " << endl;
	cin >> location_end;

	//loop for checking that given odometer value is'nt lower than on start of a trip
	do {
		read_int("Odometer value on arrival\n", odometer_end);
		if (odometer_end <= odometer_start) {
			cout << "Odometer value on end can't be less than it was when trip started." << endl;
		}
	} while (odometer_end <= odometer_start);

	//same loop for checking valid inout of time
	do {
		read_time("Time of arrival?\n", hours_end, minutes_end);
		if (hours_end > MAXHOURS || minutes_end > MAXMINUTES) {
			cout << "Invalid time." << endl;
		}
	} while (hours_end > MAXHOURS || minutes_end > MAXMINUTES);

	read_float("How much you refilled fuel (give 0 if you didn't refill)\n", refill);
}

//Member function for printing trip log to terminal
void Diary::printer() {
	cout << "Start of the trip:\t" << location_start << endl;
	cout << "End of the trip:\t" << location_end << endl;
	cout << "Time of departure:\t" << setw(2) << setfill('0') << hours_start << ":" << setw(2) << setfill('0') << minutes_start << endl;
	cout << "Time of arrival:\t" << setw(2) << setfill('0') << hours_end << ":" << setw(2) << setfill('0') << minutes_end << endl;
	cout << "Odometer on departure:\t" << odometer_start << " kilometers" << endl;
	cout << "Odometer on arrival:\t" << odometer_end << " kilometers" << endl;
	cout << "Distance travelled:\t" << odometer_end - odometer_start << " kilometers" << endl;
	cout << "Refill:\t\t\t" << refill << " litters" << endl;
}

//Member function for getting trips distance by subtracting odometer value from end and start
float Diary::get_distance() {
	float dist = odometer_end - odometer_start;
	return dist;
}

//Member function for getting refill data
float Diary::get_refill() {
	return refill;
}

//operator overload for writing data to disc
ostream &operator<<(ostream &out, const Diary & data) {
	out << data.location_start << endl << data.hours_start << endl << data.minutes_start << endl << data.odometer_start << endl << data.location_end << endl << data.hours_end << endl << data.minutes_end << endl << data.odometer_end << endl << data.refill << endl;
	return out;
}

//operator overload for reading data from disc
istream &operator>>(istream &in, Diary & data) {
	in >> data.location_start;
	in >> data.hours_start;
	in >> data.minutes_start;
	in >> data.odometer_start;
	in >> data.location_end;
	in >> data.hours_end;
	in >> data.minutes_end;
	in >> data.odometer_end;
	in >> data.refill;

	return in;
}

//Three functions for verifying valid user inputs
void read_int(string str, int &num) {
	while (cout << str && !(cin >> num))
	{
		cout << "That's not valid input. Try again" << endl << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
}

void read_float(string str, float &num) {
	while (cout << str && !(cin >> num))
	{
		cout << "That's not valid input. Try again" << endl << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
}

void read_time(string str, int &hours, int &minutes) {
	char colon;
	while (cout << str && !(cin >> hours >> colon >> minutes)) {
		cout << "That's not valid input. Try again" << endl << endl;
		cin.clear();
		cin.ignore(MAXTIMEDIGITS, '\n');
	}
}


