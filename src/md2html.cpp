#include "md2html.h"

#include <regex>

#include "constants.h"
#include "helper.h"
#include "inlineparse.h"

Md2html::Md2html()
{
    setInputPath("");
    setOutputPath("");
}

Md2html::Md2html(std::string inFilePath, std::string outFilePath)
{
    setInputPath(inFilePath);
    setOutputPath(outFilePath);
}

Md2html::~Md2html()
{
    buffer.push_front("");
    closeAllTags();
    flushBuffer();
    fin.close();
    fout.close();
}

void Md2html::start()
{
    buffer.push_front("");

    while (getNextLine())
    {
        resolveNextLine();
        writeBuffer();
    }

    buffer.push_front("");
    closeAllTags();
    flushBuffer();
}

std::string Md2html::getInputPath() { return inFilePath; }

std::string Md2html::getOutputPath() { return outFilePath; }

std::ifstream *Md2html::getInputStream() { return &fin; }

std::ofstream *Md2html::getOutputStream() { return &fout; }

void Md2html::setInputPath(std::string inFilePath)
{
    inFilePath = inFilePath;
    fin.open(inFilePath);
}

void Md2html::setOutputPath(std::string outFilePath)
{
    outFilePath = outFilePath;
    fout.open(outFilePath);
}

std::deque<std::string> &Md2html::getBuffer() { return buffer; }

bool Md2html::getNextLine()
{
    string line_buf;
    if (getline(fin, line_buf))
    {
        buffer.push_front(line_buf);
        return true;
    }

    return false;
}

void Md2html::writeBuffer()
{
    while (buffer.size() > 2)
    {
        fout << buffer.back() << "\n";
        buffer.pop_back();
    }
}

void Md2html::flushBuffer()
{
    while (buffer.size() > 0)
    {
        fout << buffer.back() << "\n";
        buffer.pop_back();
    }
}

void Md2html::clearBuffer() { buffer.clear(); }

// state management
std::stack<Tag> &Md2html::getState() { return state; }

void Md2html::clearState()
{
    while (state.size() > 0)
    {
        state.pop();
    }
}

bool Md2html::closeLastTag()
{
    if (buffer.size() == 1)
    {
        return false;
    }
    else if (state.size() == 0)
    {
        return false;
    }
    else
    {
        buffer[1] += ct(state.top().type);
        state.pop();
    }

    return true;
}

void Md2html::closeAllTags()
{
    while (closeLastTag())
        ;
}

void Md2html::resolveNextLine()
{
    smatch match;
    string* line = &buffer[0];
    string* lastLine = &buffer[1];

    if (!regex_match(*line, R_CODEBLOCK_FENCED) && !state.empty() && state.top().type == "pre")
    { // <pre> codeblock is active
        // Take this line at face value
    }
    else if (regex_match(*line, R_HEADING))
    { // Heading matched using #'s
        // Close all open tags
        closeAllTags();
        // Count #'s to determine heading level
        regex_search(*line, match, regex("#{1,6}"));
        int heading_level = match.str().length();
        // Write tag
        *line = wrapTags("h" + to_string(heading_level),
                          resolveInline(line->substr(heading_level + 1)));
    }
    else if (regex_match(*line, R_HEADING_ALT))
    { // Alternate heading matched, e.g. === or ---
        // Determine if previous line was heading-compatible
        if (regex_match(*lastLine, regex("<p>.*")))
        { // Parsed as <p> (compatible)
            // Remove <p> state
            state.pop();
            // Strip <p> open tag from previous line
            *lastLine = lastLine->substr(3, lastLine->length());
            // Write first or second level heading
            string tag = (*line)[0]=='='?"h1":"h2";
            *lastLine = wrapTags(tag, lastLine->substr(3, lastLine->length() - 7));
            // Remove line of = or -
            buffer.pop_front();
            // Close all open tags
            closeAllTags();
        }
        else if (!state.empty() && state.top().type == "p")
        { // Paragraph tag already open
            // Continue paragraph
            *lastLine += " " + *line;
            buffer.pop_front();
        }
        else
        { // Line on its own
            // Start a new paragraph
            *line = ot("p") + *line;
            state.push({"p"});
        }
    }
    else if (regex_match(*line, R_ORDERED))
    { // Ordered list matched, e.g. 1. 2. 3. 
        resolveList("ol");
    }
    else if (regex_match(*line, R_UNORDERED))
    { // Unordered list matched, e.g. bullets - * +
        resolveList("ul");
    }
    else if (regex_match(*line, R_CODEBLOCK_TABBED))
    { // Tabbed codelock matched
        if (state.empty() || state.top().type != "pre")
        { // No <pre> codeblock open
            // Open pre tag
            *line = ot("pre") + *line;
            state.push({"pre"});
        }
        else
        { // <pre> codeblock already open
            // Do nothing
        }
    }
    else if (regex_match(*line, R_CODEBLOCK_FENCED))
    { // Fenced codeblock matched, i.e. ``` or ~~~
        // Find language for syntax highlighting support
        string lang = line->substr(3);

        if (state.empty() || state.top().type != "pre")
        { // No <pre> codeblock open
            *line = ot("pre");
            state.push({"pre"});
        }
        else
        { // <pre> codbelock already open
            // Close <pre> tag
            *line = ct("pre");
            state.pop();
        }
    }
    else if (regex_match(*line, R_HTML_TAG))
    { // HTML tag by itself
        // Do nothing
    }
    else if (regex_match(*line, R_HORIZONTAL_RULE))
    { // Horizontal rule matched, i.e. *** --- ===
        *line = "<hr>";
    }
    else if (regex_match(*line, regex("\\s*")))
    { // Blank line matched
        // Close all tags
        closeAllTags();
    }
    else
    { // Line by itself with no particular syntax
        if (!state.empty())
        { // Tags still open
            // Append this line to previous line
            *lastLine += " " + resolveInline(*line);
            buffer.pop_front();
        }
        else
        { // No tags open (state empty)
            // Start new paragraph
            *line = ot("p") + resolveInline(*line);
            state.push({"p"});
        }
    }
}

void Md2html::resolveList(string thisType)
{
    smatch match;
    string* line = &buffer[0];
    string* lastLine = &buffer[1];

    if (!state.empty() && state.top().type == "li")
    {
        // Open <li>
        *lastLine += ct("li");
        state.pop();
    }

    // Get indentation size
    if (thisType == "ol")
    {
        regex_search(*line, match, regex("\\d*\\. "));
    }
    else
    {
        regex_search(*line, match, regex("(-|\\*|\\+) "));
    }

    int thisIndent = match.position();
    // Strip bullet or digits
    *line = line->substr(match.position() + match.str().length());
    // Set opposite list type (ordered/unordered)
    string oppType = "ul";
    if (thisType == "ul")
        oppType = "ol";

    // If state empty, create a new list
    if (state.empty())
    {
        state.push({thisType, thisIndent});
        *line = ot(thisType) + ot("li") + resolveInline(*line);
        state.push({"li"});
    }
    // Otherwise, run through cases
    else
    {
        Tag top = state.top();

        // Start a new list if:
        // top isn't a list, or is a list but less indented than this line
        if (!isList(top) || (isList(top) && top.data < thisIndent))
        {
            state.push({thisType, thisIndent});
            *line = ot(thisType) + ot("li") + resolveInline(*line);
            state.push({"li"});
        }
        // Continue a list if:
        // top is the same type and indent as this line
        else if (top.type == thisType && top.data == thisIndent)
        {
            *line = ot("li") + resolveInline(*line);
            state.push({"li"});
        }
        // Restart list of different type if:
        // top has same indentation but different type
        else if (top.data == thisIndent && top.type != thisType)
        {
            state.pop();
            state.push({thisType, thisIndent});
            *lastLine += ct(oppType);
            *line = ot(thisType) + ot("li") + resolveInline(*line);
            state.push({"li"});
        }
        // Terminate nested lists if:
        // top indentation is greater than that of this line
        else if (top.data > thisIndent)
        {
            string lastType = thisType;
            // Terminate all nested lists with indentation
            while (!state.empty() && isList(state.top()) &&
                   state.top().data > thisIndent)
            {
                *lastLine += ct(state.top().type);
                lastType = state.top().type;
                state.pop();
            }

            // If somebody doesn't adhere to proper indentation,
            if (state.empty() || !isList(state.top()) ||
                state.top().data < thisIndent)
            {
                // Trim last close tag
                *lastLine = lastLine->substr(0, lastLine->length() - 5);
                // Re-open list
                state.push({lastType, thisIndent});
                // Add this line to that list
                *line = ot("li") + resolveInline(*line);
                state.push({"li"});
            }
            else if (state.top().data == thisIndent)
            {
                *line = ot("li") + resolveInline(*line);
                state.push({"li"});
            }
            else
            {
                cout << "Something went terribly wrong with list termination."
                     << endl;
            }
        }
    }
}