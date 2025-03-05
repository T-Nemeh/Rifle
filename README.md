 **Rifle System Simulation Using DEVS**

## **Project Overview**
This project implements a **discrete-event simulation** of a **rifle system** using the **DEVS (Discrete Event System Specification) formalism**. The simulation models key components of the rifle, including the **bullet, magazine, bolt assembly, chamber, and trigger assembly**, to analyze their interactions under different firing conditions. The simulation was built and executed using the **Cadmium DEVS framework**.

---

## **Project Structure**
The repository contains the following key components:

**src/** – Source code for atomic and coupled DEVS models   
**logs/** – Execution logs and output results  
**Rifle_System_Simulation_Report.pdf** – Detailed report on the model and test results  
**Makefile** – Compilation and execution script for running the simulation  
**README.md** – This document  

---

## **Model Components**

The simulation consists of several **atomic** and **coupled** models that interact to simulate rifle operation:

| **Component** | **Type** | **Description** |
|--------------|---------|----------------|
| **Bullet** | Atomic | Determines if a bullet is ready and whether it is a dud (5% probability). |
| **Magazine** | Atomic | Tracks bullet count and seating status. |
| **Bolt Assembly** | Atomic | Controls bullet loading and bolt movement. |
| **Chamber** | Atomic | Handles bullet firing and extraction. |
| **Trigger Assembly** | Atomic | Manages trigger presses and firing mode. |
| **Magazine Assembly** | Coupled | Combines **Bullet** and **Magazine** models. |
| **Rifle System** | Coupled | Integrates all components to simulate complete rifle operation. |

---

## **How to Run the Simulation**

### **1. Clone the Repository**
```bash
git clone https://github.com/T-Nemeh/Rifle.git
cd Rifle
```

### **2. Compile the Project**
Ensure you have **Cadmium DEVS installed**. Then compile using:
```bash
source build_sim.sh
```

### **3. Run the Simulation**
Execute the compiled simulation binary:
```bash
./bin/sample_project
```

### **4. View the Output Logs**
Simulation results are saved in the `logs/` directory. To inspect the log output:
```bash
cat logs/log.txt
```

---

## **Test Cases and Validation**

The system has been tested under multiple scenarios:

**Bullet Readiness Test** – Ensures bullets are marked as "ready" and dud detection works correctly.  
**Magazine Seating Test** – Verifies that bullets can only be loaded when the magazine is properly seated.  
**Bolt Cycling Test** – Confirms that bullets are loaded only when the bolt is pulled back.  
 **Trigger Mode Test** – Tests rifle behavior in safe, single, and automatic firing modes.  
 **Full System Test** – Simulates realistic firing sequences with magazine reloads.  

---

## **Expected Behavior**

**When a bullet is loaded:** The **Bullet model** determines whether it is a dud (`5% chance`).  
**If the magazine is unseated:** No bullets can be loaded into the chamber.  
**If the bolt is forward:** No bullets are fired until the bolt is pulled back.  
**Trigger behavior:**  
   - **Safe mode (`0`)** – No firing.  
   - **Single mode (`1`)** – Fires one bullet per trigger pull.  
   - **Auto mode (`2`)** – Continuous firing when the trigger is held.  

---

 

---


---

## **By:**
- **[Tala Nemeh 101170694]**

---

