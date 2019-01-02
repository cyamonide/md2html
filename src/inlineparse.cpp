#include "inlineparse.h"

#include "constants.h"
#include "helper.h"

#include <iostream>
#include <regex>
#include <string>

using namespace std;

string resolveCode(string line)
{
    line = line.substr(1, line.length() - 2);
    return wrapTags("code", line);
}

// Requires line be a link and link only
string resolveLink(string line)
{
    // Add Title
    if (regex_match(line, R_LINK_TITLE))
    {
        string href = line.substr(line.find("(") + 1);
        string title = href;

        href = href.substr(0, href.find(" "));

        title = title.substr(title.find(" ") + 1);
        title = title.substr(0, title.length() - 1);

        string disp = line.substr(1, line.find("]") - 1);

        return "<a href=\"" + href + "\" title=" + title + ">" + disp + "</a>";
    }

    string href = line.substr(line.find("(") + 1);
    href = href.substr(0, href.length() - 1);
    string disp = line.substr(1, line.find("]") - 1);

    // line.replace(0, line.length(), "<a href=\"" + href + "\">" + disp +
    // "</a>");

    return "<a href=\"" + href + "\">" + disp + "</a>";
}

string resolveTriple(string line)
{
    line = line.substr(3, line.length() - 6);
    return wrapTags("em", wrapTags("strong", resolveInline(line)));
}

string resolveBold(string line)
{
    line = line.substr(2, line.length() - 4);
    return wrapTags("strong", resolveInline(line));
}

string resolveItalics(string line)
{
    line = line.substr(1, line.length() - 2);
    return wrapTags("em", resolveInline(line));
}

string resolveInline(string line)
{
    // Double space line break
    if (regex_match(line, regex(".*[^ ]  ")))
        return resolveInline(line.substr(0, line.length() - 2)) + "<br>";

    // Regex obj
    smatch m;

    pair<int, string> mFirst;
    mFirst.first = line.length();
    mFirst.second = "";
    string mType = "";

    // Resolve inline code
    if (regex_search(line, m, R_CODE))
    {
        if (m.position() < mFirst.first)
        {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "code";
        }
    }
    // Resolve links
    if (regex_search(line, m, R_LINK))
    {
        if (m.position() < mFirst.first)
        {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "link";
        }
    }
    // Resolve triple asterisk/italics
    if (regex_search(line, m, R_TRIPLE_A) || regex_search(line, m, R_TRIPLE_U))
    {
        if (m.position() < mFirst.first)
        {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "triple";
        }
    }
    // Resolve bold
    if (regex_search(line, m, R_BOLD_A) || regex_search(line, m, R_BOLD_U))
    {
        if (m.position() < mFirst.first)
        {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "bold";
        }
    }
    // Resolve italics
    if (regex_search(line, m, R_ITALICS_A) ||
        regex_search(line, m, R_ITALICS_U))
    {
        if (m.position() < mFirst.first)
        {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "italics";
        }
    }

    if (mType == "")
        return line;

    string retval = resolveInline(line.substr(0, mFirst.first));
    string content = line.substr(mFirst.first, mFirst.second.length());

    if (mType == "code")
    {
        retval += resolveCode(content);
    }
    else if (mType == "link")
    {
        retval += resolveLink(content);
    }
    else if (mType == "triple")
    {
        retval += resolveTriple(content);
    }
    else if (mType == "bold")
    {
        retval += resolveBold(content);
    }
    else if (mType == "italics")
    {
        retval += resolveItalics(content);
    }

    retval += resolveInline(line.substr(mFirst.first + mFirst.second.length()));

    return retval;
}
