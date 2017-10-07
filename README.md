##Logic

#Problem 1

1. I made separate threads for cars and attenders.
2. If the number of cars is less than 7, it creates a thread and makes a call to enterStation. Else it waits for a signal that the queue is empty. Once the queue is empty, it calls waitInLine.
3. Then, in the function for the car thread, it makes a call to pickPump, which returns false if all three pumps are occupied and returns true otherwise.
4. If pickPump returns false, it calls waitInLine.
5. pickPump calls goToPump if it finds an empty Pump. It signals the attender thread who was dreaming that it has come to the gas pump.
6. The attender thread services the car and signals back that service is done.
7. The car pays and signals to the attender thread to accept Payment.
8. The attender thread accepts payment and signals that the car can leave now.
9. The car leaves, creating a vacancy at the gas pump. 
10. The car at the head of the queue goes to this gas pump. If there are any cars outside the gas station, the first one comes in.