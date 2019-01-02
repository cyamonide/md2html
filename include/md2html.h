#ifndef MD2HTML_MD2HTML_H_
#define MD2HTML_MD2HTML_H_

#include <deque>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>

struct Tag
{
    std::string type = "";
    int data = 0;
};

class Md2html
{
private:
    std::deque<std::string> buffer;
    std::stack<Tag> state;
    std::string inFilePath;
    std::string outFilePath;
    std::ifstream fin;
    std::ofstream fout;

public:
    // constructor destructor
    Md2html();
    Md2html(std::string inFilePath, std::string outFilePath);
    ~Md2html();

    // start conversion process
    void start();

    // fstream management
    std::string getInputPath();
    std::string getOutputPath();
    std::ifstream *getInputStream();
    std::ofstream *getOutputStream();
    void setInputPath(std::string inFilePath);
    void setOutputPath(std::string outFilePath);

    // buffer management
    std::deque<std::string> &getBuffer();
    bool getNextLine();
    void writeBuffer();
    void flushBuffer();
    void clearBuffer();

    // state management
    std::stack<Tag> &getState();
    void clearState();

    // buffer and state operations
    bool closeLastTag();
    void closeAllTags();

    // parse operations
    void resolveNextLine();
    void resolveList(std::string thisType);
};

#endif