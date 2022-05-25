# microwave-simulator

## **Components**
- Tiva C TM4C123
- 16 * 2 LCD
- KeyPad
- ON / OFF switch 

# States of the porgram 
- In the **main State** of the program we choose operation and select the next state based on that.
    - {A: Popcorn, B:Beef, C: Chicken, D: User defined time}

- Once we are in the next state :
    - **Pressing** sw2 –switch 2- **while** the door is closer :
        - **starts** the cooking {moves to another state}.

- After reaching the cooking state :
    - **Waiting** until the cooking is finished will take us to the **main state** to select another operation.  
    - **Pressing** sw1 –switch 1- for one time **while** the door is **closed** :
        - Will pause the cooking
    - **Pressing** sw1 –switch 1- for one time then sw2 –switch 1- **while** the door is **closed**: 
        - Will  **resume** the cooking
    - **Pressing** sw2 –switch 2-  two times :
        - Will take us to the **main state** the main state to select another operation 
    - **Opening** the door –latch 3- :
        - will **pause** the operation until it is **closed**.
    - **Pressing** sw1 –switch 1- for one time then sw2 –switch 1- **while** the door is **open**: 
        -  Will  keep the operation **paused** until it is **closed**