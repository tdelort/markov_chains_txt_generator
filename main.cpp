#include <fstream>
#include <iostream>

#include "MarkovChain.h"

std::string sanitize(std::string str)
{
    // remove . , ( ) { } [ ] and put everything in lowercase
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::string toRemove = ".,()[]{}";
    str.erase(std::remove_if(str.begin(), str.end(), [toRemove](char c){ return toRemove.find(c) != std::string::npos; }), str.end());
    return str;
}

int main(int argc, char *argv[]) 
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file>" << std::endl;
        return 1;
    }

    // Opening the input file
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Could not open the input file" << std::endl;
        return 1;
    }

    // Creating the markov chain
    MarkovChain<std::string>* markovChain = new MarkovChain<std::string>();

    // Feeding each word to the chain
    std::string word;
    while (inputFile >> word)
        markovChain->addElement(sanitize(word));

    // Closing the input file
    inputFile.close();

    // Opening the output file
    std::ofstream outputFile(argv[2]);
    if (!outputFile.is_open())
    {
        std::cerr << "Error: Could not open the output file" << std::endl;
        return 1;
    }

    // Debugging the markov chain
    //markovChain->printToDot(outputFile);

    // Generating the output text into the output file
    std::vector<std::string> generated = markovChain->generate(1000);
    for (std::string& word : generated)
        outputFile << word << " ";

    // Closing the output file
    outputFile.close();
}