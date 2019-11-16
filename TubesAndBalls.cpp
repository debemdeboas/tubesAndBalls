#include "TubesAndBalls.h"
#include "Tube.h"

using namespace std;

int GetTubeIDFromUser(unordered_map<int, Tube*> SiberiusMachine);
int RollBall(Tube*);
void Play(unordered_map<int, Tube*>);
void Simulate(unordered_map<int, Tube*>);
void FindMostCommonEnding(unordered_map<int, Tube*>);
unordered_map<int, Tube*> ReadConfigFile(string fName);
vector<string> tokenizer(string const& s, char delim);

struct Log
{
	bool Verbose = false;

	void ShowMenu()
	{
		cout << "*-====-* Main Menu *-====-*" << endl <<
			"1. Play" << endl <<
			"2. Simulate" << endl <<
			"3. Find most common ending" << endl <<
			"4. Options" << endl <<
			"0. Exit" << endl <<
			"> ";
	}

	void ShowOptionsMenu()
	{
		cout << "===+-+=== Options ===+-+===" << endl <<
			"1. Enable verbosity (warning: greatly reduces performance!)" << endl <<
			"2. Disable verbosity" << endl <<
			"> ";
	}

	void Info(string msg) { if (Verbose) Show(msg); }
	void Show(string msg) { cout << msg << endl; }
} Screen;

int main()
{
	cout << "Hello and welcome to Mr. Siberius\' Fantastic Tubes and Balls machine!" << endl <<
		"Select an input file (1 through 8): ";

	string fileNumber;
	cin >> fileNumber;

	auto f = "./data/caso" + fileNumber + ".txt";

	cout << "Please wait while I configure the system." << endl <<
		"Using configuration schematics loaded from " << f << endl;
	auto SiberiusMachine = ReadConfigFile(f);

	cout << "Configuration complete." << endl << endl;

	bool KeepGoing = true;
	int choice;
	do
	{
		Screen.ShowMenu();
		cin >> choice;
		cout << endl;
		switch (choice)
		{
		case 1: // Play
			Play(SiberiusMachine);
			break;
		case 2: // Simulate
			Simulate(SiberiusMachine);
			break;
		case 3: // Most common ending
			FindMostCommonEnding(SiberiusMachine);
			break;
		case 4: // Options
			Screen.ShowOptionsMenu();
			int optionsChoice;
			cin >> optionsChoice;
			switch (optionsChoice)
			{
			case 1:
				Screen.Verbose = true;
				break;
			case 2:
			default:
				Screen.Verbose = false;
			}
			cout << endl;
			break;
		case 0: // Exit
			cout << "Thank you for playing, come back soon." << endl;
			KeepGoing = false;
			break;
		default:
			cout << "Invalid option." << endl << endl;
			continue;
		}
	} while (KeepGoing);

	return 0;
}

unordered_map<int, Tube*> ReadConfigFile(string fName)
{
	ifstream infile(fName);

	if (infile.fail())
	{
		cerr << "Error: Could not open file " << fName;
		exit(EXIT_FAILURE);
	}

	unordered_map<int, Tube*> Result;

	if (infile.good())
	{
		string line;
		getline(infile, line);

		// [0] -> Tubes count
		// [1] -> Number of subsections in a tube
		auto BasicConfig = tokenizer(line, ' ');

		int NumTubes = stoi(BasicConfig.at(0));
		int TubeSize = stoi(BasicConfig.at(1));

		for (int i = 0; i < NumTubes; ++i)
			Result.insert(make_pair(i, new Tube(i, TubeSize)));

		while (infile.good())
		{
			getline(infile, line);

			if (line.empty())
				continue;

			BasicConfig = tokenizer(line, ' ');
			int TubeID = stoi(BasicConfig[0]);
			int From = stoi(BasicConfig[1]);
			int ToTube = stoi(BasicConfig[2]);
			int ToPos = stoi(BasicConfig[3]);
			BasicConfig.clear();

			Tube *AuxTube = Result[TubeID];
			AuxTube->PosToTube[From] = pair<Tube*, int>(Result[ToTube], ToPos);
		}
	}
	return Result;
}

vector<string> tokenizer(string const& s, char delim)
{
	vector<string> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delim))
		tokens.push_back(token);
	return tokens;
}

void Play(unordered_map<int, Tube*> SiberiusMachine)
{
	int ID = GetTubeIDFromUser(SiberiusMachine);
	if (ID == -1) { return; }

	bool bEnd = true;
	int endingID;
	do
	{
		cout << "In what tube do you think that the ball is going to end on?" << endl << "> ";
		cin >> endingID;
		if (SiberiusMachine.find(endingID) == SiberiusMachine.end())
		{
			cout << "Invalid tube." << endl << endl;
			bEnd = false;
		}
		else
		{
			bEnd = true;
		}
	} while (!bEnd);

	Tube* selectedTube = SiberiusMachine[ID];
	int endedOn = RollBall(selectedTube);
	if (endedOn == endingID)
	{
		Screen.Show("Congratulations, you won!\n\n");
	}
	else
	{
		Screen.Show("I\'m sorry, you\'ve guessed wrong. Maybe next time.\n");
		Screen.Info("The ball from tube " + to_string(ID) + " ended on tube " + to_string(endedOn) + ".\n");
	}
}

void Simulate(unordered_map<int, Tube*> SiberiusMachine)
{
	int ID = GetTubeIDFromUser(SiberiusMachine);
	if (ID == -1) { return; }

	Tube* selectedTube = SiberiusMachine[ID];

	cout << "The ball ended on tube " << RollBall(selectedTube) << "." << endl;
	cout << endl;
}

int GetTubeIDFromUser(unordered_map<int, Tube*> SiberiusMachine)
{
	cout << "Select a tube from 0 to " << SiberiusMachine.size() - 1 << endl << "> ";
	int tubeID;
	cin >> tubeID;

	if (SiberiusMachine.find(tubeID) == SiberiusMachine.end())
	{
		cout << "Invalid tube." << endl << endl;
		return -1;
	}

	cout << "Tube " << tubeID << " selected. Inserting ball..." << endl;
	return tubeID;
}

void FindMostCommonEnding(unordered_map<int, Tube*> SiberiusMachine)
{
	// First: Tube ID
	// Second: Number of times a ball ended in this tube
	unordered_map<int, int> endings;
	for (auto&& IDToTube : SiberiusMachine)
	{
		++endings[RollBall(IDToTube.second)];
	}

	auto min = *min_element(endings.begin(), endings.end(),
		[](pair<int, int> a, pair<int, int> b) {
			return a.second > b.second;
		}
	);

	Screen.Show("The tube in which most balls ended is tube " + to_string(min.first) + ", with a total of " + to_string(min.second) + " balls.\n");
}

int RollBall(Tube* tube)
{
	int TubeSize = tube->TubeSize(), originalID = tube->ID;
	for (int i = 0; i <= TubeSize; ++i)
	{
		Screen.Info("Tube [" + to_string(tube->ID) + "]: " + to_string(i));
		if (tube->PosToTube.find(i) != tube->PosToTube.end())
		{
			int oldID = tube->ID;
			auto newTubePair = tube->PosToTube[i];
			tube = newTubePair.first;
			i = --newTubePair.second;
			Screen.Info("Tube " + to_string(oldID) + " on position " + to_string(i) + ": " + "Redirecting to tube " + to_string(tube->ID));
		}
	}

	Screen.Info("Tube " + to_string(originalID) + ": Ball ended on tube " + to_string(tube->ID) + ".");
	return tube->ID;
}