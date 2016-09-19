#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

bool compare(string p, string q){
	return p.length() < q.length();
}

bool isStrAlpha(string s){
	for(int i=0;i<s.length();i++)
		if(!isalpha(s[i]))
			return false;
	return true;
}

string convertStrToLower(string s){
	for(int i=0;i<s.length();i++){
		s[i] = tolower(s[i]);
	}
	return s;
}

bool charPresentInString(string s, char c){
	for(int i=0;i<s.length();i++){
		if(s[i] == c)
			return true;
	}
	return false;
}

char getMaxFreqChar(vector<string>& words, vector<bool>& shouldUse, int start, int end, vector<char>& hitList, vector<char>& missList){

	vector<int> freqs(26, 0);
	for(int i=start; i<end;i++){

		if(shouldUse[i]){
			
			if(hitList.size() > 0){
				bool flag = false;
				char c = hitList.back();

				if(charPresentInString(words[i], c)){
					flag = true;
				}
				
				if(!flag){
					shouldUse[i] = 0;
					continue;
				}
			}
			
			if(missList.size() > 0){
				bool flag = true;
				char d = missList.back();

				if(charPresentInString(words[i], d)){
					flag = false;
				}
		
				if(!flag){
					shouldUse[i] = 0;
					continue;
				}
			}

			// both conditions satisfied, good for calculating frequencies
			for(int j=0;j<words[i].length();j++){
				freqs[int(words[i][j])-97] += 1;
			}
		}
	}

	for(int i=0;i<missList.size();i++){
		freqs[int(missList[i])-97] = -100000000;
	}
	for(int i=0;i<hitList.size();i++){
		freqs[int(hitList[i])-97] = -100000000;
	}

	int maxFreq = 0;
	char c = 'z';
	for(int i=0;i<26;i++){
		if(freqs[i] > maxFreq){
			maxFreq = freqs[i];
			c = char(97+i);
		}
	}

	return c;
}

void printBlanks(string blanks, vector<char>& missList){
	cout<<blanks<<	" missed: ";
	for(int i=0;i<missList.size();i++)
		cout<<missList[i]<<", ";
	cout<<endl;
}

int main(){
	bool print = !true;
	vector<string> words;
	string s;
	map<int, pair<int, int> > wordBoundaries;
	int maxWordLen = 0;
	ifstream inp;

	// inp.open("/usr/share/dict/words");
	inp.open("words_50000.txt");
	while(inp>>s){
		if(isStrAlpha(s)){
			words.push_back(convertStrToLower(s));
			
			if(s.length() > maxWordLen){
				maxWordLen = s.length();
			}
		}
	}
	inp.close();
	sort(words.begin(), words.end(), compare);

	int last = 0;
	for(int i=0;i<words.size()-1;i++){
		if(words[i+1].length() > words[i].length()){
			wordBoundaries[words[i].length()] = pair<int, int>(last, i+1);
			last = i+1;
		}
	}
	wordBoundaries[maxWordLen] = pair<int, int>(last, words.size());

	vector<bool> shouldUse(words.size(), 0);
	int correct = 0, incorrect = 0;

	inp.open("words_50000.txt");
	// inp.open("test.txt");
	while(inp>>s){
		if(!print)
			cout<<correct + incorrect<<endl;
		
		vector<char> missList;
		vector<char> hitList;

		int lenRemaining = s.length();
		string blanks = string(lenRemaining, '_');

		if(print){
			printBlanks(blanks, missList);
			cout<<endl;
		}

		pair<int, int> relevantRange = wordBoundaries[lenRemaining];
		int start = relevantRange.first;
		int end = relevantRange.second;
		
		if(lenRemaining > maxWordLen){
			start = 0;
			end = words.size();
		}
		std::fill(shouldUse.begin() + start, shouldUse.begin() + end, 1);

		while(missList.size() < 6 && lenRemaining > 0){
			char c = getMaxFreqChar(words, shouldUse, start, end, hitList, missList);
			
			if(print)
				cout<<"guess: "<<c<<endl;

			if(charPresentInString(s, c)){
				hitList.push_back(c);
				for(int i=0;i<s.length();i++){
					if(s[i] == c){
						lenRemaining -= 1;
						blanks[i] = c;
					}
				}
			}
			else{
				missList.push_back(c);
			}
			
			if(print)
				printBlanks(blanks, missList);
		}

		if(charPresentInString(blanks, '_')){
			incorrect += 1;
		}
		else{
			correct += 1;
		}

	}

	float accuracy = float(correct)/(correct + incorrect);
	cout<<correct<<" "<<incorrect<<" "<<accuracy<<endl;
}