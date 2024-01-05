import random
from selenium import webdriver
from nltk.corpus import words

# Download the NLTK words dataset using nltk.download() as you described.
# Ensure that NLTK and Selenium are installed, too.

# Get the list of English words from NLTK's corpus.
word_list = words.words()

# Create a new instance of the Edge web browser.
driver = webdriver.Edge()

# Open the Bing website.
driver.get("https://www.bing.com/")

# Find the text input field using the provided XPath.
input_element = driver.find_element_by_xpath("//*[@id='sb_form_q']")

# Choose a random word from the NLTK word list.
random_word = random.choice(word_list)

# Enter the random word into the text input field.
input_element.send_keys(random_word)

# Simulate pressing "Enter."
input_element.submit()

# Close the browser when done.
driver.quit()
