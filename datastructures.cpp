#include "datastructures.h"
PhrasePair::PhrasePair(QString native, QString target) :
    nativePhrase(native, this),
    targetPhrase(target, this),
    includeFull(false),
    totalNumCards(0)
{

}

PhrasePair::PhrasePair(const PhrasePair& other) :
    nativePhrase(other.nativePhrase.fullPhrase, this),
    targetPhrase(other.targetPhrase.fullPhrase, this)
{
}

PhrasePair::PhrasePair(QJsonObject& obj) :
    nativePhrase(obj["NativePhrase"].toString(), this),
    targetPhrase(obj["TargetPhrase"].toString(), this)
{
    auto ntaArray = obj["NtaCards"].toArray();
    for(auto card : ntaArray)
    {
        auto cardObject = card.toObject();
        auto sNative = cardObject["NativeWord"].toString();
        auto sTarget = cardObject["TargetWord"].toString();
        addNtaPair(sNative, sTarget);
    }
    auto clozeArray = obj["ClozeCards"].toArray();
    for(auto card : clozeArray)
    {
        auto cardObject = card.toObject();
        auto sCloze = cardObject["ClozeWord"].toString();
    }
}
void PhrasePair::addNtaPair(QString nativeString, QString targetString)
{
    //printf("Adding pair: %s, %s\n", nativeString, targetString);
    ntaPairs[nativeString] = targetString;
    totalNumCards++;
    pingListeners();
}

void PhrasePair::removeNtaPairByNative(QString nativeWord)
{
   for(auto it = ntaPairs.begin(); it != ntaPairs.end(); it++)
   {
       if(it->first == nativeWord)
       {
           ntaPairs.erase(it);
           totalNumCards--;
           pingListeners();
           return;
       }
   }

}
void PhrasePair::removeNtaPairByTarget(QString targetWord)
{
   for(auto it = ntaPairs.begin(); it != ntaPairs.end(); it++)
   {
       if(it->second == targetWord)
       {
           ntaPairs.erase(it);
           totalNumCards--;
           pingListeners();
           return;
       }
   }
}
void PhrasePair::removeCloze(QString word)
{
    for(auto it = clozeWords.begin(); it != clozeWords.end(); it++)
    {
        if(*it == word)
        {
            totalNumCards--;
            clozeWords.erase(it);
            pingListeners();
            return;
        }
    }
}
bool PhrasePair::hasCloze(QString word)
{
    for(auto& cWord : clozeWords)
    {
        if(word == cWord)
            return true;
    }
    return false;
}
void PhrasePair::toggleIncludeFull()
{
    includeFull = !includeFull;
    pingListeners();
}
//make sure this always gets called at the end of functions which edit card/pairing information
void PhrasePair::pingListeners()
{
    for(auto list : listeners)
        list->phraseChanged(this);
}

QString PhrasePair::getJsonIdString()
{
     return nativePhrase.fullPhrase + targetPhrase.fullPhrase;
}
