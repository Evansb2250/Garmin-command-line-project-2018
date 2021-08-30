// RunningApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<vector>
#include<exception>
using namespace std;

bool ImportFileDatabase(vector<string>&  filename); // step 1
void eraseData(struct RunnerStats& currentRunner); // step 2
int menu(); // step 3
int fileSelection(vector<string>&  filename);  //  step 4
void ReadFile(vector<string>& filename, struct RunnerStats& currentRunner); // step 5
void SeparateInformation(string& fileInput, bool& newLap, struct RunnerStats& currentRunner);// step 6
void RemoveTag(string& data, string openTag = " ", const string closingTag = "<"); // step 7
void allocationScheme(string tagLessDetails, int tagIndex, struct RunnerStats& currentRunner); // step 8
																							   //Mathmatical  Algorithms Below
int CalculateAverageSpeed(const double speed, enum timeUnit unit);
int CalculateLapTime(struct RunnerStats& currentRunner, enum scheme timeScheme);
void calculateAltitude(int altitude, struct RunnerStats& currentRunner);



// Graph And Lap
// one meter per second equals 2.23694 mph
const double METERSPERSECOND = 2.23694;
//used to convert the altitude in meters to elevation in feet
const double metersToFeet = 3.28084;
//The tags in the file to be compared
const string tags[] = { "<MaximumSpeed>", "<ns3:Speed>" , "<AltitudeMeters>", "<ns3:AvgSpeed>", "<ns3:AvgRunCadence>", "<ns3:MaxRunCadence>" };
const enum tagtype { MAXSPEED, SPEED, ALTITUDE, AVGSPEED, AVGCADENCE, MAXCADENCE };
const enum timeUnit { MINUTES, SECONDS };
enum scheme { LPMINUTES, LPSECONDS, RUNMINUTES, RUNSECONDS };
enum menuSelection { viewFile = 1, quit = 2 };


struct Time {
	int minutes = 0;
	int seconds = 0;

	void displayLapTime() {
		if (seconds < 10)
			cout << setw(5) << minutes << ":0" << seconds << setw(9);
		else
			cout << setw(5) << minutes << ":" << seconds << setw(9);
	}

};

struct Lap {

	double lapDistance, averageSpeed, maxSpeed = 0;// Distance in meters
	int averageCadence, maxCadence, moveCount = 0;
	struct Time lapTime, averagePaceInTime; // add a function that initialize the variable


											//Shouldn't be there 
	void displayLap() {
		lapTime.displayLapTime();
		cout << setprecision(3) << lapDistance << " mi";
		averagePaceInTime.displayLapTime();
		cout << " min./mi " << setw(5) << METERSPERSECOND * averageSpeed << " mph  "
			<< setw(7) << maxSpeed << " mph " << setw(8) << averageCadence << " SPM" << setw(10) <<
			maxCadence << " SPM";

	}




};

struct RunnerStats {
	vector<struct Lap> splitInformation;
	struct Time overalPace, overalMovingPace, bestPace, totalRunTime;
	int totalAverageOfCadence = 0, MaxCadence = 0, highestAltitude = 0, lowestAltitude = 200, stopCount = 0, moveCount = 0;
	double totalRan = 0, bestSpeedMph, avgMph = 0, pace = 0;

	void setValues() {
		// overalPace = setPace
		// overalMovingPace = 
		overalPace.minutes = CalculateAverageSpeed(pace / moveCount, MINUTES);
		overalPace.seconds = CalculateAverageSpeed(pace / moveCount, SECONDS);
		overalMovingPace.minutes = CalculateAverageSpeed(pace / (moveCount - stopCount), MINUTES);
		overalMovingPace.seconds = CalculateAverageSpeed(pace / (moveCount - stopCount), SECONDS);
		bestPace.minutes = CalculateAverageSpeed(bestSpeedMph / METERSPERSECOND, MINUTES);
		bestPace.seconds = CalculateAverageSpeed(bestSpeedMph / METERSPERSECOND, SECONDS);
	}

	//Display a header that uses the iomanip library to structure the data.
	void showSplits() {
		cout << setw(5) << " " << setw(10) << "Lap Time" << setw(13) << "Distance" << setw(12) << " Avg Pace" <<
			setw(13) << "Avg Mph" << setw(15) << "Max Mph " << setw(10) << "  Avg Cadence  " << setw(10) << "Max Cadence" << endl;
		//This will loop through each split in the run and display its' stats
		for (int index = 0; index < splitInformation.size(); index++) {
			cout << setw(5) << "Lap " << index + 1 << " ";
			splitInformation[index].displayLap();
			cout << endl;

		}
		cout << setw(10);

	}
	// Display ALL
	// Avg Pace min/mi
	//Avg Moving Pace min/mi
	//best pace min/mi
	//avg Speed 
	//Avg Moving Speed 
	//Max speed
	//

	//Displays overall stats of the run
	void DisplayTotalStats() {

		cout << "\nRUN STATS:\n\nTotal distance ran: " << totalRan << " mi\t\t" << "RUN TIME:";
		totalRunTime.displayLapTime();
		cout << "minutes\n" << endl;
		cout << "Average pace:";
		overalPace.displayLapTime();
		cout << " min/mi \n";
		cout << "Average moving pace:";
		overalMovingPace.displayLapTime();
		cout << " min/mi" << endl;
		cout << "Best pace:";
		bestPace.displayLapTime();
		cout << " min/mi \n";
		cout << "Average pace: " << METERSPERSECOND * (avgMph / splitInformation.size()) << " mph" << endl;
		cout << "Best pace: " << setprecision(3) << bestSpeedMph << " mph" << endl;
		cout << "Average Cadence: " << 2 * (totalAverageOfCadence / splitInformation.size()) << " SPM" << endl;
		cout << "Max Cadence: " << MaxCadence / splitInformation.size() << " SPM" << endl;
		cout << "Highest Elevation Reached: " << highestAltitude << " ft" << endl;
		cout << "Lowest Elevation Reached: " << lowestAltitude << " ft" << endl << endl;

	}


};










int main()
{
	vector<string> filename;
	bool on;
	int option;
	//Create the structure
	struct RunnerStats currentRunner;
	on = ImportFileDatabase(filename);

	while (on) {
		cout << "\n\n\t\t\tGARMIN RUN DATA ANALYZER" << endl;
		cout << "\t\t_________________________________________\n" << endl;
		if (currentRunner.splitInformation.size() != 0) { eraseData(currentRunner); }
		option = menu();
		if (option == 1) {
			ReadFile(filename, currentRunner);
			currentRunner.setValues();
			currentRunner.DisplayTotalStats();
			currentRunner.showSplits();
			cin.get();
		}
		else if (option == 2)
			on = false;

	}

	cout << "Program terminating.....";
	cin.get();
	return 0;
}
















//step 1
/* Read the names in the database file that should be in the directory and place them in a vector
To choose from.
*/
bool ImportFileDatabase(vector<string>&  filename) {
	string line;
	ifstream file("database.txt");

	if (!file) {
		cout << "System did not find file" << endl;
		return false;
	}

	else {
		while (file >> line) { filename.push_back(line); }
		return true;
	}

}

//step 2
//Erase the values from all the variable inside the  RunnerStat Struct and Lap Struct
void eraseData(struct RunnerStats& currentRunner) {
	currentRunner.splitInformation.erase(currentRunner.splitInformation.begin(), currentRunner.splitInformation.end());
	currentRunner.totalRan = 0;
	currentRunner.highestAltitude = 0;
	currentRunner.lowestAltitude = 200;
	currentRunner.bestSpeedMph = 0;
	currentRunner.avgMph = 0;
	currentRunner.totalAverageOfCadence = 0;
	currentRunner.moveCount = 0;
	currentRunner.stopCount = 0;
	currentRunner.pace = 0;
	currentRunner.MaxCadence = 0;
}

// Menu interface  // step 3
int menu() {
	int selection;

	cout << "\nEnter 1 to view files " << endl;
	cout << "Enter 2 to end program" << endl;

	cin >> selection;


	if (selection == 1) {
		return 1;
	}
	else if (selection == 2) {
		return 2;
	}
	else if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "Please enter only 1 or 2 ";
	}





}





//step 4
/*
This function iterates through the vector that contains the different filenames.
The user selects a file and it is passed to the ifsream to be read.
if the file was not found in the directory the method will display that the
file could not be found and the method would call itself again to let the user
pick a different selection.
*/
void ReadFile(vector<string>& filename, struct RunnerStats& currentRunner) {
	string lineContent;

	/*
	This variable acts as a switch to determine to create a new lap struct inside the splitInformation struct.
	the number of tags identified as <lap> is determined by the number of laps you ran
	*/
	bool newLap = false;
	//This line calls the fileSelection function that returns an index to specify the string filename to open and read.
	ifstream file(filename[fileSelection(filename)]);
	if (!file) {
		cout << "System did not find file" << endl;
		ReadFile(filename, currentRunner);
	}
	else {
		//This line will read the file and call the SeparateInformation function 
		while (file >> lineContent) { SeparateInformation(lineContent, newLap, currentRunner); }
	}



}

//Step 5
/*
This function scans through the filename vector and prompts the user to select one of the files.
If the user enters an input that is not an option to select, the user will be prompted to enter a valid number.
The loop will continue until a correct input is made by the user. The value entered will then be returned to sepcify
which index in the filename vector to read from.
*/
int fileSelection(vector<string>&  filename) {
	int selection;
	bool invalid = true;
	while (invalid) {
		for (int index = 0; index < filename.size(); index++) {
			cout << "\npress " << index + 1 << " to view run file on " << filename[index];
		}
		cout << "\npick one " << endl;
		cin >> selection;
		//cin.fail checks to see if something unexpected happend for example the wrong format.
		if (selection <= 0 || selection > filename.size() || cin.fail()) {
			cin.clear(); // This takes the cin out of the failed state so the user can recover. 
			cin.ignore(); //Extract the character from the strean
			cout << "Please enter a number " << 1 << " - " << filename.size() + 1 << endl;
		}
		else invalid = false;
	}
	return selection - 1;
}


//Step 6
/*
This function takes in the data be read from the file, the boolean variable newLap, and the main structure RunnerStats

This function checks each tag and the file and verifies if it is important information that will support the programs analysis.
The first tag is the the lap tag this will tell the program to create a new Lap structure and place it in the splitInformation vector.
The splitInformation vector isolates the data found in each lap. The newLap variable will act as a switch and change its value to true
*/
void SeparateInformation(string& fileInput, bool& newLap, struct RunnerStats& currentRunner) {
	if (fileInput == "<Lap") {
		newLap = true;
		struct Lap nwLap;
		currentRunner.splitInformation.push_back(nwLap);
	}
	/*
	Each lap contains the total distance ran each lap once the it is determined that, that tag has been found while the newLap variable is still true
	it will parse the string to a double variable formate and allocate it into the lap distance member in the last element of the vector.
	newlap will then turn to false until the program detects the runner ran another lap.
	*/
	if (fileInput.find("<DistanceMeters>") == false && newLap == true) {
		newLap = false;
		RemoveTag(fileInput, "<DistanceMeters>");
		currentRunner.splitInformation.back().lapDistance = atof(fileInput.c_str()) * 0.000621371;
		currentRunner.totalRan += currentRunner.splitInformation.back().lapDistance;
	}
	/*
	This for loop is a short hand method to check through all the tags in the variable array of type string labled "tags".
	the loop will iterate through each of the elements to check if it matches the input coming from the file. If it doesn't it will
	loop to the end and search the next line in the file. However, if it does it will call the RemoveTag function that will
	remove the tags in the string so that information can be stored in a variable.

	Once the tags are removed the string will then placed in the function "allocationScheme" which will determine which data type it needs
	to be converted to and which member in the structs it goes to. The function will then break because it found a match and it will search
	the next line in the file until the last line is read.
	*/
	for (int index = 0; index < 6; index++) {
		if (fileInput.find(tags[index]) == false) {
			RemoveTag(fileInput, tags[index]);
			allocationScheme(fileInput, index, currentRunner);
			break;
		}
	}

}

//Step 7
//This function removes the begining and end tags.
void RemoveTag(string& data, string openTag, string closingTag) {
	data.erase(0, openTag.length());

	int tagIndex = data.find(closingTag);

	data.erase(tagIndex);

}

//Step 8
//This function allocates data to the different members in the RunnerStats structure.
//The data is recorded in meters per second for speed. To convert into miles per hour I multiply it by the METERSPERSECOND variable.
//atoi converts a string to an int , and atof converts a string to a double
void allocationScheme(string tagLessDetails, int tagIndex, struct RunnerStats& currentRunner) {

	switch (tagIndex) {
	case MAXSPEED:
		currentRunner.splitInformation.back().maxSpeed = METERSPERSECOND * atof(tagLessDetails.c_str());
		//Collects the highest value amongst the MaxSpeed tags
		if (currentRunner.splitInformation.back().maxSpeed > currentRunner.bestSpeedMph)
			currentRunner.bestSpeedMph = currentRunner.splitInformation.back().maxSpeed;
		break;
	case SPEED:
		if (atof(tagLessDetails.c_str()) == 0) { currentRunner.stopCount += 1; }
		currentRunner.pace += atof(tagLessDetails.c_str());
		//Calculates how many speed tags that are in the file
		currentRunner.moveCount += 1;
		break;
	case ALTITUDE:
		calculateAltitude(atoi(tagLessDetails.c_str()), currentRunner);
		break;
	case AVGSPEED:
		currentRunner.splitInformation.back().averageSpeed = atof(tagLessDetails.c_str());
		currentRunner.avgMph += atof(tagLessDetails.c_str());
		//This line take the averageSpeed tag value that is in meters per second convert it to mph and then solves how much time it took to finish a mile
		//at that speed and return the value into the time structure I created.
		currentRunner.splitInformation.back().averagePaceInTime.minutes = CalculateAverageSpeed(currentRunner.splitInformation.back().averageSpeed, MINUTES);
		currentRunner.splitInformation.back().averagePaceInTime.seconds = CalculateAverageSpeed(currentRunner.splitInformation.back().averageSpeed, SECONDS);

		//Calculate Lap time checks to see how much of the lap was ran at what speed and determines how long it took the runner to finish
		//at that given pace. The function above only calculates the average speed you ran in time, but not what time the runner finished.
		currentRunner.splitInformation.back().lapTime.minutes = CalculateLapTime(currentRunner, LPMINUTES);
		currentRunner.splitInformation.back().lapTime.seconds = CalculateLapTime(currentRunner, LPSECONDS);
		currentRunner.totalRunTime.minutes = CalculateLapTime(currentRunner, RUNMINUTES);
		currentRunner.totalRunTime.seconds = CalculateLapTime(currentRunner, RUNSECONDS);

		break;
	case AVGCADENCE:
		//multiplies the cadence by 2 to get the correct Steps per Minute
		currentRunner.splitInformation.back().averageCadence = 2 * atoi(tagLessDetails.c_str());
		currentRunner.totalAverageOfCadence += atoi(tagLessDetails.c_str());

		break;
		//multiplies the cadence by 2 to get the correct Steps per Minute
	case  MAXCADENCE:
		currentRunner.splitInformation.back().maxCadence = 2 * atoi(tagLessDetails.c_str());
		currentRunner.MaxCadence += 2 * atoi(tagLessDetails.c_str());
		break;

	}

}

// add pace up
// Divide it by the active numbers
//initialize it with CalculateAverage Speed put boolean on the end 
//



// Calculation Algorithms 
int CalculateAverageSpeed(const double speed, enum timeUnit unitTime) {

	/*
	this if statement calulates how many minutes it would take you to run at a specific speed and return its' value.
	example     1 mile ran at 7.65 mph
	1/7.65 =  .1307189542
	to get the minute value from this number multiply by 60 because there are 60 seconds in a minute
	6 * .1307189542 = 7.843137255
	this number will be returned and only the number after the decimal point will be allocated.
	*/
	if (unitTime == MINUTES)
	{
		return  (60 * (1 / (speed * METERSPERSECOND)));
	}
	/*
	To get the value of seconds you will then take the same value 7.843137255  and subtract its' whole number so only the digits behind the decimal
	point are left .843137255.  You then multiply this number by 60 and any number after the decimal point is the value in seconds.
	50 seconds.
	if the runner averaged 7.65mph he finished the lap in 7:50 min/mi

	*/
	else {
		return round(((60 * (1 / (speed * METERSPERSECOND))) - int(60 * (1 / (speed * METERSPERSECOND)))) * 60);

	}

}

/* This function focuses on calculating the time it ran each lap while accounting that the user may have not ran the full lap distance
which is a mile.
*/
int CalculateLapTime(struct RunnerStats& currentRunner, enum scheme timeScheme) {
	//I turn the average pace time into seconds and multiply it with the distance ran in miles

	/*
	This line takes the avarage time value and converts it into seconds. It then multiplies it by any number between 0 to 1.
	this value is then stored in the variable called lapConvertedToSeconds
	*/
	double lapConvertedToSeconds = ((currentRunner.splitInformation.back().averagePaceInTime.minutes * 60)
		+ currentRunner.splitInformation.back().averagePaceInTime.seconds) *  currentRunner.splitInformation.back().lapDistance;

	double totalAmountOfSeconds = 0;
	//this loop figures the number of seconds it took to run. As the vector grows the program will change its value to reflect the added laps.
	for (int index = 0; index < currentRunner.splitInformation.size(); index++) {
		totalAmountOfSeconds += (currentRunner.splitInformation[index].lapTime.minutes * 60) + currentRunner.splitInformation[index].lapTime.seconds;
	}


	//Below is the formula to calculate the time it took to run the lap
	//Bellow it has 4 different schemes the first 2 are for the individual laps 
	//The last 2 calculate how long it took to run the whole run.
	switch (timeScheme) {
	case LPMINUTES:                   //step two now divides the number by 60 to get the minutes of the lap
		return lapConvertedToSeconds / 60;
	case LPSECONDS:
		return ((lapConvertedToSeconds / 60) - currentRunner.splitInformation.back().lapTime.minutes) * 60;
	case RUNMINUTES:
		return totalAmountOfSeconds / 60;
	case RUNSECONDS:
		return ((totalAmountOfSeconds / 60) - currentRunner.totalRunTime.minutes) * 60;

	}
}







//This function checks to find the highest and lowest altitude in each lap
void calculateAltitude(int altitude, struct RunnerStats& currentRunner) {
	if (altitude * metersToFeet  > currentRunner.highestAltitude) {
		currentRunner.highestAltitude = altitude * metersToFeet;
	}

	else if (altitude * metersToFeet  <  currentRunner.lowestAltitude) {
		currentRunner.lowestAltitude = altitude * metersToFeet;
	}

}


