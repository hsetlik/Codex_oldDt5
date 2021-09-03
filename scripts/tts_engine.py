from gtts import gTTS
import pyttsx3
import urllib.request

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
        for language in voice.languages:
            if lang == language:
                engine.setProperty('voice', voice.id)
                return


def speak(text, language):
    connected = has_connection()
    if connected:
        # todo: set up an algorithm to randomize parameters
        g_output = gTTS(text, lang=language)
    else:
        engine = pyttsx3.init()
        set_pytts_voice(engine, language)
        # either figure out a way to give direct audio to gTTS or a way to
        # write to an output with pytts
        engine.say(text)
