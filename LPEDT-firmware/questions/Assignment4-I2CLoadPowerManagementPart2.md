Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.* 

1. What is the average current per period?
   Answer: 13.75 uA
   <br>Screenshot:  
   ![Avg_current_per_period](https://github.com/CU-ECEN-5823/ecen5823-assignment4-PradyumnaGudluru/blob/master/Screenshots/Avg_current_per_period.png)  

2. What is the average current when the Si7021 is Powered Off?
   Answer: 8.66 uA
   <br>Screenshot:  
   ![Avg_current_LPM_Off](https://github.com/CU-ECEN-5823/ecen5823-assignment4-PradyumnaGudluru/blob/master/Screenshots/avg_current_lpm_off.png)  

3. What is the average current when the Si7021 is Powered On?
   Answer: 246.77 uA
   <br>Screenshot:  
   ![Avg_current_LPM_On](https://github.com/CU-ECEN-5823/ecen5823-assignment4-PradyumnaGudluru/blob/master/Screenshots/avg_current_lpm_on.png)  

4. How long is the Si7021 Powered On for 1 temperature reading?
   Answer: 102.50ms
   <br>Screenshot:  
   ![duration_lpm_on](https://github.com/CU-ECEN-5823/ecen5823-assignment4-PradyumnaGudluru/blob/master/Screenshots/duration_lpm_on.png)  

5. Compute what the total operating time of your design for assignment 4 would be in hours, assuming a 1000mAh battery power supply?
   Answer: With a avg. current of 13.75 uA per 3000ms, the total operating time in hours is approximately 60.60 hrs.
			Operating Time = Battery Capacity/ avg. current for 1hr = 1000/(0.01375*20*60) = 60.60 hrs.
   
6. How has the power consumption performance of your design changed since the previous assignment?
   Answer: The power consumption has rapidly decreased from the previous assignment. Since, the design runs in EM3 mode and goes to EM1 mode for I2C transfer,
			the performance is increased.
   
7. Describe how you have tested your code to ensure you are sleeping in EM1 mode during I2C transfers.
   Answer: As per the energy profiler, there is a spike for the amount of time wait by the LETIMER0, where the source code is programmed to
			sleep in EM1 mode. Hence can be explained from the output signle node signal.
   

