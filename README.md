keystroke-authentication
========================

Windows application for keystroke athentication.

Program can work in 2 modes:

Training mode Authentication mode In training mode user selects login and password. Login should be at least 4 characters long, password should be at least 6 characters long. After 10 reenters of password application computes mean value and standard deviation of keypress time, than program computes anomaly score based on distance from mean value for each sample of password. Than threshold is created based on computed values. In current implementation scaled manhattan metric is used.

In authentication mode apllication checks password and anomaly score of it
