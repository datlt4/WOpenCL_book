[<img src= "https://upload.wikimedia.org/wikipedia/commons/thumb/4/4d/OpenCL_logo.svg/1200px-OpenCL_logo.svg.png" width=522 alignment="center">]()

# Introducing OpenCL

- Abbreviation of Open computing language

- Parallel programming assigns computational tasks to multiple processing elements to be performed at the same time.

    - Parallel programming assigns computational **tasks** to multiple processing elements to be performed at the same time.

    - These **tasks** are called **kernels**. A **kernel** is a specially coded function that’s intended to be executed by one or more OpenCL-compliant devices.

    - **Kernels** are sent to their intended device or devices by **host applications**. A **host application** is a regular **C/C++ application** running on the user’s development system.

    - **Hosts applications** manage their **connected devices** using a **container** called a **context**.

    - To create a **kernel**, the **host** selects a function from a **kernel container** called a **program**. Then it **associates** the **kernel** with **argument data** and dispatches it to a structure called a **command queue**. The **command queue** is the mechanism through which the host **tells devices what to do**, and when a **kernel** is **enqueued**, the device will execute the corresponding function.

    - An OpenCL application can configure different **devices** to perform different **tasks**, and each **task** can operate on **different data**. In other words, OpenCL provides **full task-parallelism**. This is an important advantage over many other **parallel-programming** toolsets, which only enable **data-parallelism**. In a data-parallel system, each device receives the same instructions but operates on different sets of data.

- Strange data structure:

    - platforms
    - contexts
    - devices
    - programs
    - kernels
    - command queues

[<img src= "images/F1_1.png" width="522">]()

