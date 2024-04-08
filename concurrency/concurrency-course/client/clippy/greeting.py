import datetime
import random


def pick():
    def time_based_greeting():
        now = datetime.datetime.now()
        if now.hour < 6:
            return "Good night, {}!"
        elif now.hour < 12:
            return "Good morning, {}!"
        elif now.hour < 18:
            return "Hello, {}!"
        else:
            return "Good evening, {}!"

    def random_greeting():
        candidates = ["Hi, {}!", "What's up, {}?", "Howdy, {}!"]
        return random.choice(candidates)

    if random.random() < 0.3:
        return random_greeting()
    else:
        return time_based_greeting()
