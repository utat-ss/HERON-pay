// Use this playground to verify that the pressure sensor data conversions are being handled properly (especially with 64-bit numbers)
// Should add print statements for intermediate results in pay/src/env_sensors.c
// Datasheet: http://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5803-05BA%7FB3%7Fpdf%7FEnglish%7FENG_DS_MS5803-05BA_B3.pdf%7FCAT-BLPS0011


import Foundation

var C1: Int
var C2: Int
var C3: Int
var C4: Int
var C5: Int
var C6: Int

var D1: Int
var D2: Int

var dT_1: Int
var dT: Int

var TEMP_1: Int
var TEMP: Int

var OFF_1: Int
var OFF_2: Int
var OFF: Int

var SENS_1: Int
var SENS_2: Int
var SENS: Int

var P_1: Int
var P: Int




// Typical parameters (from datasheet)

C1 = 32_428
C2 = 28_100
C3 = 44_250
C4 = 20_003
C5 = 32_747
C6 = 28_266

D1 = 5_251_266
D2 = 8_383_686

dT_1 = (C5 * (1 << 8))
dT = D2 - dT_1

TEMP_1 = (dT * C6 / (1 << 23))
TEMP = 2000 + TEMP_1

OFF_1 = (C2 * (1 << 18))
OFF_2 = (C4 * dT / (1 << 5))
OFF = OFF_1 + OFF_2

SENS_1 = C1 * (1 << 17)
SENS_2 = C3 * dT / (1 << 7)
SENS = SENS_1 + SENS_2

P_1 = D1 * SENS / (1 << 21)
P = (P_1 - OFF) / (1 << 15)

print("Pressure = \(Double(P) / 100.0) mbar = \(Double(P) / 100.0 / 10.0) kPa")




// Actual parameters (print C1-C6 and D1-D2 from AVR and change values here to update conversions)

C1 = 27361
C2 = 22557
C3 = 33834
C4 = 15128
C5 = 33279
C6 = 28322

D1 = 5366344
D2 = 8650886

dT_1 = (C5 * (1 << 8))
dT = D2 - dT_1

TEMP_1 = (dT * C6 / (1 << 23))
TEMP = 2000 + TEMP_1

OFF_1 = (C2 * (1 << 18))
OFF_2 = (C4 * dT / (1 << 5))
OFF = OFF_1 + OFF_2

SENS_1 = C1 * (1 << 17)
SENS_2 = C3 * dT / (1 << 7)
SENS = SENS_1 + SENS_2

P_1 = D1 * SENS / (1 << 21)
P = (P_1 - OFF) / (1 << 15)

print("Pressure = \(Double(P) / 100.0) mbar = \(Double(P) / 100.0 / 10.0) kPa")



