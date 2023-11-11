# Introduction to Robotics (2023 - 2024)

Welcome to the _Introduction to Robotics_ course repository for the academic year 2023-2024. This repository serves as a centralized hub for all the course materials, assignments, and projects. It is aimed at students of the Faculty of Mathematics and Computer Science at the University of Bucharest who are taking the _Introduction to Robotics_ course. 
Here you will find information on various homework assignments, along with the necessary requirements, implementation details, and resources.

## Contact: robert-mihai.gheorghe@s.unibuc.ro

### Homework 1:
<details>
<summary></summary>
  
### Components
1. ${\color{red}R}{\color{green}G}{\color{blue}B}$ led (1+)
2. Potentiometers (3+)
3. Resistors
4. Wires

### Task
Control each channel of an ${\color{red}R}{\color{green}G}{\color{blue}B}$ led using a separate potentiometer. The intensity must be controlled through software, using Arduino. (i.e read the values from the potentiometer and send analog signals to the LED)

### Setup
<details>
<summary></summary>

![Setup](https://github.com/surtexx/IntroductionToRobotics/blob/main/homework1/setup1.jpg)
</details>

### [Demo](https://www.youtube.com/watch?v=D3T7p-lxy4M)
### [Source code](./homework1/homework1.ino)

</details>

### Homework 2:
<details>
<summary></summary>
  
### Components
1. LEDs (4+)
2. Buttons (3+)
3. Buzzer (1)
4. Wires and Resistors

### Task
Simulate a working elevator using buttons and LEDs. Each pair LED-button is associated with a floor, starting from 0. When pressing a button, the buzzer with emit a sound depicting that the elevator _is going up or down_, and the extra LED will blink until the destination is reached. When the elevator _is going up or down_, the LED of each floor will be lit for a small amount of time, depicting that _the elevator is currently at that floor_, and then proceed to the next floor, when the last LED will turn off, for the present LED to light up.

### Setup
<details>
<summary></summary>

![Setup](https://github.com/surtexx/IntroductionToRobotics/blob/main/homework2/setup2.jpg)
</details>

### [Demo](https://www.youtube.com/watch?v=kEbVnQoMF-4)
### [Source code](./homework2/homework2.ino)

</details>

### Homework 3:
<details>
<summary></summary>
  
### Components
1. 7 segment display (1)
2. Joystick (1)
3. Wires and Resistors

### Task
Using the joystick, control the 7 segment display. Starting from the decimal point (DP), there are 4 possible directions: up, down, left, right. Each direction will select the neighbour segment, making it blink. If the joystick button is pressed, the current segment will stay lit (will keep blinking if selected). There can be more segments lit at once, and they also can be turned off. If the joystick button is held for more than 3 seconds, all the segments will be turned off, and the DP will be selected. (starting position)

### Setup
<details>
<summary></summary>

![Setup](https://github.com/surtexx/IntroductionToRobotics/blob/main/homework3/setup3.png)
</details>

### [Demo](https://www.youtube.com/watch?v=8gTAwDh5y10)
### [Source code](./homework3/homework3.ino)

</details>

### Homework 4:
<details>
<summary></summary>
  
### Components
1. 4 digits 7 segment display (1)
2. Joystick (1)
3. Buttons (2)
4. Wires and Resistors

### Task
Implement a timer that has capabilities of saving laps. The timer starts from 000.0, and can be started, paused, and reset. It can be started or paused by pressing the start button (left of the joystick). Reset is done by pressing the reset button (right of the joystick) when it is in pause mode, removing all saved laps. The timer can save laps by pressing the joystick button. The joystick can be used in pause mode to cycle between saved laps and current time. Pressing the joystick button will clear all the laps and restore the timer to the time when it was paused.

### Setup
<details>
<summary></summary>

![Setup](https://github.com/surtexx/IntroductionToRobotics/blob/main/homework4/setup4.png)
</details>

### [Demo](https://www.youtube.com/watch?v=T57Q6Mao5Qg)
### [Source code](./homework4/homework4.ino)

</details>