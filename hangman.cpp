#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <ctime>
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

bool charInVec(vector<char>& v, char c){
	for(int i=0;i<v.size();i++){
		if(v[i] == c)
			return false;
	}
	return true;
}

char getMaxFreqChar(vector<string>& words, vector<bool>& shouldUse, int start, int end, vector<char>& hitList, vector<char>& missList, vector<bool> &indicesOfHit){

	vector<int> freqs(26, 0);
	for(int i=start;i<end;i++){

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
				if(!indicesOfHit[j])
					freqs[int(words[i][j])-97] += 1;
			}
		}
	}

	int maxFreq = 0;
	char c = 'y';
	bool gotIn = false;
	for(int i=0;i<26;i++){
		if(freqs[i] > maxFreq){
			gotIn = true;
			maxFreq = freqs[i];
			c = char(97+i);
		}
	}
	
	if(!gotIn){
		vector<pair<int, char> > f;
		for(int i=0;i<26;i++)
			f.push_back(pair<int, char>(0, char(i+97)));

		for(int i=start;i<end;i++){
			for(int j=0;j<words[i].length();j++){
				f[int(words[i][j])-97].first += 1;
			}
		}

		sort(f.begin(), f.end());
		for(int i=0;i<26;i++){
			if(!charInVec(missList, f[i].second) && !charInVec(hitList, f[i].second)){
				return f[i].second;
			}
		}
	}
	
	
	return c;
	
}

void printBlanks(string blanks, vector<char>& missList){
	cout<<blanks<<" missed: ";
	for(int i=0;i<missList.size();i++)
		cout<<missList[i]<<", ";
	cout<<endl;
}

int main(){
	var = 0;
	clock_t tStart = clock();

	bool print = !true;
	vector<string> words;
	string s;
	map<int, pair<int, int> > wordBoundaries;
	int maxWordLen = 0;
	ifstream inp;

	inp.open("words.txt");
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

	if(!print)
		inp.open("words.txt");
	else
		inp.open("test.txt");
	while(inp>>s){
		if(!print)
			cout<<correct + incorrect<<endl;
		
		vector<char> missList;
		vector<char> hitList;

		int lenRemaining = s.length();
		string blanks = string(lenRemaining, '_');
		vector<bool> indicesOfHit(lenRemaining, false);
				
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
		fill(shouldUse.begin() + start, shouldUse.begin() + end, 1);

		while(missList.size() < 6 && lenRemaining > 0){
			char c = getMaxFreqChar(words, shouldUse, start, end, hitList, missList, indicesOfHit);
			
			if(print)
				cout<<"guess: "<<c<<endl;

			if(charPresentInString(s, c)){
				hitList.push_back(c);
				for(int i=0;i<s.length();i++){
					if(s[i] == c){
						lenRemaining -= 1;
						blanks[i] = c;
						indicesOfHit[i] = 1;
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

			int count = 0;
			for(int k=0;k<blanks.length();k++){
				if(blanks[k] == '_')
					count++;
			}
			cerr<<(correct+incorrect)<<" "<<count<<" "<<blanks.length()<<endl;
		}
		else{
			correct += 1;
		}

	}

	double time = (double)(clock() - tStart)/CLOCKS_PER_SEC;
	double accuracy = 100*float(correct)/(correct + incorrect);
	cout<<endl;
	cout<<"Number of words tested: "<<(correct + incorrect)<<endl;
	cout<<"Number of words guessed correctly: "<< correct<<endl;
	cout<<"Correct guesses (%): "<<accuracy<<"%"<<endl;
	cout<<"Time to run (s): "<<time<<"s"<<endl;
}
