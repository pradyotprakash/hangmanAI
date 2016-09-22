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
	for(int i=0;i<s.length();i++){
		if(!isalpha(s[i])){
			return false;
		}
	}
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
		if(s[i] == c){
			return true;
		}
	}
	return false;
}

char getMaxFreqChar(vector<string>& words, vector<int>& shouldUse, int start, int end, vector<char>& hitList, vector<char>& missList, bool lastHitOrMiss, int lastHitFreq, string blanks){

	vector<int> freqs(26, 0);
	vector<vector<int> > bigrams(26, vector<int>(26, 0));
	for(int i=start;i<end;i++){

		if(shouldUse[i]){
			
			if(lastHitOrMiss){
				char c = hitList.back();
				int count = lastHitFreq;
				
				for(int j=0;j<words[i].length();j++){
					if(words[i][j] == c){
						count--;
					}
				}

				if(count != 0){
					shouldUse[i] = 0;
					continue;
				}
			}
			else{
				// if condition needed for the first iteration
				if(missList.size() > 0){
					char d = missList.back();

					if(charPresentInString(words[i], d)){
						shouldUse[i] = 0;
						continue;
					}
				}
			}

			// both conditions satisfied, good for calculating frequencies
			string word = words[i];
			for(int j=0;j<word.length();j++){
				if(blanks[j] == '_'){
					int ch = int(word[j])-97;
					freqs[ch]++;

					if(word.length() > 1){
						if(j == 0){
							int chNext = int(word[j+1])-97;
							bigrams[ch][chNext]++;
						}
						else if(j == word.length()-1){
							int chPrev = int(word[j-1])-97;
							bigrams[chPrev][ch]++;
						}
						else{
							int chNext = int(word[j+1])-97;
							bigrams[ch][chNext]++;
							
							int chPrev = int(word[j-1])-97;
							bigrams[chPrev][ch]++;
						}
					}
				}
			}
		}
	}

	for(int i=0;i<hitList.size();i++)
		freqs[int(hitList[i])-97] = -100000000;
	for(int i=0;i<missList.size();i++)
		freqs[int(missList[i])-97] = -100000000;

	int maxFreq = 0;
	vector<int> indicesOfMax;
	char c = 'y';
	for(int i=0;i<26;i++){
		if(freqs[i] > maxFreq){
			maxFreq = freqs[i];
			indicesOfMax.clear();
			indicesOfMax.push_back(i);
			c = char(97+i);
		}
		else if(freqs[i] == maxFreq){
			indicesOfMax.push_back(i);
		}
	}

	if(indicesOfMax.size() > 1 && blanks.length() > 1){
		// multiple values with the same highest frequency occur
		// use bigrams to resolve the tie
		vector<int> bigramCount(indicesOfMax.size(), 0);
		for(int j=0;j<indicesOfMax.size();j++){
			int guessChar = indicesOfMax[j];

			for(int i=0;i<blanks.length();i++){
				if(blanks[i] == '_'){
					if(i == 0){
						if(blanks[i+1] == '_'){
							continue;
						}
						else{
							bigramCount[j] += bigrams[guessChar][int(blanks[i+1])-97];
						}
					}
					else if(i == blanks.length()-1){
						if(blanks[i-1] == '_'){
							continue;
						}
						else{
							bigramCount[j] += bigrams[int(blanks[i-1])-97][guessChar];
						}
					}
					else{
						if(blanks[i+1] == '_'){
							continue;
						}
						else{
							bigramCount[j] += bigrams[guessChar][int(blanks[i+1])-97];
						}

						if(blanks[i-1] == '_'){
							continue;
						}
						else{
							bigramCount[j] += bigrams[int(blanks[i-1])-97][guessChar];
						}
					}
				}	
			}
		}

		int maxIndex = 0, maxVal = 0;
		for(int i=0;i<bigramCount.size();i++){
			if(bigramCount[i] > maxVal){
				maxVal = bigramCount[i];
				maxIndex = i;
			}
		}
		return char(indicesOfMax[maxIndex]+97);
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

	vector<int> shouldUse(words.size(), 0);
	int correct = 0, incorrect = 0;

	if(!print)
		inp.open("words.txt");
	else
		inp.open("test.txt");
	while(inp>>s){
		if(!print){
			int sum = correct + incorrect + 1;
			if(sum % 1000 == 0)
				cout<<sum<<endl;
		}
		
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
		fill(shouldUse.begin() + start, shouldUse.begin() + end, 1);
		bool lastHitOrMiss = false;
		int lastHitFreq;

		while(missList.size() < 6 && lenRemaining > 0){
			char c = getMaxFreqChar(words, shouldUse, start, end, hitList, missList, lastHitOrMiss, lastHitFreq, blanks);
			
			if(print)
				cout<<"guess: "<<c<<endl;

			if(charPresentInString(s, c)){
				lastHitFreq = 0;
				lastHitOrMiss = 1;
				hitList.push_back(c);
				for(int i=0;i<s.length();i++){
					if(s[i] == c){
						lenRemaining--;
						blanks[i] = c;
						lastHitFreq++;
					}
				}
			}
			else{
				lastHitOrMiss = 0;
				missList.push_back(c);
			}
			
			if(print)
				printBlanks(blanks, missList);
		}

		if(charPresentInString(blanks, '_')){
			incorrect += 1;

			// int count = 0;
			// for(int k=0;k<blanks.length();k++){
			// 	if(blanks[k] == '_')
			// 		count++;
			// }
			// cerr<<(correct+incorrect)<<" "<<count<<" "<<blanks.length()<<endl;
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
