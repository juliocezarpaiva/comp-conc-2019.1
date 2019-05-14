from threading import Thread, Condition
import time
import random

queue = []
condition = Condition()
MAX_NUM = 5
nums = [1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025]
ids = [1,2]
class ProducerThread(Thread):
    def run(self):
        end = False
        global queue
        while not end:
            condition.acquire()
            if len(queue) == MAX_NUM:
                print("Ainda não foi tudo consumido")
                condition.wait()
                print("consumidor avisou")
            if nums: 
                num = random.choice(nums)
                print(nums)
                nums.remove(num)
                queue.append(num)
                print("Produzido", num)
            if not nums and not queue:
                print("Todos os números foram produzidos")
                end = True
            condition.notify()
            condition.release()
            time.sleep(random.random())


class ConsumerThread(Thread):
    
    
    @staticmethod
    def isprime(x):
        for i in range(2, x-1):
            if x % i == 0:
                return print('nao e primo')
        else:
                return print('e primo')
                
    def run(self):
        myId = random.choice(ids)
        ids.remove(myId)
        global queue
        fim = False
        while not fim:
            condition.acquire()
            if not queue:
                print("Consumidor tentou consumir, mas não há produto")
                condition.wait()
                print("Produtor produziu e avisou ao consumidor")
            if queue: 
                num = queue.pop(0)
                print("Consumido " + str(num) + " por consumidor de id " + str(myId))
                self.isprime(num)
            if not queue and not nums:
                print("Tudo consumido, tudo produzido")
                fim = True
            condition.notify()
            condition.release()
            time.sleep(random.random())
            
    


ProducerThread().start()
ConsumerThread().start()
ConsumerThread().start()
