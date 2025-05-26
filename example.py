import random
import time


class HelloWorld:
    def __init__(self, name):
        self.name = name

    def say_hi(self):
        print(f'Hi {self.name}')

    def add(self, a, b):
        return a + b

    def random_number(self):
        time.sleep(0.05)
        return random.randint(0, 100)
