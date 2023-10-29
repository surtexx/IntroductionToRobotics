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