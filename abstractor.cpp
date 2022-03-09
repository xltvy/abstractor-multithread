// Author: Altay Acar
// Student ID: 201840

#include <iostream>
#include <pthread.h>
#include <thread>
#include <list>
#include <vector>
#include <sstream>
#include <fstream>
#include <set>
#include <algorithm>
#include <iomanip> 
#include <mutex>

using namespace std;

ifstream read;	//reads the input file
ofstream write;	//writes to the output file

int total_threads;	//stores the value of total thrads to be created
int total_abstracts;	//stores the value of total abstracts to be read
int total_results;	//stores the value total results to be shown in the output file

list<string> lines;   //stores every line of the input
vector<string> query;	//stores the keywords to be searched in abstracts

/* custom structs to store results of operations done to abstract files */
struct abstraction {
	string file_name;
	double score;
	string summary;
};

mutex mut;

vector<abstraction> abstraction_results;	//list of resulting abstraction structs

/*	method that will be called by threads.
	-> finds which txt files to be abstracted 
	-> processes each abstract txt file
	-> calculates the similarity score between the keywords and the text inside
	-> identifies the sentences that contain any given keyword
	-> creates an abstraction struct to store the results and stores it inside abstraction_results 
	parameters:
	-> thread_jobs: vector that contain the vector of txt files to be abstracted by each thread
	-> index: indec of the thread */
void abstract(vector<vector<string>> thread_jobs, int index) {

	struct abstraction a1;	//stores the findings of abstracting operation
	/* iterates over each abstract txt file assigned to the thread for abstraction */
	for (int i=0; i<thread_jobs[index].size(); i++) {

		//name or path of the txt file saved to resulting abstraction struct
		a1.file_name = thread_jobs[index][i];

		string filestring;	//txt file's text as one string
		string buffer;
		//opens the txt file to be abstracted
		stringstream s(thread_jobs[index][i]);
		string file_path = "";	//path of the txt file
		s >> file_path;
		ifstream abs_file;
		abs_file.open(file_path);

		//mutex lock for writing to output file -> critical region
		mut.lock();
		//writes which thread performs abstraction operation to which txt file to the output file
		write << "Thread " << char(65+index) << " is calculating " << file_path << endl;
		mut.unlock();

		//creates a set that will contain each unique word in the txt file, including "."
		set<string> abs;
		string t = "";
   		while (abs_file >> t) {
   			abs.insert(t);
   		}

   		//closes and reopens the file to reprocess it line by line
   		abs_file.close();
   		abs_file.open(file_path);
   		//txt file's text as one string
		while (getline(abs_file, buffer)) {
			filestring += (buffer + '\n');
		}
		/*	calculates the similarity score between the keywords and txt file's text
			-> in_set stores the data of how many keywords found in the given text
			-> not_in_set stores the data of how many keywords not found in the given text
			-> calculates the score according to (intersection and union being set operations:
			-> keywords(intersection)text
			   --------------------------
			      keywords(union)text
			 */
   		int in_set = 0;
   		int not_in_set = 0;
   		for (int i=0; i<query.size(); i++) {
   			stringstream k(query[i]);
			string key = "";
			k >> key;
   			if (abs.count(key)) {
   				in_set++;
   			} else {
   				not_in_set++;
   			}
   		}
   		double score = (double) in_set/(abs.size()+not_in_set);	//similarity score
   		a1.score = score;	//stores the resulting score in the resulting abstract struct's score field

   		//reinitializes the cursor in the file to the beginning of the file
   		abs_file.seekg(0, abs_file.beg);

   		vector<string> sentences;	//stores each sentence of the txt file's text
   		//splits the text into sentences and pushes them into sentences vector
   		stringstream tokenize2(filestring);
   		string temp_token2;
   		while (getline(tokenize2, temp_token2, '.')) {
   			temp_token2.erase(remove(temp_token2.begin(), temp_token2.end(), '\n'), temp_token2.end());
   			sentences.push_back(temp_token2);
   		}

   		string summary;	//resulting summary of the abstract
   		/* pushes each senttence that contain any given keyword into summary */
   		for (int i=0; i<sentences.size(); i++) {
   			stringstream s1(sentences.at(i));
			string word = "";
			set<string> sent;
			/* sentence is stores word by word into set sent for exact search */
			while (s1 >> word) {
				sent.insert(word);
			}
   			for (int j=0; j<query.size(); j++) {
				if (sent.count(query.at(j))) {
					if (summary.find(sentences.at(i)) == string::npos) {
						summary += (sentences.at(i) + ".");
					}
   				}
   			}
   		}

   		/*	formats the summary string according to given project description
			by deleting the initial additional " " of the sentences that are
			not the opening sentence of the abstract.
			then stores it into the resulting abstraction struct's summary field. */
   		if (summary != "") {
   			if (summary.at(0) == ' ') {
   				string summary_final = summary.substr(1, summary.length());
   				a1.summary = summary_final;
   			} else {
   				a1.summary = summary;
   			}
   		} else {
   			a1.summary = "";
   		}
   		//mutex lock for adding the result to global variable list -> critical region
   		lock_guard<mutex> guard(mut);
   		//pushes the final result abstraction result to the abstraction_results vector
   		abstraction_results.push_back(a1);

   		abs_file.close();	//closes the opened txt file
	}

}

/*	gets an element of a list in a specified index
	parameters:
	-> _list: list that the element to be returned from
	-> a: index of the element in the _list */
string get(list<string> _list, int a) {
	list<string>::iterator itr = _list.begin();
	for(int i=0; i<a; i++) {
		++itr;
	}
	return *itr;
}

/*	extracts integers of a string full of integers and returns a vector that contains them
	parameters:
	-> str: string that contains integers */
vector<int> extract_integers(string str) {
	vector<int> results;
    stringstream ss;
    ss << str;
    string temp;
    int found;
    while (!ss.eof()) {
        ss >> temp;
        if (stringstream(temp) >> found)
            results.push_back(found);
        temp = "";
    }
    return results;
}

/* compares an abstraction struct by its score field */
bool compare_score(const abstraction &a1, const abstraction &a2) {
	return a1.score > a2.score;
}

/*  */
int main(int argc, char const *argv[]) {
	
	read.open(argv[1]);
    write.open(argv[2]);
    FILE *input = fopen(argv[1], "r");

    char line[999];
    //Reads the input file line by line and adds every line to the list "lines"
    while(fgets(line, sizeof(line), input)) {
        lines.push_back(line);
    }

    string first_line = lines.front();
    vector<int> initials = extract_integers(first_line);

    total_threads = initials[0];	//first integer of the file = T
    total_abstracts = initials[1];	//second integer of the file = A
    total_results = initials[2];	//third integer of the file = N

    lines.pop_front();

    /* keywords stored in the vector query */
    string second_line = lines.front();
    stringstream tokenize(second_line);
    string temp_token;
    while (getline(tokenize, temp_token, ' ')) {
    	temp_token.erase(remove(temp_token.begin(), temp_token.end(), '\n'), temp_token.end());
    	query.push_back(temp_token);
    }

    lines.pop_front();

    /* 	splits each work evenly to threads and stores each thread's assigned
		abstracts into a vector and stores each vector into thread_jobs vector. */
    vector<vector<string>> thread_jobs;

    for (int i=0; i<total_threads; i++) {
    	vector<string> jobs;
    	thread_jobs.push_back(jobs);
    }

    for (int i=0; i<total_abstracts; i++) {
    	int index = i % total_threads;
    	thread_jobs[index].push_back(get(lines, i));
    }

    /* creates each thread and waits for their completion */
    thread threads[total_threads];
    for (int i=0; i<total_threads; i++) {
    	threads[i] = thread(abstract, thread_jobs, i);
    }
    for (int i=0; i<total_threads; i++) {
    	threads[i].join();
    }
    /* sorts each abstraction result according their descending scores */
    sort(abstraction_results.begin(), abstraction_results.end(), compare_score);	//sorts the abstraction structs according to their score field

    write << "###" << endl;	//initial separator of the output text from the inof of which thread operates on which abstract

    /* prints the results in output text file according to the given description */
    for (int i=0; i<total_results; i++) {
    	write << "Result " << i+1 << ":" << endl;
    	stringstream ss(abstraction_results[i].file_name);
		string file_path = "";	//path of the txt file
		ss >> file_path;
    	write << "File: " << file_path << endl;
    	char buff[100];
  		snprintf(buff, sizeof(buff), "%.4f", abstraction_results[i].score);
  		string score_str= buff;
    	write << "Score: " << score_str << endl;
    	write << "Summary: " << abstraction_results[i].summary << endl;
    	write << "###" << endl;
    }

    return 0;
}