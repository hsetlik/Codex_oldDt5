#ifndef STLUTILITY_H
#define STLUTILITY_H

#include <regex>
#include <map>
#include <unordered_map>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

struct stdu
{
public:
    static std::vector<QString> matchesAsVector(QString body, std::regex reg)
        {
            std::vector<std::string> strings;
            auto sBody = body.toStdString();
            auto results = std::smatch{};
            for (std::sregex_iterator it = std::sregex_iterator(sBody.begin(), sBody.end(), reg);
                     it != std::sregex_iterator(); it++)
            {
                std::smatch match;
                match = *it;
                auto str = match.str(0);
                strings.push_back(str);
            }
            std::vector<QString> output;
            for(auto& s : strings)
            {
                QString qS = s.c_str();
                output.push_back(qS);
            }
            return output;
        }
};

struct RegexUtil
{
    static std::vector<QString> regexMatches(QString body, QRegExp exp)
    {
        qDebug() << "Full Phrase is: " << body;
        std::vector<QString> output;
        auto pos = exp.indexIn(body);
        if(pos == -1)
            return output;
        auto matches = exp.capturedTexts();
        for(int i = 1; 1 < matches.size(); ++i)
        {
            output.push_back(matches[i]);
            qDebug() << "Word is: " << matches[i];
        }
        return output;
    }
};

#endif // STLUTILITY_H
