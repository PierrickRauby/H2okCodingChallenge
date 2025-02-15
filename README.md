# H2okCodingChallenge

## Challenge Description

H2Ok Innovations is developing a Smart Factory System (SFS). The factory in mind has two
types of sensors, Type A and B, and one type of programmable logic controller (PLC), which
contains operational data. Write 3 software components for the system. Each component should
be a separate executable, as described below:

- Component 1 reads from both types of sensors once per second. Each reading
produces a record consisting of a timestamp and an array of data.
- Component 2 reads from a PLC once per second. Each reading also produces a record
consisting of a timestamp and an array of data.
- Component 3 combines the data produced by Component 1 and 2, and generates a final
data record, each consisting of a sensor reading record and a PLC data record with their
original timestamps preserved. It should try to align the two records over the timestamps
as best as it can.

If Component 3 is offline temporarily, for example, due to software upgrade, the data produced
by Component 1 and 2 must be preserved. When the Component 3 is back online again, it will
process the data.

The number of sensors as well as the type of each are unknown at the development time. It’s
not allowed to recompile the software or modify the Python script for different sets of sensors.
Using a configuration file is highly desirable.

The software should handle sensor or PLC offline conditions gracefully and report them.

The underlying communication protocols between the SFS and the sensors or PLC are omitted
in the exercise. Using stub functions to return simulated values for the sensors and PLC data is
sufficient.

If any details you feel are missing for you to complete the software, please make necessary
assumptions and write the software accordingly. If you believe what’s missing is critical, please
feel free to reach out to us for clarification.

You can use C/C++, Python, Java, Go, or any programming language that you are most
comfortable with.

Bonus point: use UML diagrams to describe your software.
Bonus point: structure the software so it’s easy for unit tests, and write a unit test.

Note: Using AI code generation software for this exercise is prohibited. Doing so will
disqualify you from consideration.

Copyright © 2025 H2Ok Innovations, Inc
