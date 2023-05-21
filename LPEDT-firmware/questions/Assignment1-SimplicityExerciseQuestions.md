Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

**1. How much current does the system draw (instantaneous measurement) when a single LED is on with the GPIO pin set to StrongAlternateStrong?**
   Answer: The average current is 4.92mA when a single LED is ON with GPIO set to StrongAlternateStrong and within the range of 5.77mA-4.71mA.


**2. How much current does the system draw (instantaneous measurement) when a single LED is on with the GPIO pin set to WeakAlternateWeak?**
   Answer:The average current is 4.91mA when a single LED is ON with GPIO set to WeakAlternateWeak and within the range of 5.45mA-4.39mA.


**3. Is there a meaningful difference in current between the answers for question 1 and 2? Please explain your answer, 
referencing the [Mainboard Schematic](https://www.silabs.com/documents/public/schematic-files/WSTK-Main-BRD4001A-A01-schematic.pdf) and [AEM Accuracy](https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf) section of the user's guide where appropriate. Extra credit is avilable for this question and depends on your answer.**
   Answer: Drive strength effects the slew rate, the rate of change in potential difference, but doesnot efect the static current current.
   		   Hence, we find almost no difference between the average currents from StrongAlternateStrong and WeakAlternateWeak measurements.
		   (Understanding for the lecture 04)

**4. With the WeakAlternateWeak drive strength setting, what is the average current for 1 complete on-off cycle for 1 LED with an on-off duty cycle of 50% (approximately 1 sec on, 1 sec off)?**
   Answer: 4.91mA is the average current for one complete on-off cycle when only 1 LED is blinking.


**5. With the WeakAlternateWeak drive strength setting, what is the average current for 1 complete on-off cycle for 2 LEDs (both on at the time same and both off at the same time) with an on-off duty cycle of 50% (approximately 1 sec on, 1 sec off)?**
   Answer: 4.93mA is the average current for one complete on-off cycle when both LED's are blinking.


