import pyttsx3
import PyPDF2

book = open('demo.pdf', 'rb')
pdf_reader = PyPDF2.PdfFileReader(book)
num_pages = pdf_reader.numPages
print(num_pages)

engine = pyttsx3.init()

rate = engine.getProperty('rate')
print(rate)
engine.setProperty('rate', 150)

volume = engine.getProperty('volume')
print(volume)
engine.setProperty('volume', 1.0)

voices = engine.getProperty('voices')
print(voices)
engine.setProperty('voice',voices[1].id)



for num in range(0, num_pages):
    page = pdf_reader.getPage(num)
    data = page.extractText() 
    print(data)
    
    #engine.say(data)
    engine.save_to_file(data, 'test.mp3')
    engine.runAndWait()
    engine.stop()
