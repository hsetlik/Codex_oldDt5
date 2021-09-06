import sys
from gtts import gTTS
import pyttsx3
import urllib.request
from playsound import playsound

# here, we need to check for an internet connection to
# determine whether we will use gTTS (online, more realistic)
# or pyttsx3 (offline)


def has_connection(host='http://google.com'):
    try:
        urllib.request(host)
        return True
    except:
        return False


# given a pyttsx3 engine and a language string, this sets the engine's voice to
# one that can handle the language
def set_pytts_voice(engine, lang):
    voices = engine.getProperty('voices')
    for voice in voices:
        print(voice.name)
        for language in voice.languages:
            print('Language is: ' + language)
            if lang == language:
                engine.setProperty('voice', voice.id)
                break


def speak(text, language='en'):
    engine = pyttsx3.init()
    set_pytts_voice(engine, language)
    engine.say(text)
    engine.runAndWait()
